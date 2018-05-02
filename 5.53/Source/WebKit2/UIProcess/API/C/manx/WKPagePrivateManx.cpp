/*
 * Copyright (C) 2011 Apple Inc. All rights reserved.
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
#include "WKPagePrivateManx.h"

#include "IntPoint.h"
#include "WKAPICast.h"
#include "WebPageProxy.h"

using namespace WebKit;

int WKPageGetProcessIdentifier(WKPageRef pageRef)
{
    return toImpl(pageRef)->processIdentifier();
}

void WKPageBlurFocusedNode(WKPageRef pageRef)
{
    return toImpl(pageRef)->exitComposition();
}

void WKPageSetCaretVisible(WKPageRef pageRef, bool visible)
{
    toImpl(pageRef)->setCaretVisible(visible);
}

void WKPageSetScaleFactorKeepingScrollOffset(WKPageRef pageRef, double scale)
{
    toImpl(pageRef)->scalePageInViewCoordinates(scale, WebCore::IntPoint());
}

uint32_t WKPageGetAcceleratedCompositingBackgroundColor(WKPageRef pageRef)
{
    return toImpl(pageRef)->acceleratedCompositingBackgroundColor();
}

void WKPageSuspendActiveDOMObjectsAndAnimations(WKPageRef pageRef)
{
    toImpl(pageRef)->suspendActiveDOMObjectsAndAnimations();
}

void WKPageResumeActiveDOMObjectsAndAnimations(WKPageRef pageRef)
{
    toImpl(pageRef)->resumeActiveDOMObjectsAndAnimations();
}

static std::function<void(const String&, WebKit::CallbackBase::Error)> toGenericCallbackFunction(void* context, void(*callback)(WKStringRef, WKErrorRef, void*))
{
    return [context, callback](const String& returnValue, WebKit::CallbackBase::Error error)
    {
        callback(toAPI(API::String::create(returnValue).ptr()), error != WebKit::CallbackBase::Error::None ? toAPI(API::Error::create().ptr()) : 0, context);
    };
}

void WKPageRunJavaScriptInMainFrameAndReturnString(WKPageRef pageRef, WKStringRef scriptRef, void* context, WKPageRunJavaScriptAndReturnStringFunction callback)
{
    toImpl(pageRef)->runJavaScriptInMainFrameWithStringCallback(toImpl(scriptRef)->string(), toGenericCallbackFunction(context, callback));
}
