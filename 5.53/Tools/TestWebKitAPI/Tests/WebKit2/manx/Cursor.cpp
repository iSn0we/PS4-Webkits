/*
 * Copyright (C) 2016 Sony Computer Entertainment Inc.
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

#if WK_HAVE_C_SPI

#include "PlatformUtilities.h"
#include "PlatformWebView.h"
#include <WebKit/WKContextPrivate.h>

namespace TestWebKitAPI {

static bool didFinishLoad = false;
static bool setCursorDone = false;
static WKCursorType expectedCursorType = kWKCursorTypePointer;

static void didFinishLoadForFrame(WKPageRef, WKFrameRef, WKTypeRef, const void* clientInfo)
{
    didFinishLoad = true;
}

static void setCursor(WKViewRef view, WKCursorType cursorType, const void* clientInfo)
{
    EXPECT_EQ(expectedCursorType, cursorType);
    setCursorDone = true;
}

static void setPageLoaderClient(WKPageRef page, const void* clientInfo)
{
    WKPageLoaderClientV6 loaderClient;
    memset(&loaderClient, 0, sizeof(loaderClient));

    loaderClient.base.version = 6;
    loaderClient.base.clientInfo = clientInfo;
    loaderClient.didFinishLoadForFrame = didFinishLoadForFrame;

    WKPageSetPageLoaderClient(page, &loaderClient.base);
}

TEST(WebKit2, SetCursor)
{
    WKRetainPtr<WKContextRef> context = adoptWK(WKContextCreate());

    WKViewClientV0 viewClient;
    memset(&viewClient, 0, sizeof(viewClient));
    viewClient.base.version = 0;
    viewClient.base.clientInfo = this;
    viewClient.setCursor = setCursor;

    PlatformWebView webView(context.get(), 0, &viewClient.base);

    setPageLoaderClient(webView.page(), this);

    didFinishLoad = false;
    setCursorDone = false;

    WKPageLoadURL(webView.page(), adoptWK(Util::createURLForResource("manx/cursor", "html")).get());
    Util::run(&didFinishLoad);

    expectedCursorType = kWKCursorTypeHand;
    webView.simulateMouseMove(20, 20);
    Util::run(&setCursorDone);
    setCursorDone = false;

    expectedCursorType = kWKCursorTypePointer;
    webView.simulateMouseMove(20, 120);
    Util::run(&setCursorDone);
    setCursorDone = false;

    expectedCursorType = kWKCursorTypeNone;
    webView.simulateMouseMove(20, 220);
    Util::run(&setCursorDone);
    setCursorDone = false;
}

} // namespace TestWebKitAPI

#endif
