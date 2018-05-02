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

#include "WebView.h"

#include "Browser.h"
#include "WKRetainPtr.h"
#include "utils.h"
#include <WebKit/WKAuthenticationChallenge.h>
#include <WebKit/WKAuthenticationDecisionListener.h>
#include <Webkit/WKCredential.h>
#include <WebKit/WKErrorManx.h>
#include <WebKit/WKPagePrivateManx.h>
#include <WebKit/WKPreferencesManx.h>
#include <WebKit/WKPreferencesRefPrivate.h>
#include <Webkit/WKProtectionSpace.h>
#include <assert.h>
#include <manx/KeyboardCodes.h>

#define WHEEL_EVENT_CAPTURE_TIMEOUT_IN_MS 100

// i-Filter proxy settings
const char* const WebView::s_iFilterProxyServer = "";
const int WebView::s_iFilterProxyPort = 0;
const char* const WebView::s_iFilterProxyUserId = "";
const char* const WebView::s_iFilterProxyPassword = "";

// Proxy / Proxy-Auth settings
const char* const WebView::s_httpProxyServer = "";
const int WebView::s_httpProxyPort = 0;


static WebView* toWebView(const void* clientInfo)
{
    return const_cast<WebView*>(static_cast<const WebView*>(clientInfo));
}

WebView::WebView(Browser* browser, WebContext* webContext)
    : m_browser(browser)
    , m_webContext(webContext)
    , m_isAcceleratedCompositingMode(false)
    , m_enabledIFilterProxy(false)
    , m_enabledDNT(false)
    , m_isActive(true)
    , m_autoCruiseTimer(this, &WebView::onAutoCruiseTimer)
    , m_wheelEventCaptureTimer(this, &WebView::onWheelEventCaptureTimeout)
    , m_enabledDebugAccessibility(false)
{
    m_autoCruiseTimer.init();
    m_wheelEventCaptureTimer.init();

    m_texture.reset(browser->windowManager()->createTexture(SW::Size(WEBVIEW_WIDTH, WEBVIEW_HEIGHT)));

    WKPreferencesRef preferences = WKPageGroupGetPreferences(m_webContext->pageGroup());
    bool acceleratedCompositingEnabled = WKPreferencesGetAcceleratedCompositingEnabled(preferences);

    WKViewClientV0 viewClient = {{ 0, this }};
    viewClient.setViewNeedsDisplay = setViewNeedsDisplay;
    viewClient.enterAcceleratedCompositingMode = enterAcceleratedCompositingMode;
    viewClient.exitAcceleratedCompositingMode = exitAcceleratedCompositingMode;
    viewClient.didChangeEditorState = didChangeEditorState;
    viewClient.doneWithKeyEvent = doneWithKeyEvent;
    viewClient.doneWithMouseUpEvent = doneWithMouseUpEvent;
    viewClient.setCursorPosition = setCursorPosition;
    viewClient.runCertificateVerificationRequest = runCertificateVerificationRequest;

    WKPageUIClientV1 uiClient = {{ 1, this }};
    uiClient.showPage = showPage;
    uiClient.close = closePage;
    uiClient.exceededDatabaseQuota = exceededDatabaseQuota;
    uiClient.didNotHandleWheelEvent = didNotHandleWheelEvent;
    uiClient.runJavaScriptAlert = runJavaScriptAlert;
    uiClient.runJavaScriptConfirm = runJavaScriptConfirm;
    uiClient.runJavaScriptPrompt = runJavaScriptPrompt;
    uiClient.createNewPage = createNewPage;

    WKPageLoaderClientV0 loadClient = {{ 0, this }};
    loadClient.didReceiveTitleForFrame = didReceiveTitleForFrame;
    loadClient.didFailProvisionalLoadWithErrorForFrame = didFailProvisionalLoadWithErrorForFrame;
    loadClient.didCommitLoadForFrame = didCommitLoadForFrame;
    loadClient.didChangeProgress = didChangeProgress;
    loadClient.didSameDocumentNavigationForFrame = didSameDocumentNavigationForFrame;
    loadClient.didReceiveAuthenticationChallengeInFrame = didReceiveAuthenticationChallengeInFrame;

    WKPagePolicyClientV0 pagePolicyClient = {{ 0, this }};
    pagePolicyClient.decidePolicyForResponse_deprecatedForUseWithV0 = decidePolicyForResponse;
    pagePolicyClient.decidePolicyForNavigationAction_deprecatedForUseWithV0 = decidePolicyForNavigationAction;
    pagePolicyClient.decidePolicyForNewWindowAction = decidePolicyForNewWindowAction;

    WKViewPopupMenuClientV0 viewPopupMenuClient = {{ 0, this }};
    viewPopupMenuClient.showPopupMenu = showPopupMenu;
    viewPopupMenuClient.hidePopupMenu = hidePopupMenu;

    WKViewAccessibilityClientV0 viewAccessibilityClient = { { 0, this } };
    viewAccessibilityClient.notification = accessibilityNotification;
    viewAccessibilityClient.textChanged = accessibilityTextChanged;
    viewAccessibilityClient.loadingEvent = accessibilityLoadingEvent;
    viewAccessibilityClient.rootObject = accessibilityRootObject;
    viewAccessibilityClient.focusedObject = accessibilityFocusedObject;
    viewAccessibilityClient.hitTest = accessibilityHitTest;

    m_webView = WKViewCreate(m_webContext->context(), m_webContext->pageGroup(), &viewClient.base);

    WKPageRef page = WKViewGetPage(m_webView);
    WKPageSetPageUIClient(page, &uiClient.base);
    WKPageSetPageLoaderClient(page, &loadClient.base);
    WKPageSetPagePolicyClient(page, &pagePolicyClient.base);
    if (acceleratedCompositingEnabled) {
        WKPageSetUseFixedLayout(page, true);
        WKPageSetFixedLayoutSize(page, WKSizeMake(m_viewWidth, m_viewHeight));
    }

    WKViewSetViewPopupMenuClient(m_webView, &viewPopupMenuClient.base);
    WKViewSetViewAccessibilityClient(m_webView, &viewAccessibilityClient.base);

    WKViewSetSize(m_webView, m_viewWidth, m_viewHeight);

    if (s_httpProxyServer && s_httpProxyServer[0] != '\0') {
        WKRetainPtr<WKStringRef> server(AdoptWK, WKStringCreateWithUTF8CString(s_httpProxyServer));
        int port = s_httpProxyPort;
        WKRetainPtr<WKStringRef> userId(AdoptWK, WKStringCreateWithUTF8CString(""));
        WKRetainPtr<WKStringRef> password(AdoptWK, WKStringCreateWithUTF8CString(""));
        WKContextSetHTTPProxy(m_webContext->context(), true, server.get(), port, userId.get(), password.get());
    }
}

WebView::~WebView()
{
    WKRelease(m_webView);
}

