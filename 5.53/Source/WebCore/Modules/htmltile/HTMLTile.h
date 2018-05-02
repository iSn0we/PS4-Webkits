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

#ifndef HTMLTile_h
#define HTMLTile_h

#if ENABLE(MANX_HTMLTILE)

#include "ActiveDOMObject.h"
#include "EventTarget.h"
#include "ImageData.h"
#include "StringCallback.h"
#include <wtf/HashMap.h>
#include <wtf/PassRefPtr.h>
#include <wtf/RefCounted.h>
#include <wtf/text/WTFString.h>

#define DEFINE_ATTRIBUTE_EVENT_LISTENER(attribute) \
    EventListener* on##attribute() { return getAttributeEventListener(eventNames().attribute##Event); } \
    void setOn##attribute(PassRefPtr<EventListener> listener) { setAttributeEventListener(eventNames().attribute##Event, listener); } \

namespace WebCore {

class ScriptExecutionContext;
class HTMLTileClient;

class HTMLTile : public RefCounted<HTMLTile>, public EventTarget, public ActiveDOMObject {
public:
    static PassRefPtr<HTMLTile> create(ScriptExecutionContext&, int width, int height);
    virtual ~HTMLTile();

    // Must be kept in sync with the values defined in HTMLTile.idl!
    static const int FocusIn = 0;
    static const int FocusOut = 1;
    static const int ButtonPress = 2;
    static const int ButtonRelease = 3;

    void destroy();

    bool isValid() const;
    bool isPaused() const;
    bool isAttaching() const;

    int width() const;
    int height() const;
    PassRefPtr<ImageData> imageData() const;

    String url() const;
    void setUrl(const String&);

    void pause();
    void sendEvent(unsigned short eventType);
    void eval(const String& jsScript, PassRefPtr<StringCallback>);

    DEFINE_ATTRIBUTE_EVENT_LISTENER(load);
    DEFINE_ATTRIBUTE_EVENT_LISTENER(error);
    DEFINE_ATTRIBUTE_EVENT_LISTENER(update);
    DEFINE_ATTRIBUTE_EVENT_LISTENER(locationchange);
    DEFINE_ATTRIBUTE_EVENT_LISTENER(unresponsive);
    DEFINE_ATTRIBUTE_EVENT_LISTENER(crash);

    WEBCORE_EXPORT void dispatchOnLoadEvent();
    WEBCORE_EXPORT void dispatchOnUpdateEvent();
    WEBCORE_EXPORT void dispatchOnErrorEvent(int errorCode);
    WEBCORE_EXPORT void dispatchOnLocationChangeEvent(const String& url);
    WEBCORE_EXPORT void dispatchOnUnresponsiveEvent();
    WEBCORE_EXPORT void dispatchOnCrashEvent();
    WEBCORE_EXPORT void dispatchEvalCallback(uint64_t callbackID, const String& result);

    using RefCounted<HTMLTile>::ref;
    using RefCounted<HTMLTile>::deref;

    // EventTarget interface
    virtual EventTargetInterface eventTargetInterface() const override;
    virtual ScriptExecutionContext* scriptExecutionContext() const override;

    // ActiveDOMObject interface
    const char* activeDOMObjectName() const override;
    virtual bool canSuspendForPageCache() const override;
    virtual void suspend(ReasonForSuspension) override;
    virtual void resume() override;
    virtual void stop() override;

    WEBCORE_EXPORT void setValid();

private:
    HTMLTile(ScriptExecutionContext&, int width, int height);

    void attachToBackend();
    void detachFromBackend();

    // EventTarget interface
    virtual void refEventTarget() override { ref(); }
    virtual void derefEventTarget() override { deref(); }
    virtual EventTargetData* eventTargetData() override;
    virtual EventTargetData& ensureEventTargetData() override;

    enum TileState {
        Invalid = 0x00,
        Valid = 0x01,
        Paused = 0x10,
        Attaching = 0x20,
    };
    typedef int TileStates; // TileState combination
    void setState(TileState);
    void resetState(TileState);

    int m_width;
    int m_height;
    RefPtr<ImageData> m_data;
    String m_url;
    TileStates m_state;

    HTMLTileClient* m_client;
    uint64_t m_tileID;

    EventTargetData m_eventTargetData;

    uint64_t m_nextCallbackID;
    HashMap<uint64_t, RefPtr<StringCallback> > m_evalCallbacks;
};

} // namespace WebCore

#endif // ENABLE(MANX_HTMLTILE)

#endif // HTMLTile_h
