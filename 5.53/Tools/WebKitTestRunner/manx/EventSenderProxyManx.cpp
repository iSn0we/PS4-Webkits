/*
 * Copyright (C) 2007, 2008 Apple Inc. All rights reserved.
 * Copyright (C) 2009 Zan Dobersek <zandobersek@gmail.com>
 * Copyright (C) 2009 Holger Hans Peter Freyther
 * Copyright (C) 2010 Igalia S.L.
 * Copyright (C) 2011 ProFUSION Embedded Systems
 * Copyright (C) 2012 Samsung Electronics
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

#include "config.h"
#include "EventSenderProxy.h"

#include "NotImplemented.h"
#include "PlatformWebView.h"
#include "TestController.h"
#include <manx/KeyboardCodes.h>
#include <manx/KeyboardEvent.h>
#include <unistd.h>

namespace WTR {

enum WTREventType {
    WTREventTypeNone = 0,
    WTREventTypeMouseDown,
    WTREventTypeMouseUp,
    WTREventTypeMouseMove,
    WTREventTypeMouseScrollBy,
    WTREventTypeLeapForward
};

struct WTREvent {
    WTREventType eventType;
    unsigned delay;
    WKEventModifiers modifiers;
    Manx::MouseEvent::MouseButtonType button;
    int horizontal;
    int vertical;

    WTREvent()
        : eventType(WTREventTypeNone)
        , delay(0)
        , modifiers(0)
        , button(Manx::MouseEvent::NoButton)
        , horizontal(-1)
        , vertical(-1)
    {
    }

    WTREvent(WTREventType eventType, unsigned delay, WKEventModifiers modifiers, Manx::MouseEvent::MouseButtonType button = Manx::MouseEvent::NoButton)
        : eventType(eventType)
        , delay(delay)
        , modifiers(modifiers)
        , button(button)
        , horizontal(-1)
        , vertical(-1)
    {
    }
};

struct KeyEventInfo {
    Manx::VirtualKeyboardCode key;
    const char* text;
};

static Manx::MouseEvent::MouseButtonType manxMouseButton(unsigned button)
{
    // The common case involves converting from a WKEventMouseButton (which
    // starts at -1) to an Manx::MouseEvent::MouseButtonType (which a starts at 0). The special
    // case for button 3 exists because of fast/events/mouse-click-events.html,
    // which tests whether a 4th mouse button behaves as the middle one.
    if (button <= kWKEventMouseButtonRightButton)
        return Manx::MouseEvent::MouseButtonType(button + 1);
    if (button == kWKEventMouseButtonRightButton + 1)
        return Manx::MouseEvent::MiddleButton;
    return Manx::MouseEvent::NoButton;
}

static bool keyName(WKStringRef keyRef, KeyEventInfo& keyInfo)
{
    struct { 
        const char *webkit;
        Manx::VirtualKeyboardCode manx;
        const char *text;
    } table[] = {
        // {"leftAlt", Manx::VK_, ""},
        // {"pageDown", Manx::VK_, ""},
        // {"pageUp", Manx::VK_, ""},
        // {"rightAlt", Manx::VK_, ""},

        {"F1", Manx::VK_F1, ""},
        {"F2", Manx::VK_F2, ""},
        {"F3", Manx::VK_F3, ""},
        {"F4", Manx::VK_F4, ""},
        {"F5", Manx::VK_F5, ""},
        {"F6", Manx::VK_F6, ""},
        {"F7", Manx::VK_F7, ""},
        {"F8", Manx::VK_F8, ""},
        {"F9", Manx::VK_F9, ""},
        {"F10", Manx::VK_F10, ""},
        {"F11", Manx::VK_F11, ""},
        {"F12", Manx::VK_F12, ""},

        {"delete", Manx::VK_DELETE, ""},
        {"home", Manx::VK_HOME, ""},
        {"end", Manx::VK_END, ""},
        {"insert", Manx::VK_INSERT, ""},
        {"menu", Manx::VK_MENU, ""},
        {"printScreen", Manx::VK_PRINT, ""},

        {"leftArrow", Manx::VK_LEFT, ""},
        {"rightArrow", Manx::VK_RIGHT, ""},
        {"upArrow", Manx::VK_UP, ""},
        {"downArrow", Manx::VK_DOWN, ""},

        {"leftControl", Manx::VK_LCONTROL, ""},
        {"rightControl", Manx::VK_RCONTROL, ""},
        {"leftShift", Manx::VK_LSHIFT, ""},
        {"rightShift", Manx::VK_RSHIFT, ""},

        {"\n", Manx::VK_RETURN, "\r"},
        {"\r", Manx::VK_RETURN, "\r"},
        {"\t", Manx::VK_TAB, "\t"},
        {"\x8", Manx::VK_BACK, "\x8"},
        {" ", Manx::VK_SPACE, " "},

        {"0", Manx::VK_0, "0"},
        {"1", Manx::VK_1, "1"},
        {"2", Manx::VK_2, "2"},
        {"3", Manx::VK_3, "3"},
        {"4", Manx::VK_4, "4"},
        {"5", Manx::VK_5, "5"},
        {"6", Manx::VK_6, "6"},
        {"7", Manx::VK_7, "7"},
        {"8", Manx::VK_8, "8"},
        {"9", Manx::VK_9, "9"},

        {"A", Manx::VK_A, "A"},
        {"B", Manx::VK_B, "B"},
        {"C", Manx::VK_C, "C"},
        {"D", Manx::VK_D, "D"},
        {"E", Manx::VK_E, "E"},
        {"F", Manx::VK_F, "F"},
        {"G", Manx::VK_G, "G"},
        {"H", Manx::VK_H, "H"},
        {"I", Manx::VK_I, "I"},
        {"J", Manx::VK_J, "J"},
        {"K", Manx::VK_K, "K"},
        {"L", Manx::VK_L, "L"},
        {"M", Manx::VK_M, "M"},
        {"N", Manx::VK_N, "N"},
        {"O", Manx::VK_O, "O"},
        {"P", Manx::VK_P, "P"},
        {"Q", Manx::VK_Q, "Q"},
        {"R", Manx::VK_R, "R"},
        {"S", Manx::VK_S, "S"},
        {"T", Manx::VK_T, "T"},
        {"U", Manx::VK_U, "U"},
        {"V", Manx::VK_V, "V"},
        {"W", Manx::VK_W, "W"},
        {"X", Manx::VK_X, "X"},
        {"Y", Manx::VK_Y, "Y"},
        {"Z", Manx::VK_Z, "Z"},

        {"a", Manx::VK_A, "a"},
        {"b", Manx::VK_B, "b"},
        {"c", Manx::VK_C, "c"},
        {"d", Manx::VK_D, "d"},
        {"e", Manx::VK_E, "e"},
        {"f", Manx::VK_F, "f"},
        {"g", Manx::VK_G, "g"},
        {"h", Manx::VK_H, "h"},
        {"i", Manx::VK_I, "i"},
        {"j", Manx::VK_J, "j"},
        {"k", Manx::VK_K, "k"},
        {"l", Manx::VK_L, "l"},
        {"m", Manx::VK_M, "m"},
        {"n", Manx::VK_N, "n"},
        {"o", Manx::VK_O, "o"},
        {"p", Manx::VK_P, "p"},
        {"q", Manx::VK_Q, "q"},
        {"r", Manx::VK_R, "r"},
        {"s", Manx::VK_S, "s"},
        {"t", Manx::VK_T, "t"},
        {"u", Manx::VK_U, "u"},
        {"v", Manx::VK_V, "v"},
        {"w", Manx::VK_W, "w"},
        {"x", Manx::VK_X, "x"},
        {"y", Manx::VK_Y, "y"},
        {"z", Manx::VK_Z, "z"},

    };
    for (auto i : table) {
        if (WKStringIsEqualToUTF8CString(keyRef, i.webkit)) {
            keyInfo.key = i.manx;
            keyInfo.text = i.text;
            return true;
        }
    }
    return false;
}

EventSenderProxy::EventSenderProxy(TestController* testController)
    : m_testController(testController)
    , m_time(0)
    , m_leftMouseButtonDown(false)
    , m_clickCount(0)
    , m_clickTime(0)
    , m_clickButton(kWKEventMouseButtonNoButton)
    , m_mouseButton(kWKEventMouseButtonNoButton)
#if ENABLE(TOUCH_EVENTS)
    , m_touchPoints(0)
#endif
{
}

EventSenderProxy::~EventSenderProxy()
{
#if ENABLE(TOUCH_EVENTS)
    clearTouchPoints();
#endif
}

void EventSenderProxy::updateClickCountForButton(int button)
{
    if (m_time - m_clickTime < 1 && m_position == m_clickPosition && button == m_clickButton) {
        ++m_clickCount;
        m_clickTime = m_time;
        return;
    }

    m_clickCount = 1;
    m_clickTime = m_time;
    m_clickPosition = m_position;
    m_clickButton = button;
}

static void modifiers(WKEventModifiers w, bool& shiftKey, bool& ctrlKey, bool& altKey, bool& metaKey)
{
    shiftKey = w & kWKEventModifiersShiftKey;
    ctrlKey = w & kWKEventModifiersControlKey;
    altKey = w & kWKEventModifiersAltKey;
    metaKey = w & kWKEventModifiersMetaKey;
}

void EventSenderProxy::dispatchEvent(const WTREvent& event)
{
    WKViewRef view = m_testController->mainWebView()->platformView();

    if (event.eventType == WTREventTypeMouseDown) {
        bool shiftKey, ctrlKey, altKey, metaKey;
        modifiers(event.modifiers, shiftKey, ctrlKey, altKey, metaKey);
        Manx::MouseEvent mevent(Manx::MouseEvent::MouseDown, event.button, m_position.x, m_position.y, m_clickCount, shiftKey, ctrlKey, altKey, metaKey);
        WKViewHandleMouseEvent(view, &mevent);
    } else if (event.eventType == WTREventTypeMouseUp) {
        bool shiftKey, ctrlKey, altKey, metaKey;
        modifiers(event.modifiers, shiftKey, ctrlKey, altKey, metaKey);
        Manx::MouseEvent mevent(Manx::MouseEvent::MouseUp, event.button, m_position.x, m_position.y, m_clickCount, shiftKey, ctrlKey, altKey, metaKey);
        WKViewHandleMouseEvent(view, &mevent);
    } else if (event.eventType == WTREventTypeMouseMove) {
        bool shiftKey, ctrlKey, altKey, metaKey;
        modifiers(event.modifiers, shiftKey, ctrlKey, altKey, metaKey);
        Manx::MouseEvent mevent(Manx::MouseEvent::MouseUp, event.button, m_position.x, m_position.y, m_clickCount, shiftKey, ctrlKey, altKey, metaKey);
        WKViewHandleMouseEvent(view, &mevent);
    } else if (event.eventType == WTREventTypeMouseScrollBy) {
        float deltaX = event.horizontal * 10;
        float deltaY = event.vertical * 10;
        Manx::WheelEvent mevent(m_position.x, m_position.y, deltaX, deltaY, event.horizontal, event.vertical, false, false, false, false);
        WKViewHandleWheelEvent(view, &mevent);
    }
}

void EventSenderProxy::replaySavedEvents()
{
    while (!m_eventQueue.isEmpty()) {
        WTREvent event = m_eventQueue.takeFirst();
        if (event.delay)
            usleep(event.delay * 1000);

        dispatchEvent(event);
    }
}

void EventSenderProxy::sendOrQueueEvent(const WTREvent& event)
{
    if (m_eventQueue.isEmpty() || !m_eventQueue.last().delay) {
        dispatchEvent(event);
        return;
    }

    m_eventQueue.append(event);
    replaySavedEvents();
}

void EventSenderProxy::mouseDown(unsigned button, WKEventModifiers wkModifiers)
{
    if (m_mouseButton == button)
        return;

    m_mouseButton = button;
    updateClickCountForButton(button);

    sendOrQueueEvent(WTREvent(WTREventTypeMouseDown, 0, wkModifiers, manxMouseButton(button)));
}

void EventSenderProxy::mouseUp(unsigned button, WKEventModifiers wkModifiers)
{
    sendOrQueueEvent(WTREvent(WTREventTypeMouseUp, 0, wkModifiers, manxMouseButton(button)));

    if (m_mouseButton == button)
        m_mouseButton = kWKEventMouseButtonNoButton;

    m_clickPosition = m_position;
    m_clickTime = currentEventTime();
}

void EventSenderProxy::mouseMoveTo(double x, double y)
{
    m_position.x = x;
    m_position.y = y;

    sendOrQueueEvent(WTREvent(WTREventTypeMouseMove, 0, 0));
}

void EventSenderProxy::mouseScrollBy(int horizontal, int vertical)
{
    WTREvent event(WTREventTypeMouseScrollBy, 0, 0);
    // We need to invert scrolling values since in EFL negative z value means that
    // canvas is scrolling down
    event.horizontal = -horizontal;
    event.vertical = -vertical;
    sendOrQueueEvent(event);
}

void EventSenderProxy::continuousMouseScrollBy(int horizontal, int vertical, bool paged)
{
    notImplemented();
}

void EventSenderProxy::mouseScrollByWithWheelAndMomentumPhases(int x, int y, int /*phase*/, int /*momentum*/)
{
    // EFL does not have the concept of wheel gesture phases or momentum. Just relay to
    // the mouse wheel handler.
    mouseScrollBy(x, y);
}

void EventSenderProxy::leapForward(int milliseconds)
{
    if (m_eventQueue.isEmpty())
        m_eventQueue.append(WTREvent(WTREventTypeLeapForward, milliseconds, 0));

    m_time += milliseconds / 1000.0;
}

void EventSenderProxy::keyDown(WKStringRef keyRef, WKEventModifiers wkModifiers, unsigned location)
{
    KeyEventInfo keyEventInfo;
    if (!keyName(keyRef, keyEventInfo))
        return;

    WKViewRef view = m_testController->mainWebView()->platformView();
    Manx::KeyboardEvent eventDown(Manx::KeyboardEvent::KeyDown, keyEventInfo.text, keyEventInfo.key, false, false, false, false, false);
    Manx::KeyboardEvent eventUp(Manx::KeyboardEvent::KeyUp, keyEventInfo.text, keyEventInfo.key, false, false, false, false, false);

    WKViewHandleKeyboardEvent(view, &eventDown);
    WKViewHandleKeyboardEvent(view, &eventUp);
}

}
