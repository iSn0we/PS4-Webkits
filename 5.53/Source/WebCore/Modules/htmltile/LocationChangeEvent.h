/*
 * Copyright (C) 2016 Sony Interactive Entertainment Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Sony Computer Entertainment Inc. nor the names
 * of its contributors may be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef LocationChangeEvent_h
#define LocationChangeEvent_h

#if ENABLE(MANX_HTMLTILE)

#include "Event.h"
#include "EventNames.h"

namespace WebCore {

struct LocationChangeEventInit : public EventInit {
    LocationChangeEventInit()
        : locationChangeAllowed(false)
    {
    }

    String url;
    bool locationChangeAllowed;
};

class LocationChangeEvent : public Event {
public:
    static PassRefPtr<LocationChangeEvent> create()
    {
        return adoptRef(new LocationChangeEvent());
    }

    static PassRefPtr<LocationChangeEvent> create(const String& url, bool locationChangeAllowed)
    {
        return adoptRef(new LocationChangeEvent(url, locationChangeAllowed));
    }

    static PassRefPtr<LocationChangeEvent> create(const AtomicString& type, const LocationChangeEventInit& initializer)
    {
        return adoptRef(new LocationChangeEvent(type, initializer));
    }

    String url() const { return m_url; }

    bool locationChangeAllowed() const { return m_locationChangeAllowed; }
    void setLocationChangeAllowed(bool allowed) { m_locationChangeAllowed = allowed; }

    // Event interface
    virtual EventInterface eventInterface() const { return LocationChangeEventInterfaceType; }

private:
    LocationChangeEvent()
        : Event(eventNames().locationchangeEvent, false, false)
        , m_locationChangeAllowed(false)
    {
    }

    LocationChangeEvent(const String& url, bool locationChangeAllowed)
        : Event(eventNames().locationchangeEvent, false, false)
        , m_url(url)
        , m_locationChangeAllowed(locationChangeAllowed)
    {
    }

    LocationChangeEvent(const AtomicString& type, const LocationChangeEventInit& initializer)
        : Event(type, initializer)
        , m_url(initializer.url)
        , m_locationChangeAllowed(initializer.locationChangeAllowed)
    {
    }

    String m_url;
    bool m_locationChangeAllowed;
};

} // namespace WebCore

#endif // ENABLE(MANX_HTMLTILE)

#endif // LocationChangeEvent_h
