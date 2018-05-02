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
#include <WebKit/WKPopupMenuItem.h>

namespace TestWebKitAPI {

static bool showPopupMenuDone = false;
static bool hidePopupMenuDone = false;
static bool valueChangedDone = false;
static bool didFinishLoad = false;

static void didFinishLoadForFrame(WKPageRef, WKFrameRef, WKTypeRef, const void* clientInfo)
{
    didFinishLoad = true;
}

static void showPopupMenu(WKViewRef view, WKArrayRef popupMenuItem, WKRect rect, int32_t selectedIndex, const void* clientInfo)
{
    EXPECT_TRUE(popupMenuItem != 0);

    EXPECT_EQ(1, selectedIndex);

    size_t itemCount = WKArrayGetSize(popupMenuItem);
    EXPECT_EQ(3, itemCount);

    WKTypeRef item = WKArrayGetItemAtIndex(popupMenuItem, 0);
    EXPECT_TRUE(WKGetTypeID(item) == WKPopupMenuItemGetTypeID());

    EXPECT_FALSE(WKPopupMenuItemIsSeparator(static_cast<WKPopupMenuItemRef>(item)));

    item = WKArrayGetItemAtIndex(popupMenuItem, 1);

    EXPECT_TRUE(WKPopupMenuItemIsItem(static_cast<WKPopupMenuItemRef>(item)));

    WKRetainPtr<WKStringRef> text = adoptWK(WKPopupMenuItemCopyText(static_cast<WKPopupMenuItemRef>(item)));
    EXPECT_TRUE(WKStringIsEqualToUTF8CString(text.get(), "item2"));

    item = WKArrayGetItemAtIndex(popupMenuItem, 2);

    EXPECT_FALSE(WKPopupMenuItemIsEnabled(static_cast<WKPopupMenuItemRef>(item)));

    showPopupMenuDone = true;
}

static void hidePopupMenu(WKViewRef view, const void* clientInfo)
{
    hidePopupMenuDone = true;
}

static void showPopupMenuCallValueChanged(WKViewRef view, WKArrayRef popupMenuItem, WKRect rect, int32_t selectedIndex, const void* clientInfo)
{
    if (valueChangedDone)
        EXPECT_EQ(2, selectedIndex);

    showPopupMenuDone = true;
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

TEST(WebKit2, ShowHidePopupMenu)
{
    WKRetainPtr<WKContextRef> context = adoptWK(WKContextCreate());

    PlatformWebView webView(context.get());
    setPageLoaderClient(webView.page(), this);

    WKViewPopupMenuClientV0 viewPopupMenuClient;
    viewPopupMenuClient.base.version = 0;
    viewPopupMenuClient.base.clientInfo = this;
    viewPopupMenuClient.showPopupMenu = showPopupMenu;
    viewPopupMenuClient.hidePopupMenu = hidePopupMenu;

    WKViewSetViewPopupMenuClient(webView.platformView(), &viewPopupMenuClient.base);

    didFinishLoad = false;
    showPopupMenuDone = false;
    hidePopupMenuDone = false;

    WKPageLoadURL(webView.page(), adoptWK(Util::createURLForResource("manx/popupmenu", "html")).get());
    Util::run(&didFinishLoad);

    webView.simulateButtonClick(kWKEventMouseButtonLeftButton, 20, 20, 0);
    Util::run(&showPopupMenuDone);

    WKPageClose(webView.page());
    Util::run(&hidePopupMenuDone);
}

TEST(WebKit2, ValueChangedPopupMenu)
{
    WKRetainPtr<WKContextRef> context = adoptWK(WKContextCreate());

    PlatformWebView webView(context.get());
    setPageLoaderClient(webView.page(), this);

    WKViewPopupMenuClientV0 viewPopupMenuClient;
    viewPopupMenuClient.base.version = 0;
    viewPopupMenuClient.base.clientInfo = this;
    viewPopupMenuClient.showPopupMenu = showPopupMenuCallValueChanged;
    viewPopupMenuClient.hidePopupMenu = hidePopupMenu;

    WKViewSetViewPopupMenuClient(webView.platformView(), &viewPopupMenuClient.base);

    didFinishLoad = false;
    showPopupMenuDone = false;
    hidePopupMenuDone = false;
    valueChangedDone = false;

    WKPageLoadURL(webView.page(), adoptWK(Util::createURLForResource("manx/popupmenu", "html")).get());
    Util::run(&didFinishLoad);

    webView.simulateButtonClick(kWKEventMouseButtonLeftButton, 20, 20, 0);
    Util::run(&showPopupMenuDone);
    showPopupMenuDone = false;

    // change the selected index from 1 to 2.
    WKViewValueChangedForPopupMenu(webView.platformView(), 2);
    valueChangedDone = true;

    // check the selected index.
    webView.simulateButtonClick(kWKEventMouseButtonLeftButton, 20, 20, 0);
    Util::run(&showPopupMenuDone);
}
} // namespace TestWebKitAPI

#endif