const std::string WebView::title()
{
    return m_title;
}

const std::string WebView::url()
{
    return m_url;
}

uint8_t WebView::progress()
{
    return m_progress;
}

void WebView::load(const std::string url)
{
    WKRetainPtr<WKURLRef> wkUrl = adoptWK(WKURLCreateWithUTF8CString(url.c_str()));
    WKPageRef page = WKViewGetPage(m_webView);
    WKPageLoadURL(page, wkUrl.get());
}

void WebView::stop()
{
    WKPageRef page = WKViewGetPage(m_webView);
    WKPageStopLoading(page);
}

void WebView::reload()
{
    WKPageRef page = WKViewGetPage(m_webView);
    WKPageReload(page);
}

void WebView::goBack()
{
    WKPageRef page = WKViewGetPage(m_webView);
    WKPageGoBack(page);
}

void WebView::goForward()
{
    WKPageRef page = WKViewGetPage(m_webView);
    WKPageGoForward(page);
}

void WebView::tryClose()
{
    WKPageRef page = WKViewGetPage(m_webView);
    WKPageTryClose(page);
}

void WebView::dispatchKeyboardEvent(SW::KeyboardEvent& keyboardEvent)
{
    Manx::KeyboardEvent::Type eventType = (keyboardEvent.m_type == SW::Event::KeyDown)
        ? Manx::KeyboardEvent::KeyDown
        : Manx::KeyboardEvent::KeyUp;

    Manx::KeyboardEvent event(eventType, keyboardEvent.m_vk, false, false, false, false, false);
    WKViewHandleKeyboardEvent(m_webView, &event);
}

void WebView::adjustMousePosition(int inX, int inY, int& outX, int& outY, bool screenPosToWindowPos) const
{
    int k = screenPosToWindowPos ? 1 : -1;
    outX = inX;
    outY = inY - (DISPLAY_HEIGHT - WEBVIEW_HEIGHT) * k;
}

void WebView::dispatchMouseEvent(SW::MouseEvent& swEvent)
{
    int x, y;
    screenPosToWindowPos(swEvent.m_x, swEvent.m_y, x, y);
    Manx::MouseEvent::MouseEventType type;
    Manx::MouseEvent::MouseButtonType button;
    switch (swEvent.m_button) {
    case SW::MouseEvent::LeftButton:
        button = Manx::MouseEvent::LeftButton;
        break;
    case SW::MouseEvent::RightButton:
        button = Manx::MouseEvent::RightButton;
        break;
    }
    switch (swEvent.m_type) {
    case SW::Event::MouseDown:
        type = Manx::MouseEvent::MouseDown;
        break;
    case SW::Event::MouseUp:
        type = Manx::MouseEvent::MouseUp;
        break;
    case SW::Event::MouseMove:
        type = Manx::MouseEvent::MouseMove;
        button = Manx::MouseEvent::NoButton;
        break;
    }
    Manx::MouseEvent event(type, button, x, y, 1, false, false, false, false);
    WKViewHandleMouseEvent(m_webView, &event);
}

void WebView::dispatchWheelEvent(SW::WheelEvent& swEvent)
{
    int x, y;
    screenPosToWindowPos(swEvent.m_x, swEvent.m_y, x, y);
    float deltaX = swEvent.m_wheelTicksX * 10;
    float deltaY = swEvent.m_wheelTicksY * 10;
    if (m_wheelEventCaptureTimer.isActive()) {
        WKViewScrollBy(m_webView, -deltaX, -deltaY);
        m_wheelEventCaptureTimer.startOneShot(WHEEL_EVENT_CAPTURE_TIMEOUT_IN_MS * 0.001);
    } else if (swEvent.m_pressed)
        WKViewScrollBy(m_webView, -deltaX, -deltaY);
    else {
        Manx::WheelEvent event(x, y, deltaX, deltaY, swEvent.m_wheelTicksX, swEvent.m_wheelTicksY, false, false, false, false);
        WKViewHandleWheelEvent(m_webView, &event);
    }
}

void WebView::didNotHandleWheelEvent(WKPageRef page, WKNativeEventPtr event, const void *clientInfo)
{
    WebView* thiz = toWebView(clientInfo);
    Manx::WheelEvent* wheelEvent = (Manx::WheelEvent*)event;
    float deltaX = wheelEvent->m_deltaX;
    float deltaY = wheelEvent->m_deltaY;
    WKViewScrollBy(thiz->m_webView, -deltaX, -deltaY);

    thiz->m_wheelEventCaptureTimer.startOneShot(WHEEL_EVENT_CAPTURE_TIMEOUT_IN_MS * 0.001);
}

void WebView::runJavaScriptAlert(WKPageRef page, WKStringRef alertText, WKFrameRef frame, const void *clientInfo)
{
    WebView* thiz = toWebView(clientInfo);
    thiz->browser()->windowManager()->messageBox(createString(alertText));
}

bool WebView::runJavaScriptConfirm(WKPageRef page, WKStringRef message, WKFrameRef frame, const void *clientInfo)
{
    WebView* thiz = toWebView(clientInfo);
    return (thiz->browser()->windowManager()->messageBox(createString(message), SW::MB_OKCANCEL) == SW::ID_OK);
}

WKStringRef WebView::runJavaScriptPrompt(WKPageRef page, WKStringRef message, WKStringRef defaultValue, WKFrameRef frame, const void *clientInfo)
{
    WebView* thiz = toWebView(clientInfo);
    std::string msg = createString(message);
    std::string text;
    text.append(msg.substr(0, msg.length() - 1));
    text.append("  [defaultValue]->");
    text.append(createString(defaultValue));
    static const char* replyValues[] = { "OK Selected", "CANCEL Selected" };

    return WKStringCreateWithUTF8CString(replyValues[thiz->browser()->windowManager()->messageBox(text, SW::MB_OKCANCEL)]);
}

WKPageRef WebView::createNewPage(WKPageRef page, WKURLRequestRef urlRequest, WKDictionaryRef features, WKEventModifiers modifiers, WKEventMouseButton mouseButton, const void *clientInfo)
{
    WebView* thiz = toWebView(clientInfo);

    WKPageRef newPage = nullptr;
    if (!thiz->autoCruiseMode()) {
        WebView* webView = thiz->m_browser->createWebView(thiz->webContext());
        newPage = WKViewGetPage(webView->m_webView);
        WKRetain(newPage);
    }
    return newPage;
}

void WebView::onWheelEventCaptureTimeout()
{
    // Do nothing
}

bool WebView::handleEvent(SW::Event& event)
{
    switch (event.m_type) {
    case SW::Event::KeyDown:
    case SW::Event::KeyUp:
        dispatchKeyboardEvent(static_cast<SW::KeyboardEvent&>(event));
        break;
    case SW::Event::MouseDown:
    case SW::Event::MouseUp:
    case SW::Event::MouseMove:
        dispatchMouseEvent(static_cast<SW::MouseEvent&>(event));
        break;
    case SW::Event::Wheel:
        dispatchWheelEvent(static_cast<SW::WheelEvent&>(event));
        break;
    }
    return false;
}

