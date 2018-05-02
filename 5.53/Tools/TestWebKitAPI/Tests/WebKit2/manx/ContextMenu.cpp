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
static bool showContextMenuDone = false;
static bool hideContextMenuDone = false;

static void didFinishLoadForFrame(WKPageRef, WKFrameRef, WKTypeRef, const void* clientInfo)
{
    didFinishLoad = true;
}

static void showContextMenu(WKPageRef, WKPoint, WKArrayRef contextMenuItem, const void*)
{
    EXPECT_NE(0, WKArrayGetSize(contextMenuItem));
    showContextMenuDone = true;
}

static void hideContextMenu(WKPageRef page, const void* clientInfo)
{
    hideContextMenuDone = true;
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

TEST(WebKit2, ContextMenuShowHide)
{
    WKRetainPtr<WKContextRef> context = adoptWK(WKContextCreate());

    PlatformWebView webView(context.get());

    setPageLoaderClient(webView.page(), this);

    WKPageContextMenuClientV3 contextMenuClient;
    memset(&contextMenuClient, 0, sizeof(contextMenuClient));
    contextMenuClient.base.version = 3;
    contextMenuClient.base.clientInfo = this;
    contextMenuClient.showContextMenu = showContextMenu;
    contextMenuClient.hideContextMenu = hideContextMenu;
    WKPageSetPageContextMenuClient(webView.page(), &contextMenuClient.base);

    didFinishLoad = false;
    showContextMenuDone = false;
    hideContextMenuDone = false;

    WKPageLoadURL(webView.page(), adoptWK(Util::createURLForResource("manx/contextmenu", "html")).get());
    Util::run(&didFinishLoad);

    webView.simulateRightClick(10, 10);
    Util::run(&hideContextMenuDone);
    Util::run(&showContextMenuDone);
}

} // namespace TestWebKitAPI

#endif
