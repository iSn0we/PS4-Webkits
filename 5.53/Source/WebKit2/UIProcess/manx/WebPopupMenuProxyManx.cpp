/*
* Copyright (C) 2010 Apple Inc. All rights reserved.
* Copyright (C) 2016 Sony Computer Entertainment Inc.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the folloManxg conditions
* are met:
* 1. Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the folloManxg disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the folloManxg disclaimer in the
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
#include "WebPopupMenuProxyManx.h"

#include "ManxWebView.h"

using namespace WebCore;

namespace WebKit {

WebPopupMenuProxyManx::WebPopupMenuProxyManx(WebView* webView, WebPopupMenuProxy::Client* client)
    : WebPopupMenuProxy(client)
    , m_webView(webView)
    , m_isPopup(false)
{
}

WebPopupMenuProxyManx::~WebPopupMenuProxyManx()
{
    hidePopupMenu();
}

void WebPopupMenuProxyManx::showPopupMenu(const IntRect& rect, TextDirection, double, const Vector<WebPopupItem>& items, const PlatformPopupMenuData& data, int32_t selectedIndex)
{
    if (m_isPopup)
        return;
    m_webView->showPopupMenu(this, items, rect, selectedIndex);
    m_isPopup = true;
}

void WebPopupMenuProxyManx::hidePopupMenu()
{
    if (!m_isPopup)
        return;
    m_webView->hidePopupMenu();
    m_isPopup = false;
}

void WebPopupMenuProxyManx::valueChangedForPopupMenu(int selectedIndex)
{
    m_client->valueChangedForPopupMenu(this, selectedIndex);
}

}