void WebView::showPage(WKPageRef page, const void *clientInfo)
{
}

void WebView::closePage(WKPageRef page, const void *clientInfo)
{
    WebView* thiz = toWebView(clientInfo);
    WKPageClose(page);
    thiz->m_browser->closeTab(thiz);
}

unsigned long long WebView::exceededDatabaseQuota(WKPageRef page, WKFrameRef frame, WKSecurityOriginRef origin, WKStringRef databaseName, WKStringRef displayName, unsigned long long currentQuota, unsigned long long currentOriginUsage, unsigned long long currentDatabaseUsage, unsigned long long expectedUsage, const void *clientInfo)
{
    return 5 * 1000 * 1000;
}

void WebView::didReceiveTitleForFrame(WKPageRef page, WKStringRef title, WKFrameRef frame, WKTypeRef userData, const void *clientInfo)
{
    if (!WKFrameIsMainFrame(frame))
        return;
    std::string titleString = createString(title);
    WebView* thiz = toWebView(clientInfo);
    thiz->m_title = titleString;
}

void WebView::didFailProvisionalLoadWithErrorForFrame(WKPageRef page, WKFrameRef frame, WKErrorRef error, WKTypeRef userData, const void *clientInfo)
{
    int errorCode = WKErrorGetErrorCode(error);

    WKRetainPtr<WKStringRef> wkErrorCodeString = adoptWK(WKErrorCopyLocalizedDescription(error));
    std::string errorCodeString = createString(wkErrorCodeString.get());

    int sslVerificationResult = WKErrorGetSslVerificationResult(error);

    WKRetainPtr<WKStringRef> wkSslVerificationResultString = adoptWK(WKErrorCopySslVerificationResultString(error));
    std::string sslVerificationResultString = createString(wkSslVerificationResultString.get());

    printf("### %s : errCodeString    = [0x%08x] %s\n", __FUNCTION__, errorCode, errorCodeString.c_str());
    printf("### %s : sslVerifiyString = [0x%08x] %s\n", __FUNCTION__, sslVerificationResult, sslVerificationResultString.c_str());
}

void WebView::didCommitLoadForFrame(WKPageRef page, WKFrameRef frame, WKTypeRef userData, const void *clientInfo)
{
    if (!WKFrameIsMainFrame(frame))
        return;
    WebView* thiz = toWebView(clientInfo);

    // Get URL
    WKRetainPtr<WKURLRef> wkurl = adoptWK(WKFrameCopyURL(frame));
    std::string urlString = createString(wkurl.get());
    thiz->m_url = urlString;
}

void WebView::didChangeProgress(WKPageRef page, const void* clientInfo)
{
    WebView* thiz = toWebView(clientInfo);
    double progress = WKPageGetEstimatedProgress(page);
    thiz->m_progress = progress * 100;
}

void WebView::didSameDocumentNavigationForFrame(WKPageRef page, WKFrameRef frame, WKSameDocumentNavigationType type, WKTypeRef userData, const void *clientInfo)
{
    if (!WKFrameIsMainFrame(frame))
        return;

    didCommitLoadForFrame(page, frame, userData, clientInfo);
}

void WebView::didReceiveAuthenticationChallengeInFrame(WKPageRef page, WKFrameRef frame, WKAuthenticationChallengeRef authenticationChallenge, const void *clientInfo)
{
    static const wchar_t* const title[][2] = {
        { L"Enter User ID for HTTP auth.", L"Enter password for HTTP Auth." },
        { L"Enter User ID for Proxy auth.", L"Enter password for Proxy Auth." }
    };

    if (!authenticationChallenge || !clientInfo)
        return;

    WebView* thiz = toWebView(clientInfo);

    WKAuthenticationDecisionListenerRef decisionListener = WKAuthenticationChallengeGetDecisionListener(authenticationChallenge);
    if (!decisionListener)
        return;

    WKProtectionSpaceRef protectionSpace = WKAuthenticationChallengeGetProtectionSpace(authenticationChallenge);
    if (!protectionSpace) {
        WKAuthenticationDecisionListenerCancel(decisionListener);
        return;
    }

    bool isProxy = WKProtectionSpaceGetIsProxy(protectionSpace);

    std::string userIdBuf;
    thiz->showImeDialog(title[isProxy][0], userIdBuf);

    if (!userIdBuf.length()) {
        WKAuthenticationDecisionListenerCancel(decisionListener);
        return;
    }

    std::string passwordBuf;
    thiz->showImeDialog(title[isProxy][1], passwordBuf);

    WKRetainPtr<WKStringRef> userId(AdoptWK, WKStringCreateWithUTF8CString(userIdBuf.c_str()));
    WKRetainPtr<WKStringRef> password(AdoptWK, WKStringCreateWithUTF8CString(passwordBuf.c_str()));
    WKRetainPtr<WKCredentialRef> credential(AdoptWK, WKCredentialCreate(userId.get(), password.get(), kWKCredentialPersistenceForSession));
    WKAuthenticationDecisionListenerUseCredential(decisionListener, credential.get());
}

void WebView::setIsVisible(bool isVisible)
{
    WKViewSetFocused(m_webView, isVisible);
    WKViewSetIsVisible(m_webView, isVisible);
    WKViewSetActive(m_webView, isVisible);
}

void WebView::paint(SW::GraphicsContext* graphicsContext, const SW::Rect& rect)
{
    if (m_isAcceleratedCompositingMode)
        return;
    graphicsContext->drawTexture(rect.m_pos, rect.m_size, m_texture.get());
}

void WebView::paintToCurrentGLContext(const SW::Rect& clipRect)
{
    if (!m_isAcceleratedCompositingMode)
        return;
    WKViewPaintFlags flags = 0;
    if (SW::WindowManager::renderMode() == SW::RenderModeIndirect)
        flags |= kWKViewPaintFlagIndirectCompositing;
    WKViewPaintToCurrentGLContext(m_webView, WKRectMake(clipRect.m_pos.m_x, clipRect.m_pos.m_y, clipRect.m_size.m_width, clipRect.m_size.m_height), flags);
}

SW::CanvasHole WebView::canvasHole(uint32_t index)
{
    WKCanvasHole wkCanvasHole = WKViewGetCanvasHole(m_webView, index);
    SW::CanvasHole canvasHole;
    memcpy(canvasHole.quad, wkCanvasHole.quad, sizeof(canvasHole.quad));
    canvasHole.opacity = wkCanvasHole.opacity;
    canvasHole.canvasHandle = wkCanvasHole.canvasHandle;
    canvasHole.flags = wkCanvasHole.flags;
    return canvasHole;
}

