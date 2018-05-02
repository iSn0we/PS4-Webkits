/*
 * Copyright (C) 2010 Apple Inc. All rights reserved.
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies)
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
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "WebEventFactory.h"

#include <WebCore/FloatPoint.h>
#include <WebCore/IntPoint.h>
#include <WebCore/PlatformKeyboardEvent.h>
#include <wtf/ASCIICType.h>
#include <wtf/CurrentTime.h>

using namespace WebCore;

namespace WebKit {

WebEvent::Type webEventTypeForEvent(const Manx::KeyboardEvent& event)
{
    switch (event.m_type) {
    case Manx::KeyboardEvent::KeyDown: return WebEvent::KeyDown;
    case Manx::KeyboardEvent::KeyUp: return WebEvent::KeyUp;
    case Manx::KeyboardEvent::RawKeyDown: return WebEvent::RawKeyDown;
    case Manx::KeyboardEvent::Char: return WebEvent::Char;
    }
    ASSERT_NOT_REACHED();
    return WebEvent::KeyDown;
}

WebEvent::Type webEventTypeForEvent(const Manx::MouseEvent& mouseEvent)
{
    switch (mouseEvent.m_type) {
    case Manx::MouseEvent::MouseDown: return WebEvent::MouseDown;
    case Manx::MouseEvent::MouseUp: return WebEvent::MouseUp;
    case Manx::MouseEvent::MouseMove: return WebEvent::MouseMove;
    }
    ASSERT_NOT_REACHED();
    return WebEvent::MouseDown;
}

WebMouseEvent::Button buttonForEvent(const Manx::MouseEvent& mouseEvent)
{
    switch (mouseEvent.m_button) {
    case Manx::MouseEvent::NoButton: return WebMouseEvent::NoButton;
    case Manx::MouseEvent::LeftButton: return WebMouseEvent::LeftButton;
    case Manx::MouseEvent::MiddleButton: return WebMouseEvent::MiddleButton;
    case Manx::MouseEvent::RightButton: return WebMouseEvent::RightButton;
    }
    ASSERT_NOT_REACHED();
    return WebMouseEvent::LeftButton;
}

WebEvent::Modifiers modifiersForEvent(const Manx::KeyboardEvent& event)
{
    int modifiers = 0;
    
    if (event.m_shiftKey)
        modifiers |= WebEvent::ShiftKey;
    if (event.m_controlKey)
        modifiers |=  WebEvent::ControlKey;
    if (event.m_altKey)
        modifiers |=  WebEvent::AltKey;
    if (event.m_metaKey)
        modifiers |=  WebEvent::MetaKey;
    if (event.m_capsLockKey)
        modifiers |=  WebEvent::CapsLockKey;

    return static_cast<WebEvent::Modifiers>(modifiers);
}

WebEvent::Modifiers modifiersForEvent(const Manx::MouseEvent& mouseEvent)
{
    int modifiers = 0;
    
    if (mouseEvent.m_shiftKey)
        modifiers |= WebEvent::ShiftKey;
    if (mouseEvent.m_controlKey)
        modifiers |=  WebEvent::ControlKey;
    if (mouseEvent.m_altKey)
        modifiers |=  WebEvent::AltKey;
    if (mouseEvent.m_metaKey)
        modifiers |=  WebEvent::MetaKey;

    return static_cast<WebEvent::Modifiers>(modifiers);
}

WebEvent::Modifiers modifiersForEvent(const Manx::WheelEvent& wheelEvent)
{
    int modifiers = 0;
    
    if (wheelEvent.m_shiftKey)
        modifiers |= WebEvent::ShiftKey;
    if (wheelEvent.m_controlKey)
        modifiers |=  WebEvent::ControlKey;
    if (wheelEvent.m_altKey)
        modifiers |=  WebEvent::AltKey;
    if (wheelEvent.m_metaKey)
        modifiers |=  WebEvent::MetaKey;

    return static_cast<WebEvent::Modifiers>(modifiers);
}

WebKeyboardEvent WebEventFactory::createWebKeyboardEvent(const Manx::KeyboardEvent& event)
{
    WebEvent::Type type = webEventTypeForEvent(event);
    const String text = WTF::String::fromUTF8(event.m_text);
    const String unmodifiedText = WTF::String::fromUTF8(event.m_unmodifiedText);
    bool isAutoRepeat = event.m_isAutoRepeat;
    bool isSystemKey = event.m_isSystemKey;
    bool isKeypad = event.m_isKeypad;
    const String keyIdentifier(event.m_keyIdentifier);
    int windowsVirtualKeyCode = event.m_windowsVirtualKeyCode;
    int nativeVirtualKeyCode = event.m_nativeVirtualKeyCode;
    int macCharCode = event.m_macCharCode;
    WebEvent::Modifiers modifiers = modifiersForEvent(event);
    double timestamp = currentTime();

    return WebKeyboardEvent(type, text, unmodifiedText, keyIdentifier, windowsVirtualKeyCode, nativeVirtualKeyCode, macCharCode, isAutoRepeat, isKeypad, isSystemKey, modifiers, timestamp);
}

WebMouseEvent WebEventFactory::createWebMouseEvent(const Manx::MouseEvent& mouseEvent)
{
    static WebCore::IntPoint lastPosition = WebCore::IntPoint();

    WebEvent::Type type = webEventTypeForEvent(mouseEvent);
    WebMouseEvent::Button button = buttonForEvent(mouseEvent);
    const IntPoint position(mouseEvent.m_x, mouseEvent.m_y);
    const IntPoint globalPosition = position;
    int clickCount = mouseEvent.m_clickCount;
    WebEvent::Modifiers modifiers = modifiersForEvent(mouseEvent);
    double timestamp = currentTime();
    float deltaX = position.x() - lastPosition.x();
    float deltaY = position.y() - lastPosition.y();
    lastPosition = position;

    return WebMouseEvent(type, button, position, globalPosition, deltaX, deltaY, 0, clickCount, modifiers, timestamp);
}

WebWheelEvent WebEventFactory::createWebWheelEvent(const Manx::WheelEvent& wheelEvent)
{
    const IntPoint position(wheelEvent.m_x, wheelEvent.m_y);
    const IntPoint globalPosition = position;
    FloatSize delta(wheelEvent.m_deltaX, wheelEvent.m_deltaY);
    FloatSize wheelTicks(wheelEvent.m_wheelTicksX, wheelEvent.m_wheelTicksY);
    WebEvent::Modifiers modifiers = modifiersForEvent(wheelEvent);
    double timestamp = currentTime();

    return WebWheelEvent(WebEvent::Wheel, position, globalPosition, delta, wheelTicks, WebWheelEvent::ScrollByPixelWheelEvent, modifiers, timestamp);
}

} // namespace WebKit
