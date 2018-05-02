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

#include "WebContext.h"

#include "WKRetainPtr.h"
#include "utils.h"
#include <WebKit/WKContextManx.h>
#include <WebKit/WKContextPrivate.h>
#include <WebKit/WKPageGroup.h>
#include <WebKit/WKPreferencesManx.h>
#include <WebKit/WKPreferencesRef.h>
#include <WebKit/WKPreferencesRefPrivate.h>
#include <cassert>
#include <sw/sw.h>

#define WEBKIT_WEBPROCESS_FILE "/hostapp/common/lib/WebProcess.self"

WebContext::WebContext(int userID, const char* injectedBundlePath, bool enableWebSecurity)
{
    if (strlen(injectedBundlePath) > 0) {
        WKStringRef bundlePath = WKStringCreateWithUTF8CString(injectedBundlePath);
        m_context = WKContextCreateWithInjectedBundlePath(bundlePath);
    } else
        m_context = WKContextCreate();

    WKContextInjectedBundleClientV0 injectedBundleClient = {
        { 0, this },
        didReceiveMessageFromInjectedBundle,
        didReceiveSynchronousMessageFromInjectedBundle,
    };
    WKContextSetInjectedBundleClient(m_context, &injectedBundleClient.base);

    WKRetainPtr<WKStringRef> webProcessPath = adoptWK(WKStringCreateWithUTF8CString(WEBKIT_WEBPROCESS_FILE));
    WKContextSetWebProcessPath(m_context, webProcessPath.get());

    WKContextSetUserID(m_context, userID);

    static char storagePathStr[] = "/hostapp";
    WKRetainPtr<WKStringRef> storagePath = adoptWK(WKStringCreateWithUTF8CString(storagePathStr));
    WKContextSetCookieStorageDirectory(m_context, storagePath.get());

    WKContextEnableAccessibility(m_context);

    WKRetainPtr<WKStringRef> groupName = adoptWK(WKStringCreateWithUTF8CString("Default"));
    m_pageGroup = WKPageGroupCreateWithIdentifier(groupName.get());

    const bool enableAcceleratedCompositing = SW::WindowManager::renderMode() != SW::RenderModeCPU;

    WKPreferencesRef preferences = WKPreferencesCreate();

    WKPreferencesSetWebGLEnabled(preferences, enableAcceleratedCompositing);
    WKPreferencesSetAcceleratedDrawingEnabled(preferences, enableAcceleratedCompositing);
    WKPreferencesSetCanvasUsesAcceleratedDrawing(preferences, enableAcceleratedCompositing);
    WKPreferencesSetAcceleratedCompositingEnabled(preferences, enableAcceleratedCompositing);
    WKPreferencesSetJavaEnabled(preferences, false);
    WKPreferencesSetOfflineWebApplicationCacheEnabled(preferences, true);
    WKPreferencesSetDeveloperExtrasEnabled(preferences, true);
    WKPreferencesSetWebSecurityEnabled(preferences, enableWebSecurity);
    WKPreferencesSetEncodingDetectorEnabled(preferences, true);
    WKPreferencesSetForceEnableVideo(preferences, true);

    WKPageGroupSetPreferences(m_pageGroup, preferences);
}

WebContext::~WebContext()
{
    WKContextEnableProcessTermination(m_context);
    WKRelease(m_context);
}

WKPageGroupRef& WebContext::pageGroup()
{
    return m_pageGroup;
}

WKContextRef& WebContext::context()
{
    return m_context;
}

void WebContext::didReceiveMessageFromInjectedBundle(WKContextRef wkContext, WKStringRef messageName, WKTypeRef messageBody, const void *clientInfo)
{
    WebContext* context = const_cast<WebContext*>(static_cast<const WebContext*>(clientInfo));
    if (WKStringIsEqualToUTF8CString(messageName, "PostMessageFromNavigatorSceObject"))
        context->didReceivePostMessageFromNavigatorSceObject(messageBody);
}

void WebContext::didReceiveSynchronousMessageFromInjectedBundle(WKContextRef wkContext, WKStringRef messageName, WKTypeRef messageBody, WKTypeRef* returnData, const void *clientInfo)
{
    WebContext* context = const_cast<WebContext*>(static_cast<const WebContext*>(clientInfo));
    if (WKStringIsEqualToUTF8CString(messageName, "SendMessageFromNavigatorSceObject"))
        context->didReceiveSendMessageFromNavigatorSceObject(messageBody, returnData);
}

static void dumpMessageBody(WKTypeRef messageBody)
{
    assert(WKGetTypeID(messageBody) == WKArrayGetTypeID());
    WKArrayRef body = static_cast<WKArrayRef>(messageBody);
    size_t bodySize = WKArrayGetSize(body);
    assert(bodySize > 0);
    assert(WKGetTypeID(WKArrayGetItemAtIndex(body, 0)) == WKPageGetTypeID());
    assert(WKGetTypeID(WKArrayGetItemAtIndex(body, 1)) == WKStringGetTypeID());
    WKPageRef page = static_cast<WKPageRef>(WKArrayGetItemAtIndex(body, 0));
    WKStringRef data = static_cast<WKStringRef>(WKArrayGetItemAtIndex(body, 1));
    printf("page=%p\n", page);
    printf("data=\"%s\"\n", createString(data).c_str());
}

void WebContext::didReceivePostMessageFromNavigatorSceObject(WKTypeRef messageBody)
{
    printf("WebContext::didPostReceiveMessageFromInjectedBundle\n");
    dumpMessageBody(messageBody);
    WKPageRef page = static_cast<WKPageRef>(WKArrayGetItemAtIndex(static_cast<WKArrayRef>(messageBody), 0));
    postMessageToNavigatorSceObject(page);
}

void WebContext::didReceiveSendMessageFromNavigatorSceObject(WKTypeRef messageBody, WKTypeRef* returnData)
{
    printf("WebContext::didSendReceiveMessageFromInjectedBundle\n");
    dumpMessageBody(messageBody);
    *returnData = WKStringCreateWithUTF8CString("return value of sendMessage");
}

void WebContext::postMessageToNavigatorSceObject(WKPageRef page)
{
    WKStringRef messageName = WKStringCreateWithUTF8CString("MessageToNavigatorSceObject");
    WKStringRef contents = WKStringCreateWithUTF8CString("message from UI process");
    WKTypeRef body[] = { page, contents };
    WKArrayRef messageBody = WKArrayCreate(body, 2);
    WKContextPostMessageToInjectedBundle(m_context, messageName, messageBody);
    WKRelease(messageName);
    WKRelease(contents);
    WKRelease(messageBody);
}
