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

#pragma once
#include "WebContext.h"
#include "utils.h"
#include <WebKit/WKContextManx.h>
#include <WebKit/WebKit2_C.h>
#include <pthread.h>
#include <sw/sw.h>

#define DISPLAY_WIDTH 1920
#define DISPLAY_HEIGHT 1080

#define WEBVIEW_WIDTH DISPLAY_WIDTH
#define WEBVIEW_HEIGHT (DISPLAY_HEIGHT - 130)

#define IME_BUFFER_SIZE 1024

class Browser;

class WebView {
public:
    WebView(Browser*, WebContext*);
    ~WebView();
    
    void load(const std::string url);
    bool handleEvent(SW::Event&);
    void stop();
    void reload();
    void goBack();
    void goForward();
    void tryClose();
    bool autoCruiseMode();
    void setAutoCruiseMode(bool enable);
    void toggleZoomState();
    void setUserAgent(const char *);

    const std::string title();
    const std::string url();
    uint8_t progress();
    WebContext* webContext() { return m_webContext; }

    void setIsVisible(bool);
    Browser* browser() { return m_browser; }

    int width() const { return m_viewWidth; }
    int height() const { return m_viewHeight; }

    void paint(SW::GraphicsContext*, const SW::Rect&);
    void paintToCurrentGLContext(const SW::Rect&);
    SW::CanvasHole canvasHole(uint32_t index);

    bool isAcceleratedCompositingMode() const { return m_isAcceleratedCompositingMode; }

    bool isIFilterProxyMenuEnabled();
    bool isIFilterProxyEnabled();
    void toggleIFilterProxy();

    bool isDNTEnabled();
    void toggleDNT();

    bool isDebugAccessibilityEnabled();
    void toggleDebugAccessibility();

    bool isActive() const;
    void setIsActive(bool);

private:
    static void showPage(WKPageRef, const void *clientInfo);
    static void closePage(WKPageRef, const void *clientInfo);
    static void didNotHandleWheelEvent(WKPageRef, WKNativeEventPtr, const void *clientInfo);
    static void runJavaScriptAlert(WKPageRef, WKStringRef alertText, WKFrameRef, const void *clientInfo);
    static bool runJavaScriptConfirm(WKPageRef, WKStringRef message, WKFrameRef, const void *clientInfo);
    static WKStringRef runJavaScriptPrompt(WKPageRef, WKStringRef message, WKStringRef defaultValue, WKFrameRef, const void *clientInfo);
    static unsigned long long exceededDatabaseQuota(WKPageRef, WKFrameRef, WKSecurityOriginRef, WKStringRef databaseName, WKStringRef displayName, unsigned long long currentQuota, unsigned long long currentOriginUsage, unsigned long long currentDatabaseUsage, unsigned long long expectedUsage, const void *clientInfo);
    static WKPageRef createNewPage(WKPageRef, WKURLRequestRef, WKDictionaryRef features, WKEventModifiers, WKEventMouseButton, const void *clientInfo);

    void dispatchKeyboardEvent(SW::KeyboardEvent&);
    void dispatchMouseEvent(SW::MouseEvent&);
    void dispatchWheelEvent(SW::WheelEvent&);

    static void didReceiveTitleForFrame(WKPageRef, WKStringRef title, WKFrameRef, WKTypeRef userData, const void *clientInfo);
    static void didFailProvisionalLoadWithErrorForFrame(WKPageRef, WKFrameRef, WKErrorRef, WKTypeRef userData, const void *clientInfo);
    static void didCommitLoadForFrame(WKPageRef, WKFrameRef, WKTypeRef userData, const void *clientInfo);
    static void didChangeProgress(WKPageRef, const void* clientInfo);
    static void didSameDocumentNavigationForFrame(WKPageRef, WKFrameRef, WKSameDocumentNavigationType, WKTypeRef userData, const void *clientInfo);
    static void didReceiveAuthenticationChallengeInFrame(WKPageRef, WKFrameRef, WKAuthenticationChallengeRef, const void *clientInfo);

