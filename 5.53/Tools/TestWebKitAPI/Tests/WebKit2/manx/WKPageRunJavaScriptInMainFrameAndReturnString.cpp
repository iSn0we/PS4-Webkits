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
#include <WebKit/WKPagePrivateManx.h>

namespace TestWebKitAPI {

static bool loadDone = false;
static bool done = false;

static void didFinishLoadForFrame(WKPageRef page, WKFrameRef frame, WKTypeRef userData, const void*)
{
    loadDone = true;
}

static void javaScriptCallback(WKStringRef resultString, WKErrorRef error, void* ctx)
{
    EXPECT_WK_STREQ("WKPageRunJavaScriptInMainFrameAndReturnString testing page", resultString);
    done = true;
}

TEST(WebKit2, WKPageRunJavaScriptInMainFrameAndReturnStringTest)
{
    WKRetainPtr<WKContextRef> context = adoptWK(WKContextCreate());
    PlatformWebView webView(context.get());

    WKPageLoaderClientV0 loaderClient;
    memset(&loaderClient, 0, sizeof(loaderClient));

    loaderClient.base.version = 0;
    loaderClient.didFinishLoadForFrame = didFinishLoadForFrame;

    WKPageSetPageLoaderClient(webView.page(), &loaderClient.base);

    WKPageLoadURL(webView.page(), adoptWK(Util::createURLForResource("manx/WKPageRunJavaScriptInMainFrameAndReturnString", "html")).get());
    Util::run(&loadDone);

    WKRetainPtr<WKStringRef> scriptRef = adoptWK(WKStringCreateWithUTF8CString("window.document.title"));
    WKPageRunJavaScriptInMainFrameAndReturnString(webView.page(), scriptRef.get(), 0 /* void* context */, javaScriptCallback);
    Util::run(&done);
}

} // namespace TestWebKitAPI

#endif
