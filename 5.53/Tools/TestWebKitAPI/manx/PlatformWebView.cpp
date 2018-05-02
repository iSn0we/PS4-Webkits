/*
 * Copyright (C) 2012, 2014 Samsung Electronics
 * Copyright (C) 2012 Intel Corporation. All rights reserved.
 * Copyright (C) 2015 Sony Computer Entertainment Inc
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
#include "PlatformWebView.h"

#include <WebKit/WKContextManx.h>
#include <WebKit/WebKit2_C.h>
#include <manx/KeyboardCodes.h>

namespace TestWebKitAPI {

static void setWebProcessPath(WKContextRef contextRef)
{
    WKStringRef path = WKStringCreateWithUTF8CString("/hostapp/common/lib/WebProcess.self");
    WKContextSetWebProcessPath(contextRef, path);
    WKRelease(path);
}

PlatformWebView::PlatformWebView(WKContextRef contextRef, WKPageGroupRef pageGroupRef, WKViewClientBase* viewClientBase)
{
    setWebProcessPath(contextRef);
    m_window = nullptr;
    WKViewClientV0 viewClient = { { 0, this } };
    m_view = WKViewCreate(contextRef, pageGroupRef, viewClientBase ? viewClientBase : &viewClient.base);
    resizeTo(800, 600);
}

PlatformWebView::~PlatformWebView()
{
    WKRelease(m_view);
}

void PlatformWebView::resizeTo(unsigned width, unsigned height)
{
    WKViewSetSize(m_view, width, height);
}

WKPageRef PlatformWebView::page() const
{
    return WKViewGetPage(m_view);
}

void PlatformWebView::simulateSpacebarKeyPress()
{
    Manx::KeyboardEvent event(Manx::KeyboardEvent::KeyDown, Manx::VK_SPACE, false, false, false, false, false);
    WKViewHandleKeyboardEvent(m_view, &event);
    event.m_type = Manx::KeyboardEvent::KeyUp;
    WKViewHandleKeyboardEvent(m_view, &event);
}

void PlatformWebView::simulateMouseMove(unsigned x, unsigned y)
{
    Manx::MouseEvent event(Manx::MouseEvent::MouseMove, Manx::MouseEvent::NoButton, x, y, 1, false, false, false, false);
    WKViewHandleMouseEvent(m_view, &event);
}

void PlatformWebView::simulateRightClick(unsigned x, unsigned y)
{
    simulateButtonClick(kWKEventMouseButtonRightButton, x, y, 0);
}

void PlatformWebView::simulateButtonClick(WKEventMouseButton button, unsigned x, unsigned y, WKEventModifiers)
{
    Manx::MouseEvent event(Manx::MouseEvent::MouseMove, Manx::MouseEvent::NoButton, x, y, 1, false, false, false, false);
    WKViewHandleMouseEvent(m_view, &event);

    Manx::MouseEvent::MouseButtonType buttonType = Manx::MouseEvent::NoButton;

    switch (button) {
    case kWKEventMouseButtonLeftButton:
        buttonType = Manx::MouseEvent::LeftButton;
        break;
    case kWKEventMouseButtonMiddleButton:
        buttonType = Manx::MouseEvent::MiddleButton;
        break;
    case kWKEventMouseButtonRightButton:
        buttonType = Manx::MouseEvent::RightButton;
        break;
    case kWKEventMouseButtonNoButton:
    default:
        break;
    }
    event.m_type = Manx::MouseEvent::MouseDown;
    event.m_button = buttonType;
    WKViewHandleMouseEvent(m_view, &event);
    event.m_type = Manx::MouseEvent::MouseUp;
    WKViewHandleMouseEvent(m_view, &event);
}
} // namespace TestWebKitAPI