void WebView::setViewNeedsDisplay(WKViewRef view, WKRect rect, const void* clientInfo)
{
    WebView* thiz = toWebView(clientInfo);

    SW::Texture* texture = thiz->m_texture.get();
    if (!texture)
        return;

    assert(texture->buffer());
    assert(texture->pixelFormat() == SW::Texture::PixelFormatARGB32);
    assert(texture->strideInBytes() == texture->size().m_width * sizeof(uint32_t));

    WKSize size;
    size.width = texture->size().m_width;
    size.height = texture->size().m_height;
    WKViewPaint(view, (unsigned char*)texture->buffer(), size, rect);
}

void WebView::enterAcceleratedCompositingMode(WKViewRef, const void* clientInfo)
{
    WebView* thiz = toWebView(clientInfo);
    thiz->m_isAcceleratedCompositingMode = true;
}

void WebView::exitAcceleratedCompositingMode(WKViewRef, const void* clientInfo)
{
    WebView* thiz = toWebView(clientInfo);
    thiz->m_isAcceleratedCompositingMode = false;
}

void WebView::decidePolicyForNavigationAction(WKPageRef page, WKFrameRef frame, WKFrameNavigationType navigationType, WKEventModifiers modifiers, WKEventMouseButton mouseButton, WKURLRequestRef request, WKFramePolicyListenerRef listener, WKTypeRef userData, const void* clientInfo)
{
    bool policyUse = true;
    // This is a same logic in WebKit/manx/WebCoreSupport/FrameLoaderClientManx.cpp.
    // FrameLoaderClientManx::dispatchDecidePolicyForNavigationAction
    if (WKFrameIsMainFrame(frame)) {
        // prepare necessary datas.
        WKRetainPtr<WKURLRef> nexturl = adoptWK(WKURLRequestCopyURL(request));
        WKRetainPtr<WKURLRef> oldurl = adoptWK(WKFrameCopyURL(frame));

        if (oldurl) {
            // for debug
            std::string nextstring = createString(nexturl.get());
            std::string oldstring = createString(oldurl.get());

            printf("### WebView::decidePolicyForNavigationAction\n");
            printf("    nextURL = [%s]\n", nextstring.c_str());
            printf("    oldURL = [%s]\n", oldstring.c_str());

            // end debug

            WKRetainPtr<WKStringRef> nextscheme = adoptWK(WKURLCopyScheme(nexturl.get()));
            WKRetainPtr<WKStringRef> oldscheme = adoptWK(WKURLCopyScheme(oldurl.get()));

            bool handleNavigation = false;
            bool supportScheme = false;

            // Navigation Confirm
            // Please check Navigation Type here.
            switch (navigationType) {
            case kWKFrameNavigationTypeLinkClicked:
            case kWKFrameNavigationTypeFormSubmitted:
            case kWKFrameNavigationTypeBackForward:
            case kWKFrameNavigationTypeReload:
            case kWKFrameNavigationTypeFormResubmitted:
            case kWKFrameNavigationTypeOther:
                handleNavigation = true;
                break;
            }

            policyUse = false;
            if (handleNavigation) {
                // Unsupported Scheme (http,https/about/file/data)
                // Please check if it is an unsupported scheme here.
                supportScheme = true;

                // Navigation Warning
                // Please check if it is a HTTPS -> HTTP redirection here
                bool nextIsSecure = WKStringIsEqualToUTF8CStringIgnoringCase(nextscheme.get(), "https");
                bool oldIsSecure = WKStringIsEqualToUTF8CStringIgnoringCase(oldscheme.get(), "https");
                if (!nextIsSecure && oldIsSecure)
                    printf("#### NavigationWarning HTTPS->HTTP ####\n");

                if (supportScheme)
                    policyUse = true;
            }
        }
    }

    if (policyUse)
        WKFramePolicyListenerUse(listener);
    else
        WKFramePolicyListenerIgnore(listener);
}

void WebView::decidePolicyForNewWindowAction(WKPageRef page, WKFrameRef frame, WKFrameNavigationType navigationType, WKEventModifiers modifiers, WKEventMouseButton mouseButton, WKURLRequestRef request, WKStringRef frameName, WKFramePolicyListenerRef listener, WKTypeRef userData, const void* clientInfo)
{
    WebView* thiz = toWebView(clientInfo);

    bool policyUse = !thiz->autoCruiseMode();

    if (policyUse)
        WKFramePolicyListenerUse(listener);
    else
        WKFramePolicyListenerIgnore(listener);
}

void WebView::decidePolicyForResponse(WKPageRef page, WKFrameRef frame, WKURLResponseRef response, WKURLRequestRef request, WKFramePolicyListenerRef listener, WKTypeRef userData, const void* clientInfo)
{
    WKFramePolicyListenerUse(listener);
    return;
}

void WebView::showPopupMenu(WKViewRef view, WKArrayRef popupMenuItem, WKRect rect, int32_t selectedIndex, const void* clientInfo)
{
    WebView* thiz = toWebView(clientInfo);
    size_t count = WKArrayGetSize(popupMenuItem);

    if (count <= 0)
        return;

    SW::MenuDialog popupMenu(thiz->browser()->windowManager());

    for (size_t i = 0; i < count; i++) {
        WKTypeRef item = WKArrayGetItemAtIndex(popupMenuItem, i);
        if (WKGetTypeID(item) == WKPopupMenuItemGetTypeID()) {
            WKPopupMenuItemRef popupMenuItem = static_cast<WKPopupMenuItemRef>(item);
            WKRetainPtr<WKStringRef> itemLabel = WKPopupMenuItemCopyText(popupMenuItem);
            popupMenu.appendItem(createString(itemLabel.get()), i);
        }
    }

    popupMenu.setSelectedIndex(selectedIndex);
    int selectedId = popupMenu.show();

    WKViewValueChangedForPopupMenu(view, selectedId);
}
void WebView::hidePopupMenu(WKViewRef view, const void* clientInfo)
{
}

void WebView::doneWithKeyEvent(WKViewRef view, const Manx::KeyboardEvent* event, bool wasEventHandled, const void* clientInfo)
{
    if (wasEventHandled)
        return;
    if (event->m_type != Manx::KeyboardEvent::KeyDown)
        return;
    switch (event->m_nativeVirtualKeyCode) {
    case PSX_UP:
    case PSX_DOWN:
    case PSX_LEFT:
    case PSX_RIGHT:
        printf("CWebView::doneWithKeyEvent %d\n", event->m_nativeVirtualKeyCode);
        break;
    }
}

void WebView::doneWithMouseUpEvent(WKViewRef view, bool wasEventHandled, const void* clientInfo)
{
    WebView* thiz = toWebView(clientInfo);

    if (!thiz->m_isContentEditable)
        return;

    thiz->showImeDialog();

    WKPageBlurFocusedNode(WKViewGetPage(thiz->m_webView));
}

