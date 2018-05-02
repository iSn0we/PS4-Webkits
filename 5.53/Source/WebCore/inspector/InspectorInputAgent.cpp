/*
 * Copyright (C) 2012 Google Inc. All rights reserved.
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
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
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

#if PLATFORM(MANX)
#include "InspectorInputAgent.h"

#include "EventHandler.h"
#include "Frame.h"
#include "FrameView.h"
#include "HostWindow.h"
#include "InspectorPageAgent.h"
#include "IntPoint.h"
#include "IntRect.h"
#include "IntSize.h"
#include "Page.h"
#include "PlatformEvent.h"
#include "PlatformKeyboardEvent.h"
#include "PlatformMouseEvent.h"

#include <inspector/InspectorValues.h>
#include <wtf/CurrentTime.h>
#include <wtf/text/StringBuilder.h>

namespace WebCore {

InspectorInputAgent::InspectorInputAgent(InstrumentingAgents* instrumentingAgents, InspectorPageAgent* pageAgent)
    : InspectorAgentBase(ASCIILiteral("Input"), instrumentingAgents)
    , m_pageAgent(pageAgent)
{
}

void InspectorInputAgent::didCreateFrontendAndBackend(Inspector::FrontendChannel* frontendChannel, Inspector::BackendDispatcher* backendDispatcher)
{
    m_backendDispatcher = Inspector::InputBackendDispatcher::create(backendDispatcher, this);
}

void InspectorInputAgent::willDestroyFrontendAndBackend(Inspector::DisconnectReason)
{
    m_backendDispatcher = nullptr;
}

void InspectorInputAgent::dispatchKeyEvent(ErrorString& error, const String& inType, const int* optinModifiers, const double* timestamp, const String* optinText, const String* optinunmodifiedText, const String* optinkeyIdentifier, const int* optinwindowsVirtualKeyCode, const int* optinnativeVirtualKeyCode, const int* optinmacCharCode, const bool* optinautoRepeat, const bool* optinisKeypad, const bool* optinisSystemKey)
{
    PlatformEvent::Type convertedType;
    if (inType == "keyDown")
        convertedType = PlatformEvent::KeyDown;
    else if (inType == "keyUp")
        convertedType = PlatformEvent::KeyUp;
    else if (inType == "char")
        convertedType = PlatformEvent::Char;
    else if (inType == "rawKeyDown")
        convertedType = PlatformEvent::RawKeyDown;
    else {
        error = "Unrecognized type: " + inType;
        return;
    }

    PlatformKeyboardEvent event(
        convertedType,
        optinText ? *optinText : "",
        optinunmodifiedText ? *optinunmodifiedText : "",
        optinkeyIdentifier ? *optinkeyIdentifier : "",
        optinwindowsVirtualKeyCode ? *optinwindowsVirtualKeyCode : 0,
        optinnativeVirtualKeyCode ? *optinnativeVirtualKeyCode : 0,
        optinmacCharCode ? *optinmacCharCode : 0,
        optinautoRepeat ? *optinautoRepeat : false,
        optinisKeypad ? *optinisKeypad : false,
        optinisSystemKey ? *optinisSystemKey : false,
        static_cast<PlatformEvent::Modifiers>(optinModifiers ? *optinModifiers : 0),
        timestamp ? *timestamp : currentTime());
    m_pageAgent->mainFrame()->eventHandler().keyEvent(event);
}

void InspectorInputAgent::dispatchMouseEvent(ErrorString& error, const String& inType, int inX, int inY, const int* optinModifiers, const double* timestamp, const String* optinButton, const int* optinclickCount)
{
    PlatformEvent::Type convertedType;
    if (inType == "mousePressed")
        convertedType = PlatformEvent::MousePressed;
    else if (inType == "mouseReleased")
        convertedType = PlatformEvent::MouseReleased;
    else if (inType == "mouseMoved")
        convertedType = PlatformEvent::MouseMoved;
    else {
        error = "Unrecognized type: " + inType;
        return;
    }

    int convertedModifiers = optinModifiers ? *optinModifiers : 0;

    MouseButton convertedButton = NoButton;
    if (optinButton) {
        if (*optinButton == "left")
            convertedButton = LeftButton;
        else if (*optinButton == "middle")
            convertedButton = MiddleButton;
        else if (*optinButton == "right")
            convertedButton = RightButton;
        else if (*optinButton != "none") {
            error = "Unrecognized button: " + *optinButton;
            return;
        }
    }

    // Some platforms may have flipped coordinate systems, but the given coordinates
    // assume the origin is in the top-left of the window. Convert.
    IntPoint convertedPoint = m_pageAgent->mainFrame()->view()->convertToContainingWindow(IntPoint(inX, inY));
    IntPoint globalPoint = m_pageAgent->mainFrame()->view()->hostWindow()->rootViewToScreen(IntRect(IntPoint(inX, inY), IntSize(0, 0))).location();

    PlatformMouseEvent event(
        convertedPoint,
        globalPoint,
        convertedButton,
        convertedType,
        optinclickCount ? *optinclickCount : 0,
        (convertedModifiers & PlatformEvent::ShiftKey),
        (convertedModifiers & PlatformEvent::CtrlKey),
        (convertedModifiers & PlatformEvent::AltKey),
        (convertedModifiers & PlatformEvent::MetaKey),
        timestamp ? *timestamp : currentTime(),
        ForceAtClick);

    EventHandler* handler = &(m_pageAgent->mainFrame()->eventHandler());
    switch (convertedType) {
    case PlatformEvent::MousePressed:
        handler->handleMousePressEvent(event);
        break;
    case PlatformEvent::MouseReleased:
        handler->handleMouseReleaseEvent(event);
        break;
    case PlatformEvent::MouseMoved:
        handler->handleMouseMoveEvent(event);
        break;
    default:
        error = "Unhandled type: " + inType;
    }
}

} // namespace WebCore

#endif // PLATFORM(MANX)
