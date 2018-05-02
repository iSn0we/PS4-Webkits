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

namespace TestWebKitAPI {
static bool didFinishLoad = false;
static bool runJavascriptDone = false;
static bool runJavascriptFirstTime = true;
static bool lastReplyValue = false;

static void didFinishLoadForFrame(WKPageRef, WKFrameRef, WKTypeRef, const void* clientInfo)
{
    didFinishLoad = true;
}

static bool runJavaScriptConfirm(WKPageRef page, WKStringRef message, WKFrameRef frame, WKSecurityOriginRef securityOrigin, const void *clientInfo)
{
    if (runJavascriptFirstTime)
        EXPECT_STRCASEEQ("confirm", Util::toSTD(message).c_str());
    else {
        if (lastReplyValue)
            EXPECT_STRCASEEQ("OK", Util::toSTD(message).c_str());
        else
            EXPECT_STRCASEEQ("Cancel", Util::toSTD(message).c_str());
    }
    runJavascriptDone = true;
    runJavascriptFirstTime = false;

    lastReplyValue = !lastReplyValue;
    return lastReplyValue;
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

TEST(WebKit2, JavascriptConfirm)
{
    WKRetainPtr<WKContextRef> context = adoptWK(WKContextCreate());

    PlatformWebView webView(context.get());

    setPageLoaderClient(webView.page(), this);

    WKPageUIClientV5 uiClient;
    memset(&uiClient, 0, sizeof(uiClient));
    uiClient.base.version = 5;
    uiClient.base.clientInfo = this;
    uiClient.runJavaScriptConfirm = runJavaScriptConfirm;
    WKPageSetPageUIClient(webView.page(), &uiClient.base);

    didFinishLoad = false;
    runJavascriptDone = false;

    WKPageLoadURL(webView.page(), adoptWK(Util::createURLForResource("manx/javascriptconfirm", "html")).get());
    Util::run(&didFinishLoad);

    // first time reply true.
    webView.simulateButtonClick(kWKEventMouseButtonLeftButton, 10, 10, 0);
    Util::run(&runJavascriptDone);

    runJavascriptDone = false;

    // second time check reply value and reply false.
    webView.simulateButtonClick(kWKEventMouseButtonLeftButton, 10, 10, 0);
    Util::run(&runJavascriptDone);

    runJavascriptDone = false;

    // third time check reply value.
    webView.simulateButtonClick(kWKEventMouseButtonLeftButton, 10, 10, 0);
    Util::run(&runJavascriptDone);

    runJavascriptDone = false;
}

} // namespace TestWebKitAPI

#endif
