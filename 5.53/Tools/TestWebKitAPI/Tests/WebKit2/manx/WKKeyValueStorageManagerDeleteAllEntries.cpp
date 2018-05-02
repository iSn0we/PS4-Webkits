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
#include <WebKit/WKContextPrivate.h>
#include <WebKit/WKKeyValueStorageManager.h>


namespace TestWebKitAPI {

static bool didFinishLoad = false;
static bool runJavascriptDone = false;
static int testSeq = 0;


static void didFinishLoadForFrame(WKPageRef, WKFrameRef, WKTypeRef, const void* clientInfo)
{
    didFinishLoad = true;
}

static bool runJavaScriptConfirm(WKPageRef page, WKStringRef message, WKFrameRef frame, WKSecurityOriginRef securityOrigin, const void *clientInfo)
{
    runJavascriptDone = true;

    if (!testSeq)
        EXPECT_WK_STREQ("Registered", message);
    else if (testSeq == 1)
        EXPECT_WK_STREQ("Unregistered", message);
    else
        EXPECT_TRUE(false);

    testSeq++;

    return true;
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

static void setPageUIClient(WKPageRef page, const void* clientInfo)
{
    WKPageUIClientV5 uiClient;
    memset(&uiClient, 0, sizeof(uiClient));

    uiClient.base.version = 5;
    uiClient.base.clientInfo = clientInfo;
    uiClient.runJavaScriptConfirm = runJavaScriptConfirm;

    WKPageSetPageUIClient(page, &uiClient.base);
}

TEST(WebKit2, WKKeyValueStorageManagerDeleteAllEntries)
{
    WKRetainPtr<WKContextRef> context = adoptWK(WKContextCreate());

    PlatformWebView webView(context.get());

    setPageLoaderClient(webView.page(), this);
    setPageUIClient(webView.page(), this);

    WKPageLoadURL(webView.page(), adoptWK(Util::createURLForResource("manx/WKKeyValueStorageManagerDeleteAllEntries", "html")).get());
    Util::run(&didFinishLoad);

    // Register a value to local storage, and then checks it.
    // Write to the local storage file to a disk, and wait for it to complete.
    runJavascriptDone = false;
    webView.simulateButtonClick(kWKEventMouseButtonLeftButton, 10, 10, 0);
    Util::run(&runJavascriptDone);
    Util::sleep(3);

    // Remove the local storage file, and wait for it to complete.
    // And make sure that the file is empty.
    WKKeyValueStorageManagerRef kvsmanager = WKContextGetKeyValueStorageManager(context.get());
    if (!kvsmanager)
        EXPECT_TRUE(false);

    WKKeyValueStorageManagerDeleteAllEntries(kvsmanager);
    Util::sleep(3);

    runJavascriptDone = false;
    webView.simulateButtonClick(kWKEventMouseButtonLeftButton, 10, 10, 0);
    Util::run(&runJavascriptDone);
}

} // namespace TestWebKitAPI

#endif
