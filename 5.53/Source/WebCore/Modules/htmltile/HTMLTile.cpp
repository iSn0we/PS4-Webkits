/*
 * Copyright (C) 2016 Sony Interactive Entertainment Inc.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "HTMLTile.h"

#if ENABLE(MANX_HTMLTILE)

#include "CustomEvent.h"
#include "Document.h"
#include "Event.h"
#include "HTMLTileClient.h"
#include "LocationChangeEvent.h"
#include "Page.h"
#include <ArrayBuffer.h>

using namespace WTF;

namespace WebCore {

PassRefPtr<HTMLTile> HTMLTile::create(ScriptExecutionContext& context, int width, int height)
{
    RefPtr<HTMLTile> newTile = adoptRef(new HTMLTile(context, width, height));
    newTile->attachToBackend();
    newTile->suspendIfNeeded();

    return newTile.release();
}

HTMLTile::HTMLTile(ScriptExecutionContext& context, int width, int height)
    : ActiveDOMObject(&context)
    , m_width(width)
    , m_height(height)
    , m_state(Invalid)
    , m_client(0)
    , m_tileID(0)
    , m_nextCallbackID(1)
{
    ASSERT(is<Document>(context));
    Document& document = downcast<Document>(context);
    m_client = document.page()->htmlTileClient();
}

HTMLTile::~HTMLTile()
{
    destroy();
}

void HTMLTile::destroy()
{
    if (!isValid() && !isAttaching())
        return;

    detachFromBackend();
    m_evalCallbacks.clear();
}

bool HTMLTile::isValid() const
{
    return m_state & Valid;
}

bool HTMLTile::isPaused() const
{
    return m_state & Paused;
}

bool HTMLTile::isAttaching() const
{
    return m_state & Attaching;
}

int HTMLTile::width() const
{
    return isValid() ? m_width : 0;
}

int HTMLTile::height() const
{
    return isValid() ? m_height : 0;
}

PassRefPtr<ImageData> HTMLTile::imageData() const
{
    return isValid() ? m_data : RefPtr<ImageData>() /*null*/;
}

String HTMLTile::url() const
{
    return isValid() ? m_url : String();
}

void HTMLTile::setUrl(const String &url)
{
    if (url == m_url)
        return;

    m_url = url;

    if (!isValid())
        return;

    ASSERT(m_client);
    m_client->loadURL(m_tileID, url);
}

void HTMLTile::setValid()
{
    ASSERT(m_client);

    setState(Valid);
    resetState(Attaching);

    if (!m_url.isEmpty())
        m_client->loadURL(m_tileID, m_url);
}

void HTMLTile::attachToBackend()
{
    ASSERT(m_client && !m_tileID);
    ASSERT(!isAttaching());

    // Limit the tiles to a maximum size of 1920*1080
    if (m_width <= 0 || m_height <= 0 || m_width > 1920 || m_height > 1080)
        return;

    const unsigned char* imageData = 0;
    size_t imageDataSize = 0;
    m_tileID = m_client->createTile(m_width, m_height, imageData, imageDataSize, this);

    if (!m_tileID)
        return;

    if (!imageData || !imageDataSize) {
        m_tileID = 0;
        return;
    }

    JSC::ArrayBufferContents contents;
    JSC::ArrayBufferContents::fromExternalData(const_cast<unsigned char*>(imageData), imageDataSize, contents);

    m_data = ImageData::create(IntSize(m_width, m_height), Uint8ClampedArray::create(ArrayBuffer::create(contents), 0, imageDataSize));

    setState(Attaching);
}

void HTMLTile::detachFromBackend()
{
    ASSERT(m_client && m_tileID);

    resetState(Attaching);
    resetState(Valid);

    // invalidate ArrayBufferContents in case there are still references to imageData on the JS side
    JSC::ArrayBufferContents externalData; // will be deleted at the end of the scope
    m_data->data()->buffer()->transfer(externalData);
    m_data = nullptr;

    m_client->destroyTile(m_tileID);
    m_client = nullptr;

    m_tileID = 0;
}