void WebView::setCursorPosition(WKViewRef view, WKPoint cursorPosition, const void* clientInfo)
{
    WebView* thiz = toWebView(clientInfo);
    int x, y;
    thiz->windowPosToScreenPos(cursorPosition.x, cursorPosition.y, x, y);
    thiz->m_browser->windowManager()->setCursorPosition(x, y);

    if (!thiz->isDebugAccessibilityEnabled())
        return;

    WKViewAccessibilityHitTest(view, cursorPosition);
}

void WebView::didChangeEditorState(WKViewRef view, Manx::Ime::EditorState editorState, WKRect inputRect, WKStringRef inputText, const void* clientInfo)
{
    WebView* thiz = toWebView(clientInfo);

    thiz->m_isContentEditable = editorState.m_contentEditable;
    if (!thiz->m_isContentEditable)
        return;
    
    WKPageSetCaretVisible(WKViewGetPage(thiz->m_webView), false);
    
    std::string s = createString(inputText);
    std::wstring ws = fromUtf8(s);
    
    wcscpy_s(thiz->m_imeBuffer, IME_BUFFER_SIZE, ws.c_str());
    thiz->m_inputFieldType = editorState.m_type;
    thiz->m_inputFieldLanguage = editorState.m_lang;
    thiz->m_isInLoginForm = editorState.m_isInLoginForm;
}

void WebView::runCertificateVerificationRequest(WKViewRef view, WKFrameRef frame, int error, WKStringRef url, WKArrayRef certificates, const void* clientInfo)
{
    std::string message = "Invalid Certificate";
    std::string strUrl = createString(url);
    printf("URL  : %s\nError: %d\n", strUrl.c_str(), error);

    size_t certsLen = WKArrayGetSize(certificates);
    for (size_t i = 0; i < certsLen; i++) {
        WKStringRef certRef = static_cast<WKStringRef>(WKArrayGetItemAtIndex(certificates, i));
        std::string certRaw = createString(certRef);
        printf("Certificate[%lu]:\n%s\n", i, certRaw.c_str());
    }

    WebView* thiz = toWebView(clientInfo);
    bool verified = thiz->browser()->windowManager()->messageBox(message, SW::MB_OKCANCEL) == SW::ID_OK ? true : false;
    WKViewReplyCertificateVerification(view, verified);
}

bool WebView::autoCruiseMode()
{
    return m_autoCruiseTimer.isActive();
}

void WebView::setAutoCruiseMode(bool enable)
{
    if (enable)
        m_autoCruiseTimer.startRepeating(10);
}

void WebView::onAutoCruiseTimer()
{
    load("http://www.randomwebsite.com/cgi-bin/random.pl");
}

void WebView::setUserAgent(const char *userAgent)
{
    WKRetainPtr<WKStringRef> userAgentStringRef = adoptWK(WKStringCreateWithUTF8CString(userAgent));
    WKPageSetCustomUserAgent(WKViewGetPage(m_webView), userAgentStringRef.get());
}

void WebView::toggleZoomState()
{
    m_zoomState = (m_zoomState + 1) % 3;
    double zoomFactor = m_zoomState + 1;
    WKPageSetPageZoomFactor(WKViewGetPage(m_webView), zoomFactor);
}

bool WebView::isIFilterProxyMenuEnabled()
{
    return (s_iFilterProxyServer && (s_iFilterProxyServer[0] != '\0'));
}

bool WebView::isIFilterProxyEnabled()
{
    return m_enabledIFilterProxy;
}

void WebView::toggleIFilterProxy()
{
    if (!isIFilterProxyMenuEnabled())
        return;

    m_enabledIFilterProxy = !m_enabledIFilterProxy;

    WKRetainPtr<WKStringRef> server = adoptWK(WKStringCreateWithUTF8CString(""));
    WKRetainPtr<WKStringRef> userId = adoptWK(WKStringCreateWithUTF8CString(""));
    WKRetainPtr<WKStringRef> password = adoptWK(WKStringCreateWithUTF8CString(""));
    int port = 0;

    if (m_enabledIFilterProxy) {
        server = adoptWK(WKStringCreateWithUTF8CString(s_iFilterProxyServer));
        userId = adoptWK(WKStringCreateWithUTF8CString(s_iFilterProxyUserId));
        password = adoptWK(WKStringCreateWithUTF8CString(s_iFilterProxyPassword));
        port = s_iFilterProxyPort;
    }

    WKContextSetIFilterHTTPProxy(m_webContext->context(), m_enabledIFilterProxy, server.get(), port, userId.get(), password.get());
}

bool WebView::isDNTEnabled()
{
    return m_enabledDNT;
}

void WebView::toggleDNT()
{
    m_enabledDNT = !m_enabledDNT;

    WKPreferencesRef pref = WKPageGroupGetPreferences(m_webContext->pageGroup());
    WKPreferencesSetDoNotTrack(pref, m_enabledDNT);
}

bool WebView::isActive() const
{
    return m_isActive;
}

void WebView::setIsActive(bool isActive)
{
    m_isActive = isActive;
    WKViewSetActive(m_webView, isActive);
    if (isActive) {
        WKViewRestoreBackingStores(m_webView);
        WKPageResumeActiveDOMObjectsAndAnimations(WKViewGetPage(m_webView));
    } else {
        WKPageSuspendActiveDOMObjectsAndAnimations(WKViewGetPage(m_webView));
        WKViewClearBackingStores(m_webView);
    }
}

static SW::InputFieldType convertToSWInputFieldType(int fieldType)
{
    switch (fieldType) {
    case Manx::InputFieldTypes::INPUT_FIELD_TEXT:
        return SW::InputFieldType::TEXT;
    case Manx::InputFieldTypes::INPUT_FIELD_PASSWORD:
        return SW::InputFieldType::PASSWORD;
    case Manx::InputFieldTypes::INPUT_FIELD_EMAIL:
        return SW::InputFieldType::EMAIL;
    case Manx::InputFieldTypes::INPUT_FIELD_URL:
        return SW::InputFieldType::URL;
    case Manx::InputFieldTypes::INPUT_FIELD_SEARCH:
        return SW::InputFieldType::SEARCH;
    case Manx::InputFieldTypes::INPUT_FIELD_NUMBER:
        return SW::InputFieldType::NUMBER;
    case Manx::InputFieldTypes::INPUT_FIELD_TELEPHONE:
        return SW::InputFieldType::TELEPHONE;
    case Manx::InputFieldTypes::INPUT_FIELD_TEXTAREA:
        return SW::InputFieldType::TEXTAREA;
    case Manx::InputFieldTypes::INPUT_FIELD_OTHER:
        return SW::InputFieldType::OTHER;
    default:
        return SW::InputFieldType::UNKNOWN_INPUT_FIELD_TYPE;
    }
}