    void adjustMousePosition(int inX, int inY, int& outX, int& outY, bool screenPosToWindowPos) const;
    void screenPosToWindowPos(int inX, int inY, int& outX, int& outY) const { adjustMousePosition(inX, inY, outX, outY, true); }
    void windowPosToScreenPos(int inX, int inY, int& outX, int& outY) const { adjustMousePosition(inX, inY, outX, outY, false); }
    static void setViewNeedsDisplay(WKViewRef, WKRect, const void* clientInfo);
    static void enterAcceleratedCompositingMode(WKViewRef, const void* clientInfo);
    static void exitAcceleratedCompositingMode(WKViewRef, const void* clientInfo);
    static void decidePolicyForResponse(WKPageRef, WKFrameRef, WKURLResponseRef, WKURLRequestRef, WKFramePolicyListenerRef, WKTypeRef, const void*);
    static void decidePolicyForNavigationAction(WKPageRef, WKFrameRef, WKFrameNavigationType, WKEventModifiers, WKEventMouseButton, WKURLRequestRef, WKFramePolicyListenerRef, WKTypeRef userData, const void* clientInfo);
    static void decidePolicyForNewWindowAction(WKPageRef, WKFrameRef, WKFrameNavigationType, WKEventModifiers, WKEventMouseButton, WKURLRequestRef, WKStringRef frameName, WKFramePolicyListenerRef, WKTypeRef userData, const void* clientInfo);
    static void doneWithKeyEvent(WKViewRef, const Manx::KeyboardEvent*, bool wasEventHandled, const void* clientInfo);
    static void doneWithMouseUpEvent(WKViewRef, bool wasEventHandled, const void* clientInfo);
    static void setCursorPosition(WKViewRef, WKPoint cursorPosition, const void* clientInfo);
    static void didChangeEditorState(WKViewRef, Manx::Ime::EditorState, WKRect inputRect, WKStringRef inputText, const void* clientInfo);
    static void runCertificateVerificationRequest(WKViewRef, WKFrameRef, int error, WKStringRef url, WKArrayRef certificates, const void* clientInfo);

    static void showPopupMenu(WKViewRef, WKArrayRef popupMenuItem, WKRect, int32_t selectedIndex, const void* clientInfo);
    static void hidePopupMenu(WKViewRef, const void* clientInfo);

    static void accessibilityNotification(WKViewRef, WKAXObjectRef, WKAXNotification, const void* clientInfo);
    static void accessibilityTextChanged(WKViewRef, WKAXObjectRef, WKAXTextChange, uint32_t offset, WKStringRef text, const void* clientInfo);
    static void accessibilityLoadingEvent(WKViewRef, WKAXObjectRef, WKAXLoadingEvent, const void* clientInfo);
    static void accessibilityRootObject(WKViewRef, WKAXObjectRef, const void* clientInfo);
    static void accessibilityFocusedObject(WKViewRef, WKAXObjectRef, const void* clientInfo);
    static void accessibilityHitTest(WKViewRef, WKAXObjectRef, const void* clientInfo);
    static void dumpAccessibilityObject(WKAXObjectRef);

    void onAutoCruiseTimer();
    void onWheelEventCaptureTimeout();

    void showImeDialog();
    void showImeDialog(const wchar_t*, std::string&);

    Browser* m_browser;
    WebContext* m_webContext;
    WKViewRef m_webView;
    
    std::string m_title;
    std::string m_url;
    uint8_t m_progress = 0;
    int m_viewWidth = WEBVIEW_WIDTH;
    int m_viewHeight = WEBVIEW_HEIGHT;
    int m_zoomState = 0;

    std::auto_ptr<SW::Texture> m_texture;
    bool m_isAcceleratedCompositingMode;

    static const char* const s_iFilterProxyServer;
    static const int s_iFilterProxyPort;
    static const char* const s_iFilterProxyUserId;
    static const char* const s_iFilterProxyPassword;
    bool m_enabledIFilterProxy;

    static const char* const s_httpProxyServer;
    static const int s_httpProxyPort;

    bool m_enabledDNT;

    bool m_isActive;

    Timer<WebView> m_autoCruiseTimer;
    Timer<WebView> m_wheelEventCaptureTimer;

    bool m_isContentEditable;
    Manx::InputFieldTypes m_inputFieldType;
    Manx::InputLanguages m_inputFieldLanguage;
    bool m_isInLoginForm;
    wchar_t m_imeBuffer[IME_BUFFER_SIZE];

    bool m_enabledDebugAccessibility;
};