void HTMLTile::pause()
{
    if (!isValid() || isPaused())
        return;

    ASSERT(m_client);
    m_client->pause(m_tileID);

    setState(Paused);
}

void HTMLTile::resume()
{
    if (!isValid() || !isPaused())
        return;

    ASSERT(m_client);
    m_client->resume(m_tileID);

    resetState(Paused);
}

void HTMLTile::sendEvent(unsigned short eventType)
{
    if (!isValid() || isPaused())
        return;

    ASSERT(m_client);
    m_client->sendEvent(m_tileID, eventType);
}

void HTMLTile::eval(const String& jsScript, PassRefPtr<StringCallback> callback)
{
    // PENDING what about paused/unresponsive states?
    // should we discard, delay until the tile is available again, or...?
    if (!isValid())
        return;

    ASSERT(m_client);

    uint64_t callbackID = 0;
    if (callback) {
        callbackID = m_nextCallbackID++;
        m_evalCallbacks.add(callbackID, callback);
    }
    m_client->eval(m_tileID, jsScript, callbackID);
}

void HTMLTile::dispatchOnLoadEvent()
{
    ASSERT(isValid());

    dispatchEvent(Event::create(eventNames().loadEvent, false, false));
}

void HTMLTile::dispatchOnUpdateEvent()
{
    ASSERT(isValid());

    dispatchEvent(Event::create(eventNames().updateEvent, false, false));
}

void HTMLTile::dispatchOnErrorEvent(int errorCode)
{
    ASSERT(isValid());

    CustomEventInit eventInit;
    eventInit.bubbles = false;
    eventInit.cancelable = false;
    // TODO: Replaced with non-deprecated code.
    eventInit.detail = Deprecated::ScriptValue(scriptExecutionContext()->vm(), JSC::JSValue(errorCode));
    dispatchEvent(CustomEvent::create(eventNames().errorEvent, eventInit));
}

void HTMLTile::dispatchOnLocationChangeEvent(const String &url)
{
    ASSERT(isValid());

    RefPtr<LocationChangeEvent> locationChangeEvent = LocationChangeEvent::create(url, false /*locationChangeAllowed*/);
    dispatchEvent(locationChangeEvent);

    if (locationChangeEvent->locationChangeAllowed())
        setUrl(url);
}

void HTMLTile::dispatchOnUnresponsiveEvent()
{
    ASSERT(isValid());

    dispatchEvent(Event::create(eventNames().unresponsiveEvent, false, false));
}

void HTMLTile::dispatchOnCrashEvent()
{
    ASSERT(isValid());

    dispatchEvent(Event::create(eventNames().crashEvent, false, false));
}

void HTMLTile::dispatchEvalCallback(uint64_t callbackID, const String& result)
{
    ASSERT(m_evalCallbacks.contains(callbackID));

    RefPtr<StringCallback> callback = m_evalCallbacks.take(callbackID);
    callback->scheduleCallback(scriptExecutionContext(), result);
}

const char* HTMLTile::activeDOMObjectName() const
{
    return "HTMLTile";
}

EventTargetInterface HTMLTile::eventTargetInterface() const
{
    return HTMLTileEventTargetInterfaceType;
}

ScriptExecutionContext* HTMLTile::scriptExecutionContext() const
{
    return ActiveDOMObject::scriptExecutionContext();
}

bool HTMLTile::canSuspendForPageCache() const
{
    return true;
}

void HTMLTile::suspend(ReasonForSuspension reason)
{
    if (reason != JavaScriptDebuggerPaused)
        pause();
}

void HTMLTile::stop()
{
    destroy();
}

EventTargetData* HTMLTile::eventTargetData()
{
    return &m_eventTargetData;
}

EventTargetData& HTMLTile::ensureEventTargetData()
{
    return m_eventTargetData;
}

void HTMLTile::setState(TileState state)
{
    m_state |= state;
}

void HTMLTile::resetState(TileState state)
{
    m_state &= ~state;
}

} // namespace WebCore

#endif // ENABLE(MANX_HTMLTILE)
