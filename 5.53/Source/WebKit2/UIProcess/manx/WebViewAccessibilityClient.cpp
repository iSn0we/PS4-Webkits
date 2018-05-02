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
#include "WebViewAccessibilityClient.h"

#include "ManxWebView.h"

using namespace WebCore;

namespace WebKit {

void WebViewAccessibilityClient::accessibilityNotification(WebView* view, WebAccessibilityObject* axObject, WebCore::AXObjectCache::AXNotification notification)
{
    if (!m_client.notification)
        return;

    m_client.notification(toAPI(view), toAPI(axObject), notification, m_client.base.clientInfo);
}

void WebViewAccessibilityClient::accessibilityTextChanged(WebView* view, WebAccessibilityObject* axObject, WebCore::AXTextChange textChange, uint32_t offset, const String& text)
{
    if (!m_client.textChanged)
        return;

    m_client.textChanged(toAPI(view), toAPI(axObject), textChange, offset, toAPI(text.impl()), m_client.base.clientInfo);
}

void WebViewAccessibilityClient::accessibilityLoadingEvent(WebView* view, WebAccessibilityObject* axObject, WebCore::AXObjectCache::AXLoadingEvent loadingEvent)
{
    if (!m_client.loadingEvent)
        return;

    m_client.loadingEvent(toAPI(view), toAPI(axObject), loadingEvent, m_client.base.clientInfo);
}

void WebViewAccessibilityClient::handleAccessibilityRootObject(WebView* view, WebAccessibilityObject* axObject)
{
    if (!m_client.rootObject)
        return;

    m_client.rootObject(toAPI(view), toAPI(axObject), m_client.base.clientInfo);
}

void WebViewAccessibilityClient::handleAccessibilityFocusedObject(WebView* view, WebAccessibilityObject* axObject)
{
    if (!m_client.focusedObject)
        return;

    m_client.focusedObject(toAPI(view), toAPI(axObject), m_client.base.clientInfo);
}

void WebViewAccessibilityClient::handleAccessibilityHitTest(WebView* view, WebAccessibilityObject* axObject)
{
    if (!m_client.hitTest)
        return;

    m_client.hitTest(toAPI(view), toAPI(axObject), m_client.base.clientInfo);
}

} // namespace WebKit