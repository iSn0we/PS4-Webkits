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
#include "WebTileManagerProxy.h"

#if ENABLE(MANX_HTMLTILE)

#include "UpdateInfo.h"
#include "WebPageGroup.h"
#include "WebPreferences.h"
#include "WebProcessPool.h"
#include "WebTileManagerMessages.h"
#include "WebTileManagerProxyMessages.h"
#include "WebTileProxy.h"

#include <WebCore/HTMLTile.h>

namespace WebKit {

const char* WebTileManagerProxy::supplementName()
{
    return "WebTileManagerProxy";
}

PassRefPtr<WebTileManagerProxy> WebTileManagerProxy::create(WebProcessPool* processPool)
{
    return adoptRef(new WebTileManagerProxy(processPool));
}

WebTileManagerProxy::WebTileManagerProxy(WebProcessPool* processPool)
    : WebContextSupplement(processPool)
    , m_webTilesPageGroup(WebPageGroup::create("WebTilePageGroup"))
{
    WebContextSupplement::processPool()->addMessageReceiver(Messages::WebTileManagerProxy::messageReceiverName(), *this);

    // Disable AC and WebGL, not supported by HTMLTile.
    // For safety, we also disable everything not explicitly required for HTMLTile
    WebPreferences& tilePreferences = m_webTilesPageGroup->preferences();

#define DISABLE_BOOL_PREFERENCE(KeyUpper, KeyLower, TypeName, Type, DefaultValue) if (DefaultValue) tilePreferences.set##KeyUpper(false);

FOR_EACH_WEBKIT_BOOL_PREFERENCE(DISABLE_BOOL_PREFERENCE)

#undef DISABLE_BOOL_PREFERENCE

    // these are explicitly allowed, see BUG 32374
    tilePreferences.setJavaScriptEnabled(true);
    tilePreferences.setJavaScriptMarkupEnabled(true);
    tilePreferences.setRequestAnimationFrameEnabled(true);
    tilePreferences.setLoadsImagesAutomatically(true);
    tilePreferences.setAuthorAndUserStylesEnabled(true);
    tilePreferences.setXSSAuditorEnabled(true);
    tilePreferences.setWebSecurityEnabled(true);

    tilePreferences.setCookieEnabled(true);
    tilePreferences.setLocalStorageEnabled(true);
    tilePreferences.setOfflineWebApplicationCacheEnabled(true);

    // enable for remote web inspector access
    tilePreferences.setDeveloperExtrasEnabled(true);
}

void WebTileManagerProxy::createWebTile(uint64_t tileID, uint32_t width, uint32_t height, uint64_t pageID)
{
    ASSERT(!m_webTiles.contains(tileID));

    RefPtr<WebTileProxy> webTile = WebTileProxy::create(this, m_webTilesPageGroup.get());
    webTile->setTileID(tileID);
    webTile->setTileSize(width, height);
    webTile->setParentPageID(pageID);

    m_webTiles.set(tileID, webTile);

    dispatchTileValid(tileID);

    if (m_paused)
        webTile->pause();
}

void WebTileManagerProxy::destroyWebTile(uint64_t tileID)
{
    if (!m_webTiles.contains(tileID))
        return;

    m_webTiles.remove(tileID);
}

void WebTileManagerProxy::pauseWebTile(uint64_t tileID)
{
    if (!m_webTiles.contains(tileID))
        return;

    m_webTiles.get(tileID)->pause();
}

void WebTileManagerProxy::resumeWebTile(uint64_t tileID)
{
    if (!m_webTiles.contains(tileID))
        return;

    m_webTiles.get(tileID)->resume();
}

void WebTileManagerProxy::sendWebTileEvent(uint64_t tileID, uint32_t eventType)
{
    if (!m_webTiles.contains(tileID))
        return;

    RefPtr<WebTileProxy> webTile = m_webTiles.get(tileID);

    switch (eventType) {
    case WebCore::HTMLTile::FocusIn: webTile->setFocused(true); break;
    case WebCore::HTMLTile::FocusOut: webTile->setFocused(false); break;
    case WebCore::HTMLTile::ButtonPress: webTile->setClicked(true); break;
    case WebCore::HTMLTile::ButtonRelease: webTile->setClicked(false); break;
    }
}

void WebTileManagerProxy::runJavaScriptInWebTile(uint64_t tileID, const String &jsScript, uint64_t callbackID)
{
    if (!m_webTiles.contains(tileID))
        return;

    m_webTiles.get(tileID)->evalScript(jsScript, callbackID);
}

void WebTileManagerProxy::didChangeWebTileURL(uint64_t tileID, const String& url)
{
    if (!m_webTiles.contains(tileID))
        return;

    m_webTiles.get(tileID)->setTileURL(url);
}

void WebTileManagerProxy::didUpdateWebTile(uint64_t tileID)
{
    if (!m_webTiles.contains(tileID))
        return;

    m_webTiles.get(tileID)->didUpdate();
}

WebTileManagerProxy::~WebTileManagerProxy()
{
    if (m_webTilesProcessPool)
        m_webTilesProcessPool->enableProcessTermination();
}

WebProcessPool* WebTileManagerProxy::webTilesProcessPool()
{
    if (!m_webTilesProcessPool) {
        auto configuration = API::ProcessPoolConfiguration::createWithLegacyOptions();
        configuration->setProcessModel(ProcessModelSharedSecondaryProcess);
        m_webTilesProcessPool = WebProcessPool::create(configuration.get()); // no injected bundle for the tiles context

        // Replace the parent context's webprocess binary with our own, which is expected to be in the same directory
        const String webProcessDir = processPool()->webProcessPath().left(processPool()->webProcessPath().reverseFind('/')+1);
        m_webTilesProcessPool->setWebProcessPath(webProcessDir + "WebProcessHTMLTile.self");
        m_webTilesProcessPool->setUserID(processPool()->userID());

        static const String suffix = "/htmltile";
        if (!processPool()->cookieDatabaseDirectory().isEmpty()) 
            m_webTilesProcessPool->setCookieStorageDirectory(processPool()->cookieDatabaseDirectory() + suffix);
        if (!processPool()->configuration().applicationCacheDirectory().isEmpty()) 
            m_webTilesProcessPool->configuration().setApplicationCacheDirectory(processPool()->configuration().applicationCacheDirectory() + suffix);
        if (!processPool()->configuration().diskCacheDirectory().isEmpty()) 
            m_webTilesProcessPool->configuration().setDiskCacheDirectory(processPool()->configuration().diskCacheDirectory() + suffix);
        if (!processPool()->configuration().indexedDBDatabaseDirectory().isEmpty())
            m_webTilesProcessPool->configuration().setIndexedDBDatabaseDirectory(processPool()->configuration().indexedDBDatabaseDirectory() + suffix);
        if (!processPool()->configuration().localStorageDirectory().isEmpty())
            m_webTilesProcessPool->configuration().setLocalStorageDirectory(processPool()->configuration().localStorageDirectory() + suffix);
        if (!processPool()->configuration().webSQLDatabaseDirectory().isEmpty())
            m_webTilesProcessPool->configuration().setWebSQLDatabaseDirectory(processPool()->configuration().webSQLDatabaseDirectory() + suffix);
        if (!processPool()->configuration().mediaKeysStorageDirectory().isEmpty()) 
            m_webTilesProcessPool->configuration().setMediaKeysStorageDirectory(processPool()->configuration().mediaKeysStorageDirectory() + suffix);
    }
    return m_webTilesProcessPool.get();
}

void WebTileManagerProxy::processDidClose(WebProcessProxy*)
{
    m_webTiles.clear();
}

void WebTileManagerProxy::processPoolDestroyed()
{
    m_webTiles.clear();
}

bool WebTileManagerProxy::shouldTerminate(WebProcessProxy*) const
{
    return m_webTiles.isEmpty();
}

void WebTileManagerProxy::refWebContextSupplement()
{
    ref();
}

void WebTileManagerProxy::derefWebContextSupplement()
{
    deref();
}

void WebTileManagerProxy::dispatchTileValid(uint64_t tileID) const
{
    if (!m_webTiles.contains(tileID))
        return;

    RefPtr<WebTileProxy> tile = m_webTiles.get(tileID);
    if (WebPageProxy* parentPage = tile->parentPage())
        parentPage->process().send(Messages::WebTileManager::TileValid(tileID), tile->parentPageID());
}

void WebTileManagerProxy::dispatchTileUpdated(uint64_t tileID, const UpdateInfo &updateInfo) const
{
    if (!m_webTiles.contains(tileID))
        return;

    RefPtr<WebTileProxy> tile = m_webTiles.get(tileID);
    if (WebPageProxy* parentPage = tile->parentPage())
        parentPage->process().send(Messages::WebTileManager::TileUpdated(tileID, updateInfo), tile->parentPageID());
}

void WebTileManagerProxy::dispatchTileUnresponsive(uint64_t tileID) const
{
    if (!m_webTiles.contains(tileID))
        return;

    RefPtr<WebTileProxy> tile = m_webTiles.get(tileID);
    if (WebPageProxy* parentPage = tile->parentPage())
        parentPage->process().send(Messages::WebTileManager::TileUnresponsive(tileID), tile->parentPageID());
}

void WebTileManagerProxy::dispatchTileCrashed(uint64_t tileID) const
{
    if (!m_webTiles.contains(tileID))
        return;

    RefPtr<WebTileProxy> tile = m_webTiles.get(tileID);
    if (WebPageProxy* parentPage = tile->parentPage())
        parentPage->process().send(Messages::WebTileManager::TileCrashed(tileID), tile->parentPageID());
}

void WebTileManagerProxy::dispatchLoadCompleted(uint64_t tileID) const
{
    if (!m_webTiles.contains(tileID))
        return;

    RefPtr<WebTileProxy> tile = m_webTiles.get(tileID);
    if (WebPageProxy* parentPage = tile->parentPage())
        parentPage->process().send(Messages::WebTileManager::LoadCompleted(tileID), tile->parentPageID());
}

void WebTileManagerProxy::dispatchLoadFailed(uint64_t tileID, uint32_t errorCode) const
{
    if (!m_webTiles.contains(tileID))
        return;

    RefPtr<WebTileProxy> tile = m_webTiles.get(tileID);
    if (WebPageProxy* parentPage = tile->parentPage())
        parentPage->process().send(Messages::WebTileManager::LoadFailed(tileID, errorCode), tile->parentPageID());
}

void WebTileManagerProxy::dispatchLocationChangeRequested(uint64_t tileID, const String& url) const
{
    if (!m_webTiles.contains(tileID))
        return;

    RefPtr<WebTileProxy> tile = m_webTiles.get(tileID);
    if (WebPageProxy* parentPage = tile->parentPage())
        parentPage->process().send(Messages::WebTileManager::LocationChangeRequested(tileID, url), tile->parentPageID());
}

void WebTileManagerProxy::dispatchRunJavaScriptResultCallback(uint64_t tileID, uint64_t callbackID, const String& result) const
{
    if (!m_webTiles.contains(tileID))
        return;

    RefPtr<WebTileProxy> tile = m_webTiles.get(tileID);
    if (WebPageProxy* parentPage = tile->parentPage())
        parentPage->process().send(Messages::WebTileManager::RunJavaScriptResultCallback(tileID, callbackID, result), tile->parentPageID());
}

void WebTileManagerProxy::pauseWebTiles()
{
    for (auto i : m_webTiles)
        i.value->pause();

    m_paused = true;
}

void WebTileManagerProxy::resumeWebTiles()
{
    for (auto i : m_webTiles)
        i.value->resume();

    m_paused = false;
}

bool WebTileManagerProxy::isPaused()
{
    return m_paused;
}

} // namespace WebKit

#endif // ENABLE(MANX_HTMLTILE)
