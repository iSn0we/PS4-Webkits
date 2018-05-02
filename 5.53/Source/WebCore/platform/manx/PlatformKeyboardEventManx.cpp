/*
 * Copyright (C) 2008 Apple Computer, Inc.  All rights reserved.
 * Copyright (C) 2012 Sony Interactive Entertainment Inc.
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
#include "PlatformKeyboardEvent.h"

#include "WindowsKeyboardCodes.h"
#include <wtf/CurrentTime.h>

namespace WebCore {

PlatformEvent::Type eventTypeForKeyboardEventType(Manx::KeyboardEvent::Type type)
{
    switch (type) {
    case Manx::KeyboardEvent::KeyDown:
        return PlatformEvent::KeyDown;
    case Manx::KeyboardEvent::KeyUp:
        return PlatformEvent::KeyUp;
    case Manx::KeyboardEvent::RawKeyDown:
        return PlatformEvent::RawKeyDown;
    case Manx::KeyboardEvent::Char:
        return PlatformEvent::Char;
    }
    return PlatformEvent::NoType;
}

PlatformKeyboardEvent::PlatformKeyboardEvent(const Manx::KeyboardEvent& event)
    : PlatformEvent(eventTypeForKeyboardEventType(event.m_type), Modifiers((event.m_shiftKey ? ShiftKey : 0) | (event.m_controlKey ? CtrlKey : 0) | (event.m_altKey ? AltKey : 0) | (event.m_metaKey ? MetaKey : 0)), WTF::currentTime())
    , m_text(event.m_text)
    , m_unmodifiedText(event.m_unmodifiedText)
    , m_keyIdentifier(event.m_keyIdentifier)
    , m_windowsVirtualKeyCode(event.m_windowsVirtualKeyCode)
    , m_nativeVirtualKeyCode(event.m_nativeVirtualKeyCode)
    , m_macCharCode(event.m_macCharCode)
    , m_autoRepeat(event.m_isAutoRepeat)
    , m_isKeypad(event.m_isKeypad)
    , m_isSystemKey(event.m_isSystemKey)
{
    if (event.m_windowsVirtualKeyCode == VK_RETURN) {
        m_text = "\r";
        m_unmodifiedText = m_text;
    }
}

void PlatformKeyboardEvent::disambiguateKeyDownEvent(Type type, bool)
{
    // Can only change type from KeyDown to RawKeyDown or Char, as we lack information for other conversions.
    ASSERT(m_type == KeyDown);
    m_type = type;

    if (type == RawKeyDown) {
        m_text = String();
        m_unmodifiedText = String();
    } else {
        m_keyIdentifier = String();
        m_windowsVirtualKeyCode = 0;
    }
}

bool PlatformKeyboardEvent::currentCapsLockState()
{
    return false;
}

void PlatformKeyboardEvent::getCurrentModifierState(bool& shiftKey, bool& ctrlKey, bool& altKey, bool& metaKey)
{
    shiftKey = false;
    ctrlKey = false;
    altKey = false;
    metaKey = false;
}

}