static SW::InputFieldLanguage convertToSWInputFieldLanguage(int fieldLang)
{
    switch (fieldLang) {
    case Manx::InputLanguages::INPUT_LANGUAGE_DANISH:
        return SW::InputFieldLanguage::DANISH;
    case Manx::InputLanguages::INPUT_LANGUAGE_GERMAN:
        return SW::InputFieldLanguage::GERMAN;
    case Manx::InputLanguages::INPUT_LANGUAGE_ENGLISH_US:
        return SW::InputFieldLanguage::ENGLISH_US;
    case Manx::InputLanguages::INPUT_LANGUAGE_SPANISH:
        return SW::InputFieldLanguage::SPANISH;
    case Manx::InputLanguages::INPUT_LANGUAGE_FRENCH:
        return SW::InputFieldLanguage::FRENCH;
    case Manx::InputLanguages::INPUT_LANGUAGE_ITALIAN:
        return SW::InputFieldLanguage::ITALIAN;
    case Manx::InputLanguages::INPUT_LANGUAGE_DUTCH:
        return SW::InputFieldLanguage::DUTCH;
    case Manx::InputLanguages::INPUT_LANGUAGE_NORWEGIAN:
        return SW::InputFieldLanguage::NORWEGIAN;
    case Manx::InputLanguages::INPUT_LANGUAGE_POLISH:
        return SW::InputFieldLanguage::POLISH;
    case Manx::InputLanguages::INPUT_LANGUAGE_PORTUGUESE_PT:
        return SW::InputFieldLanguage::PORTUGUESE_PT;
    case Manx::InputLanguages::INPUT_LANGUAGE_RUSSIAN:
        return SW::InputFieldLanguage::RUSSIAN;
    case Manx::InputLanguages::INPUT_LANGUAGE_FINNISH:
        return SW::InputFieldLanguage::FINNISH;
    case Manx::InputLanguages::INPUT_LANGUAGE_SWEDISH:
        return SW::InputFieldLanguage::SWEDISH;
    case Manx::InputLanguages::INPUT_LANGUAGE_JAPANESE:
        return SW::InputFieldLanguage::JAPANESE;
    case Manx::InputLanguages::INPUT_LANGUAGE_KOREAN:
        return SW::InputFieldLanguage::KOREAN;
    case Manx::InputLanguages::INPUT_LANGUAGE_SIMPLIFIED_CHINESE:
        return SW::InputFieldLanguage::SIMPLIFIED_CHINESE;
    case Manx::InputLanguages::INPUT_LANGUAGE_TRADITIONAL_CHINESE:
        return SW::InputFieldLanguage::TRADITIONAL_CHINESE;
    case Manx::InputLanguages::INPUT_LANGUAGE_PORTUGUESE_BR:
        return SW::InputFieldLanguage::PORTUGUESE_BR;
    case Manx::InputLanguages::INPUT_LANGUAGE_ENGLISH_GB:
        return SW::InputFieldLanguage::ENGLISH_GB;
    case Manx::InputLanguages::INPUT_LANGUAGE_TURKISH:
        return SW::InputFieldLanguage::TURKISH;
    case Manx::InputLanguages::INPUT_LANGUAGE_SPANISH_LATIN_AMERICA:
        return SW::InputFieldLanguage::SPANISH_LATIN_AMERICA;
    default:
        return SW::InputFieldLanguage::UNKNOWN_LANGUAGE;
    }
}

void WebView::showImeDialog()
{
    SW::ImeDialog imeDialog(browser()->windowManager());

    SW::ImeDialogParam param = { };

    param.type = convertToSWInputFieldType(m_inputFieldType);
    param.lang = convertToSWInputFieldLanguage(m_inputFieldLanguage);
    param.isInLoginForm = m_isInLoginForm;
    param.inputBuffer = m_imeBuffer;
    param.bufferLength = IME_BUFFER_SIZE;
    param.pos_x = DISPLAY_WIDTH / 2;
    param.pos_y = DISPLAY_HEIGHT / 2;
    param.title = 0;

    WKPageSetCaretVisible(WKViewGetPage(m_webView), true);

    if (!imeDialog.show(param))
        return;

    Manx::KeyboardEvent keyDownEvent(Manx::KeyboardEvent::KeyDown, Manx::VK_PROCESSKEY, false, false, false, false, false);
    WKViewHandleKeyboardEvent(m_webView, &keyDownEvent);

    std::wstring ws(m_imeBuffer);
    std::string s = toUtf8(ws);
    Manx::ImeEvent imeEvent(Manx::ImeEvent::SetValueToFocusedNode, s.c_str(), 0);
    WKViewHandleImeEvent(m_webView, &imeEvent);

    Manx::KeyboardEvent keyUpEvent(Manx::KeyboardEvent::KeyUp, Manx::VK_PROCESSKEY, false, false, false, false, false);
    WKViewHandleKeyboardEvent(m_webView, &keyUpEvent);
}

void WebView::showImeDialog(const wchar_t* title, std::string& outputBuffer)
{
    wchar_t inputBuffer[IME_BUFFER_SIZE];
    memset(inputBuffer, 0, sizeof(inputBuffer));

    SW::ImeDialog imeDialog(browser()->windowManager());

    SW::ImeDialogParam param = { };

    param.type = SW::InputFieldType::TEXT;
    param.lang = SW::InputFieldLanguage::ENGLISH_US;
    param.isInLoginForm = false;
    param.inputBuffer = inputBuffer;
    param.bufferLength = IME_BUFFER_SIZE;
    param.pos_x = DISPLAY_WIDTH / 2;
    param.pos_y = DISPLAY_HEIGHT / 2;
    param.title = title;

    if (!imeDialog.show(param))
        return;

    std::wstring ws(inputBuffer);
    outputBuffer = toUtf8(ws);
}

bool WebView::isDebugAccessibilityEnabled()
{
    return m_enabledDebugAccessibility;
}

void WebView::toggleDebugAccessibility()
{
    m_enabledDebugAccessibility = !m_enabledDebugAccessibility;
}

