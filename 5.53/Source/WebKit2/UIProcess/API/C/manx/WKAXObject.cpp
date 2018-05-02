/*
 * (C) 2016 Sony Interactive Entertainment Inc.
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
#include "WKAXObject.h"

#include "WKAPICast.h"
#include "WebAccessibilityObject.h"
#include "WebFrameProxy.h"
#include "WebPageProxy.h"

using namespace WebKit;

WKTypeID WKAXObjectGetTypeID()
{
    return toAPI(WebAccessibilityObject::APIType);
}

WKAXRole WKAXObjectRole(WKAXObjectRef axObjectref)
{
    return toImpl(axObjectref)->role();
}

WKStringRef WKAXObjectTitle(WKAXObjectRef axObjectref)
{
    RefPtr<API::String> title = API::String::create(toImpl(axObjectref)->title());
    return toAPI(title.release().leakRef());
}

WKStringRef WKAXObjectDescription(WKAXObjectRef axObjectref)
{
    RefPtr<API::String> description = API::String::create(toImpl(axObjectref)->description());
    return toAPI(description.release().leakRef());
}

WKStringRef WKAXObjectHelpText(WKAXObjectRef axObjectref)
{
    RefPtr<API::String> helpText = API::String::create(toImpl(axObjectref)->helpText());
    return toAPI(helpText.release().leakRef());
}

WKStringRef WKAXObjectURL(WKAXObjectRef axObjectref)
{
    RefPtr<API::String> urlText = API::String::create(toImpl(axObjectref)->url());
    return toAPI(urlText.release().leakRef());
}

WKAXButtonState WKAXObjectButtonState(WKAXObjectRef axObjectref)
{
    return toImpl(axObjectref)->buttonState();
}

WKPageRef WKAXObjectPage(WKAXObjectRef axObjectref)
{
    return toAPI(toImpl(axObjectref)->page());
}

WKFrameRef WKAXObjectFrame(WKAXObjectRef axObjectref)
{
    return toAPI(toImpl(axObjectref)->frame());
}
