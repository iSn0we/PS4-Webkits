/*
 * Copyright (C) 2017 Sony Interactive Entertainment Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "WebTileManager.h"

#if ENABLE(MANX_HTMLTILE)

#include "DOMWrapperWorld.h"
#include "HTMLTile.h"
#include "JSDOMWindow.h"
#include "JSHTMLTile.h"
#include "ScriptState.h"
#include "ShareableBitmap.h"
#include "UpdateInfo.h"
#include "WebFrame.h"
#include "WebPage.h"
#include "WebProcess.h"
#include "WebTile.h"
#include "WebTileManagerMessages.h"
#include "WebTileManagerProxyMessages.h"

using namespace WebCore;

namespace WebKit {

static uint64_t generateWebTileID()
{
    static uint64_t uniqueWebTileID = 1;
    return uniqueWebTileID++;
}

WebTileManager::WebTileManager(WebPage* page)
    : m_webPage(page)
{
    WebProcess::singleton().addMessageReceiver(Messages::WebTileManager::messageReceiverName(), page->pageID(), *this);
}

WebTileManager::~WebTileManager()
{
    WebProcess::singleton().removeMessageReceiver(Messages::WebTileManager::messageReceiverName(), m_webPage->pageID());
}

bool WebTileManager::enableHTMLTileForFrame(WebFrame* frame, DOMWrapperWorld& world)
{
    if (frame->page() != m_webPage)
        return false;

    JSDOMWindow* window = toJSDOMWindow(frame->coreFrame(), world);
    ScriptState* exec = window->globalExec();
    JSC::Identifier htmlTileIdentifier = JSC::Identifier::fromString(exec, "HTMLTile");
    JSC::JSValue htmlTileConstructor = JSHTMLTile::getConstructor(exec->vm(), window);

    window->putDirect(exec->vm(), htmlTileIdentifier, htmlTileConstructor);

    return true;
}

void WebTileManager::tileUpdated(uint64_t tileID, const UpdateInfo& updateInfo)
{
    if (!m_tiles.contains(tileID)) {
        // We must still "consume" updateInfo.bitmapHandle to ensure the backing shared memory segment is released.
        // This happens automatically from ShareableBitmap::create():
        // - create() returns a PassRefPtr<ShareableBitmap> which is deleted right after.
        // - The PassRefPtr destructor derefs the ShareableBitmap object, which ends up deleted too since there was only one ref.
        // - The SharedMemory member gets deleted in turn, which is where the shared memory segment is released.
        ShareableBitmap::create(updateInfo.bitmapHandle);
        return;
    }

    m_tiles.get(tileID)->webTile->tileUpdated(updateInfo);
    m_tiles.get(tileID)->htmlTile->dispatchOnUpdateEvent(/*TODO updateInfo.updateRects*/);

    m_webPage->send(Messages::WebTileManagerProxy::DidUpdateWebTile(tileID), 0);
}

void WebTileManager::tileValid(uint64_t tileID)
{
    if (!m_tiles.contains(tileID))
        return;

    m_tiles.get(tileID)->htmlTile->setValid();
}

void WebTileManager::tileUnresponsive(uint64_t tileID)
{
    if (!m_tiles.contains(tileID))
        return;

    m_tiles.get(tileID)->htmlTile->dispatchOnUnresponsiveEvent();
}

void WebTileManager::tileCrashed(uint64_t tileID)
{
    if (!m_tiles.contains(tileID))
        return;

    m_tiles.get(tileID)->htmlTile->dispatchOnCrashEvent();
}

void WebTileManager::loadCompleted(uint64_t tileID)
{
    if (!m_tiles.contains(tileID))
        return;

    m_tiles.get(tileID)->htmlTile->dispatchOnLoadEvent();
}

void WebTileManager::loadFailed(uint64_t tileID, uint32_t errorCode)
{
    if (!m_tiles.contains(tileID))
        return;

    // Note: errorCode() is a CurlCode, see curl.h for the list of values
    m_tiles.get(tileID)->htmlTile->dispatchOnErrorEvent(errorCode);
}

void WebTileManager::locationChangeRequested(uint64_t tileID, const String& url)
{
    if (!m_tiles.contains(tileID))
        return;

    m_tiles.get(tileID)->htmlTile->dispatchOnLocationChangeEvent(url);
}

void WebTileManager::runJavaScriptResultCallback(uint64_t tileID, uint64_t callbackID, const String& result)
{
    if (!m_tiles.contains(tileID))
        return;

    m_tiles.get(tileID)->htmlTile->dispatchEvalCallback(callbackID, result);
}

uint64_t WebTileManager::createTile(int width, int height, const unsigned char*& tileData, size_t& tileDataSize, HTMLTile* htmlTile)
{
    uint64_t tileID = generateWebTileID();

    m_webPage->send(Messages::WebTileManagerProxy::CreateWebTile(tileID, width, height, m_webPage->pageID()), 0);

    ASSERT(!m_tiles.contains(tileID));

    std::unique_ptr<WebTile> newTile = WebTile::create(width, height, this);
    tileData = newTile->tileData();
    tileDataSize = newTile->tileDataSize();

    TileData* td = new TileData { newTile.release(), htmlTile };
    m_tiles.set(tileID, std::unique_ptr<TileData>(td));

    return tileID;
}

void WebTileManager::destroyTile(uint64_t tileID)
{
    m_webPage->send(Messages::WebTileManagerProxy::DestroyWebTile(tileID), 0);

    ASSERT(m_tiles.contains(tileID));

    m_tiles.remove(tileID);
}

void WebTileManager::loadURL(uint64_t tileID, const String &url)
{
    m_webPage->send(Messages::WebTileManagerProxy::DidChangeWebTileURL(tileID, url), 0);
}

void WebTileManager::sendEvent(uint64_t tileID, int eventType)
{
    m_webPage->send(Messages::WebTileManagerProxy::SendWebTileEvent(tileID, eventType), 0);
}

void WebTileManager::pause(uint64_t tileID)
{
    (void)tileID;
    // Disabled. [bug 100976]
    // m_webPage->send(Messages::WebTileManagerProxy::PauseWebTile(tileID), 0);
}

void WebTileManager::resume(uint64_t tileID)
{
    (void)tileID;
    // Disabled. [bug 100976]
    // m_webPage->send(Messages::WebTileManagerProxy::ResumeWebTile(tileID), 0);
}

void WebTileManager::eval(uint64_t tileID, const String& jsScript, uint64_t callbackID)
{
    m_webPage->send(Messages::WebTileManagerProxy::RunJavaScriptInWebTile(tileID, jsScript, callbackID), 0);
}

} // namespace WebKit

#endif // ENABLE(MANX_HTMLTILE)