static std::map<WKAXNotification, const char*> axNotificationMap = {
    { kWKAXActiveDescendantChanged, "ActiveDescendantChanged" },
    { kWKAXAutocorrectionOccured, "AutocorrectionOccured" },
    { kWKAXCheckedStateChanged, "CheckedStateChanged" },
    { kWKAXChildrenChanged, "ChildrenChanged" },
    { kWKAXFocusedUIElementChanged, "FocusedUIElementChanged" },
    { kWKAXLayoutComplete, "LayoutComplete" },
    { kWKAXLoadComplete, "LoadComplete" },
    { kWKAXNewDocumentLoadComplete, "NewDocumentLoadComplete" },
    { kWKAXSelectedChildrenChanged, "SelectedChildrenChanged" },
    { kWKAXSelectedTextChanged, "SelectedTextChanged" },
    { kWKAXValueChanged, "ValueChanged" },
    { kWKAXScrolledToAnchor, "ScrolledToAnchor" },
    { kWKAXLiveRegionCreated, "LiveRegionCreated" },
    { kWKAXLiveRegionChanged, "LiveRegionChanged" },
    { kWKAXMenuListItemSelected, "MenuListItemSelected" },
    { kWKAXMenuListValueChanged, "MenuListValueChanged" },
    { kWKAXMenuClosed, "MenuClosed" },
    { kWKAXMenuOpened, "MenuOpened" },
    { kWKAXRowCountChanged, "RowCountChanged" },
    { kWKAXRowCollapsed, "RowCollapsed" },
    { kWKAXRowExpanded, "RowExpanded" },
    { kWKAXExpandedChanged, "ExpandedChanged" },
    { kWKAXInvalidStatusChanged, "InvalidStatusChanged" },
    { kWKAXTextChanged, "TextChanged" },
    { kWKAXAriaAttributeChanged, "AriaAttributeChanged" },
    { kWKAXElementBusyChanged, "ElementBusyChanged" },
};

static std::map<WKAXTextChange, const char*> axTextChangeMap = {
    { kWKAXTextInserted, "TextInserted" },
    { kWKAXTextDeleted, "TextDeleted" },
};

static std::map<WKAXLoadingEvent, const char*> axLoadingEventMap = {
    { kWKAXLoadingStarted, "LoadingStarted" },
    { kWKAXLoadingReloaded, "LoadingReloaded" },
    { kWKAXLoadingFailed, "LoadingFailed" },
    { kWKAXLoadingFinished, "LoadingFinished" },
};

static std::map<WKAXRole, const char*> axRoleMap = {
    { kWKAnnotationRole, "AnnotationRole" },
    { kWKApplicationRole, "ApplicationRole" },
    { kWKApplicationAlertRole, "ApplicationAlertRole" },
    { kWKApplicationAlertDialogRole, "ApplicationAlertDialogRole" },
    { kWKApplicationDialogRole, "ApplicationDialogRole" },
    { kWKApplicationLogRole, "ApplicationLogRole" },
    { kWKApplicationMarqueeRole, "ApplicationMarqueeRole" },
    { kWKApplicationStatusRole, "ApplicationStatusRole" },
    { kWKApplicationTimerRole, "ApplicationTimerRole" },
    { kWKAudioRole, "AudioRole" },
    { kWKBlockquoteRole, "BlockquoteRole" },
    { kWKBrowserRole, "BrowserRole" },
    { kWKBusyIndicatorRole, "BusyIndicatorRole" },
    { kWKButtonRole, "ButtonRole" },
    { kWKCanvasRole, "CanvasRole" },
    { kWKCaptionRole, "CaptionRole" },
    { kWKCellRole, "CellRole" },
    { kWKCheckBoxRole, "CheckBoxRole" },
    { kWKColorWellRole, "ColorWellRole" },
    { kWKColumnRole, "ColumnRole" },
    { kWKColumnHeaderRole, "ColumnHeaderRole" },
    { kWKComboBoxRole, "ComboBoxRole" },
    { kWKDefinitionRole, "DefinitionRole" },
    { kWKDescriptionListRole, "DescriptionListRole" },
    { kWKDescriptionListTermRole, "DescriptionListTermRole" },
    { kWKDescriptionListDetailRole, "DescriptionListDetailRole" },
    { kWKDetailsRole, "DetailsRole" },
    { kWKDirectoryRole, "DirectoryRole" },
    { kWKDisclosureTriangleRole, "DisclosureTriangleRole" },
    { kWKDivRole, "DivRole" },
    { kWKDocumentRole, "DocumentRole" },
    { kWKDocumentArticleRole, "DocumentArticleRole" },
    { kWKDocumentMathRole, "DocumentMathRole" },
    { kWKDocumentNoteRole, "DocumentNoteRole" },
    { kWKDocumentRegionRole, "DocumentRegionRole" },
    { kWKDrawerRole, "DrawerRole" },
    { kWKEditableTextRole, "EditableTextRole" },
    { kWKFooterRole, "FooterRole" },
    { kWKFormRole, "FormRole" },
    { kWKGridRole, "GridRole" },
    { kWKGroupRole, "GroupRole" },
    { kWKGrowAreaRole, "GrowAreaRole" },
    { kWKHeadingRole, "HeadingRole" },
    { kWKHelpTagRole, "HelpTagRole" },
    { kWKHorizontalRuleRole, "HorizontalRuleRole" },
    { kWKIgnoredRole, "IgnoredRole" },
    { kWKInlineRole, "InlineRole" },
    { kWKImageRole, "ImageRole" },
    { kWKImageMapRole, "ImageMapRole" },
    { kWKImageMapLinkRole, "ImageMapLinkRole" },
    { kWKIncrementorRole, "IncrementorRole" },
    { kWKLabelRole, "LabelRole" },
    { kWKLandmarkApplicationRole, "LandmarkApplicationRole" },
    { kWKLandmarkBannerRole, "LandmarkBannerRole" },
    { kWKLandmarkComplementaryRole, "LandmarkComplementaryRole" },
    { kWKLandmarkContentInfoRole, "LandmarkContentInfoRole" },
    { kWKLandmarkMainRole, "LandmarkMainRole" },
    { kWKLandmarkNavigationRole, "LandmarkNavigationRole" },
    { kWKLandmarkSearchRole, "LandmarkSearchRole" },
    { kWKLegendRole, "LegendRole" },
    { kWKLinkRole, "LinkRole" },
    { kWKListRole, "ListRole" },
    { kWKListBoxRole, "ListBoxRole" },
    { kWKListBoxOptionRole, "ListBoxOptionRole" },
    { kWKListItemRole, "ListItemRole" },
    { kWKListMarkerRole, "ListMarkerRole" },
    { kWKMathElementRole, "MathElementRole" },
    { kWKMatteRole, "MatteRole" },
    { kWKMenuRole, "MenuRole" },
    { kWKMenuBarRole, "MenuBarRole" },
    { kWKMenuButtonRole, "MenuButtonRole" },
    { kWKMenuItemRole, "MenuItemRole" },
    { kWKMenuItemCheckboxRole, "MenuItemCheckboxRole" },
    { kWKMenuItemRadioRole, "MenuItemRadioRole" },
    { kWKMenuListPopupRole, "MenuListPopupRole" },
    { kWKMenuListOptionRole, "MenuListOptionRole" },
    { kWKOutlineRole, "OutlineRole" },
    { kWKParagraphRole, "ParagraphRole" },
    { kWKPopUpButtonRole, "PopUpButtonRole" },
    { kWKPreRole, "" },
    { kWKPresentationalRole, "PresentationalRole" },
    { kWKProgressIndicatorRole, "ProgressIndicatorRole" },
    { kWKRadioButtonRole, "RadioButtonRole" },
    { kWKRadioGroupRole, "RadioGroupRole" },
    { kWKRowHeaderRole, "RowHeaderRole" },
    { kWKRowRole, "RowRole" },
    { kWKRubyBaseRole, "RubyBaseRole" },
    { kWKRubyBlockRole, "RubyBlockRole" },
    { kWKRubyInlineRole, "RubyInlineRole" },
    { kWKRubyRunRole, "RubyRunRole" },
    { kWKRubyTextRole, "RubyTextRole" },
    { kWKRulerRole, "RulerRole" },
    { kWKRulerMarkerRole, "RulerMarkerRole" },
    { kWKScrollAreaRole, "ScrollAreaRole" },
    { kWKScrollBarRole, "ScrollBarRole" },
    { kWKSearchFieldRole, "SearchFieldRole" },
    { kWKSheetRole, "SheetRole" },
    { kWKSliderRole, "SliderRole" },
    { kWKSliderThumbRole, "SliderThumbRole" },
    { kWKSpinButtonRole, "SpinButtonRole" },
    { kWKSpinButtonPartRole, "SpinButtonPartRole" },
    { kWKSplitGroupRole, "SplitGroupRole" },
    { kWKSplitterRole, "SplitterRole" },
    { kWKStaticTextRole, "StaticTextRole" },
    { kWKSummaryRole, "SummaryRole" },
    { kWKSwitchRole, "SwitchRole" },
    { kWKSystemWideRole, "SystemWideRole" },
    { kWKSVGRootRole, "SVGRootRole" },
    { kWKTabGroupRole, "TabGroupRole" },
    { kWKTabListRole, "TabListRole" },
    { kWKTabPanelRole, "TabPanelRole" },
    { kWKTabRole, "TabRole" },
    { kWKTableRole, "TableRole" },
    { kWKTableHeaderContainerRole, "TableHeaderContainerRole" },
    { kWKTextAreaRole, "TextAreaRole" },
    { kWKTreeRole, "TreeRole" },
    { kWKTreeGridRole, "TreeGridRole" },
    { kWKTreeItemRole, "TreeItemRole" },
    { kWKTextFieldRole, "TextFieldRole" },
    { kWKToggleButtonRole, "ToggleButtonRole" },
    { kWKToolbarRole, "ToolbarRole" },
    { kWKUnknownRole, "UnknownRole" },
    { kWKUserInterfaceTooltipRole, "UserInterfaceTooltipRole" },
    { kWKValueIndicatorRole, "ValueIndicatorRole" },
    { kWKVideoRole, "VideoRole" },
    { kWKWebAreaRole, "WebAreaRole" },
    { kWKWebCoreLinkRole, "WebCoreLinkRole" },
    { kWKWindowRole, "WindowRole" },
};

void WebView::dumpAccessibilityObject(WKAXObjectRef axObject)
{
    WKAXRole role = WKAXObjectRole(axObject);

    WKStringRef title = WKAXObjectTitle(axObject);
    size_t maxSize = WKStringGetMaximumUTF8CStringSize(title);
    char* titleStr = new char[maxSize];
    WKStringGetUTF8CString(title, titleStr, maxSize);

    WKStringRef description = WKAXObjectDescription(axObject);
    maxSize = WKStringGetMaximumUTF8CStringSize(description);
    char* descriptionStr = new char[maxSize];
    WKStringGetUTF8CString(description, descriptionStr, maxSize);

    WKStringRef helpText = WKAXObjectHelpText(axObject);
    maxSize = WKStringGetMaximumUTF8CStringSize(helpText);
    char* helpTextStr = new char[maxSize];
    WKStringGetUTF8CString(helpText, helpTextStr, maxSize);

    printf("role:%s, title:%s, description:%s, helpText:%s \n", axRoleMap.at(role), titleStr, descriptionStr, helpTextStr);

    delete[] titleStr;
    delete[] descriptionStr;
    delete[] helpTextStr;
}

void WebView::accessibilityNotification(WKViewRef view, WKAXObjectRef axObject, WKAXNotification notification, const void* clientInfo)
{
    WebView* thiz = toWebView(clientInfo);

    if (!thiz->isDebugAccessibilityEnabled())
        return;

    printf("[AX Notification %s] ", axNotificationMap[notification]);
    dumpAccessibilityObject(axObject);
}

void WebView::accessibilityTextChanged(WKViewRef view, WKAXObjectRef axObject, WKAXTextChange textChange, uint32_t offset, WKStringRef text, const void* clientInfo)
{
    WebView* thiz = toWebView(clientInfo);

    if (!thiz->isDebugAccessibilityEnabled())
        return;

    size_t maxSize = WKStringGetMaximumUTF8CStringSize(text);
    char* textStr = new char[maxSize];
    WKStringGetUTF8CString(text, textStr, maxSize);

    printf("[AX TextChanged %s], text:%s, offset:%d ", axTextChangeMap[textChange], textStr, offset);

    delete[] textStr;
}

void WebView::accessibilityLoadingEvent(WKViewRef view, WKAXObjectRef axObject, WKAXLoadingEvent loadingEvent, const void* clientInfo)
{
    WebView* thiz = toWebView(clientInfo);

    if (!thiz->isDebugAccessibilityEnabled())
        return;

    WKStringRef url = WKAXObjectURL(axObject);
    size_t maxSize = WKStringGetMaximumUTF8CStringSize(url);
    char* urlStr = new char[maxSize];
    WKStringGetUTF8CString(url, urlStr, maxSize);

    printf("[AX LoadingEvent %s] url: %s, ", axLoadingEventMap[loadingEvent], urlStr);
    dumpAccessibilityObject(axObject);

    delete[] urlStr;
}

void WebView::accessibilityRootObject(WKViewRef view, WKAXObjectRef axObject, const void* clientInfo)
{
    WebView* thiz = toWebView(clientInfo);

    if (!thiz->isDebugAccessibilityEnabled())
        return;

    printf("[AX RootObject] ");

    dumpAccessibilityObject(axObject);
}

void WebView::accessibilityFocusedObject(WKViewRef view, WKAXObjectRef axObject, const void* clientInfo)
{
    WebView* thiz = toWebView(clientInfo);

    if (!thiz->isDebugAccessibilityEnabled())
        return;

    printf("[AX FocusedObject] ");

    dumpAccessibilityObject(axObject);
}

void WebView::accessibilityHitTest(WKViewRef view, WKAXObjectRef axObject, const void* clientInfo)
{
    WebView* thiz = toWebView(clientInfo);

    if (!thiz->isDebugAccessibilityEnabled())
        return;

    printf("[AX HitTest] ");

    dumpAccessibilityObject(axObject);
}
