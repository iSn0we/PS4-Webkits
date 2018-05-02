/*
 * Copyright (C) 2010 Apple Inc. All rights reserved.
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

#ifndef ManxWebView_h
#define ManxWebView_h

#include "APIObject.h"
#include "ManxWebViewClient.h"
#include "PageClient.h"
#include "PageViewportController.h"
#include "PageViewportControllerClientManx.h"
#include "WebCanvasHole.h"
#include "WebFullScreenManagerProxy.h"
#include "WebPageProxy.h"
#include "WebPopupMenuClient.h"
#include "WebPopupMenuProxyManx.h"
#include "WebViewAccessibilityClient.h"
#include <WKView.h>
#include <WebCore/TextureMapper.h>
#include <wtf/Forward.h>
#include <wtf/PassRefPtr.h>
#include <wtf/RefPtr.h>

namespace WebKit {

class CoordinatedGraphicsScene;

class WebView : public API::ObjectImpl<API::Object::Type::View>, public PageClient, public WebFullScreenManagerProxyClient {
public:
    static PassRefPtr<WebView> create(WebProcessPool* context, WebPageGroup* pageGroup, const WKViewClientBase* client)
    {
        RefPtr<WebView> webView = adoptRef(new WebView(context, pageGroup, client));
        webView->initialize();
        return webView;
    }
    ~WebView();

    void initialize();

    WebPageProxy* page() const { return m_page.get(); }
    void paint(unsigned char*, const WebCore::IntSize&, const WebCore::IntRect&);
    void scrollBy(int x, int y);
    void handleKeyboardEvent(const Manx::KeyboardEvent&);
    void handleImeEvent(const Manx::ImeEvent&);
    void clearSelection();
    void handleMouseEvent(const Manx::MouseEvent&);
    void handleWheelEvent(const Manx::WheelEvent&);
    void setSize(int, int);
    const WebCore::IntSize& size() const { return m_size; }
    void setFocused(bool);
    void setActive(bool);
    void setIsVisible(bool);
    bool isFocused() const { return m_focused; }
    bool isActive() const { return m_active; }
    bool isVisible() const { return m_visible; }
    const WebCore::IntSize& contentsSize() const { return m_contentsSize; }
    WebCore::FloatSize visibleContentsSize() const;

    void setViewBackgroundColor(WebCore::Color);
    WebCore::Color viewBackgroundColor();
    float contentScaleFactor() const { return m_page->pageScaleFactor(); }

    PageViewportController& viewportController();
    void setPageScaleFactor(float newScale);
    void postFakeMouseMove();
    void didChangeContentsVisibility(const WebCore::IntSize&, const WebCore::FloatPoint& position, float scale);

    void setContentPosition(const WebCore::FloatPoint& position) { m_contentPosition = position; }
    const WebCore::FloatPoint& contentPosition() const { return m_contentPosition; }

    void paintToCurrentGLContext(const WebCore::FloatRect&, WebCore::TextureMapper::PaintFlags);
    void setACMemoryInfo(const WebCore::GLMemoryInfo&);

    void clearBackingStores();
    void restoreBackingStores();
    void syncCoordinatedGraphicsState();

    virtual bool isFullScreen() override;
    void willEnterFullScreen();
    void didEnterFullScreen();
    void willExitFullScreen();
    void didExitFullScreen();
    void requestExitFullScreen();

    void setPopupMenuClient(const WKViewPopupMenuClientBase*);
    void showPopupMenu(WebPopupMenuProxyManx*, const Vector<WebPopupItem>&, const WebCore::IntRect&, int32_t);
    void hidePopupMenu();
    void valueChangedForPopupMenu(int);

    void replyCertificateVerificationRequest(bool result);

#if HAVE(ACCESSIBILITY)
    void setAccessibilityClient(const WKViewAccessibilityClientBase*);
    void handleAccessibilityNotification(WebAccessibilityObject*, WebCore::AXObjectCache::AXNotification) override;
    void handleAccessibilityTextChange(WebAccessibilityObject*, WebCore::AXTextChange, uint32_t, const String&) override;
    void handleAccessibilityLoadingEvent(WebAccessibilityObject*, WebCore::AXObjectCache::AXLoadingEvent) override;
    void accessibilityRootObject();
    void accessibilityFocusedObject();
    void accessibilityHitTest(const WebCore::IntPoint&);
    void handleAccessibilityRootObject(WebAccessibilityObject* axObject) override;
    void handleAccessibilityFocusedObject(WebAccessibilityObject* axObject) override;
    void handleAccessibilityHitTest(WebAccessibilityObject* axObject) override;
#endif

#if ENABLE(MANX_INDIRECT_COMPOSITING)
    WebCanvasHole canvasHole(uint32_t index) const;
#endif

private:
    WebView(WebProcessPool*, WebPageGroup*, const WKViewClientBase*);

    // PageClient
    virtual std::unique_ptr<DrawingAreaProxy> createDrawingAreaProxy() override;
    virtual void setViewNeedsDisplay(const WebCore::IntRect&) override;
    virtual void displayView() override;
    virtual bool canScrollView() override;
    virtual void scrollView(const WebCore::IntRect&, const WebCore::IntSize&) override;
    virtual void requestScroll(const WebCore::FloatPoint& scrollPosition, const WebCore::IntPoint& scrollOrigin, bool isProgrammaticScroll) override;

    virtual WebCore::IntSize viewSize() override;
    virtual bool isViewWindowActive() override;
    virtual bool isViewFocused() override;
    virtual bool isViewVisible() override;
    virtual bool isViewInWindow() override;

    virtual void processDidExit() override;
    virtual void didRelaunchProcess() override;
    virtual void pageClosed() override;

    virtual void preferencesDidChange() override;

    virtual void toolTipChanged(const String&, const String&) override;

    virtual void didCommitLoadForMainFrame(const String& mimeType, bool useCustomContentProvider) override;

    virtual void handleDownloadRequest(DownloadProxy*) override;

#if USE(COORDINATED_GRAPHICS_MULTIPROCESS)
    virtual void pageDidRequestScroll(const WebCore::IntPoint&) override;
    virtual void didRenderFrame(const WebCore::IntSize& contentsSize, const WebCore::IntRect& coveredRect) override;
    virtual void pageTransitionViewportReady() override;
    virtual void didFindZoomableArea(const WebCore::IntPoint&, const WebCore::IntRect&) override;
#endif
    virtual void didChangeContentSize(const WebCore::IntSize&) override;

#if ENABLE(MANX_CURSOR_NAVIGATION)
    virtual void setCursorPosition(const WebCore::IntPoint&) override;
#endif

#if ENABLE(MANX_ADVANCED_ACCELERATED_COMPOSITING)
    virtual void didUpdateBackingStoreState() override;
    virtual void didCommitCoordinatedGraphicsState() override;
#endif

    virtual void setCursor(const WebCore::Cursor&) override;
    virtual void setCursorHiddenUntilMouseMoves(bool) override;
    virtual void didChangeViewportProperties(const WebCore::ViewportAttributes&) override;

    virtual void registerEditCommand(PassRefPtr<WebEditCommandProxy>, WebPageProxy::UndoOrRedo) override;
    virtual void clearAllEditCommands() override;
    virtual bool canUndoRedo(WebPageProxy::UndoOrRedo) override;
    virtual void executeUndoRedo(WebPageProxy::UndoOrRedo) override;

    virtual WebCore::FloatRect convertToDeviceSpace(const WebCore::FloatRect&) override;
    virtual WebCore::FloatRect convertToUserSpace(const WebCore::FloatRect&) override;
    virtual WebCore::IntPoint screenToRootView(const WebCore::IntPoint&) override;
    virtual WebCore::IntRect rootViewToScreen(const WebCore::IntRect&) override;

    virtual void didChangeEditorState(const WebKit::EditorState&) override;
    virtual void didChangeCompositionState(const WebCore::IntRect& compositionRect) override;
    virtual void didChangeSelectionState(const String& selectedText, const WebCore::IntRect& selectedRect) override;

    virtual void doneWithKeyEvent(const NativeWebKeyboardEvent&, bool) override;
#if ENABLE(GESTURE_EVENTS)
    virtual void doneWithGestureEvent(const WebGestureEvent&, bool wasEventHandled) override;
#endif
#if ENABLE(TOUCH_EVENTS)
    virtual void doneWithTouchEvent(const NativeWebTouchEvent&, bool wasEventHandled) override;
#endif
    virtual void doneWithMouseDownEvent(bool wasEventHandled) override;
    virtual void doneWithMouseUpEvent(bool wasEventHandled) override;

    virtual PassRefPtr<WebPopupMenuProxy> createPopupMenuProxy(WebPageProxy*) override;
    virtual PassRefPtr<WebContextMenuProxy> createContextMenuProxy(WebPageProxy*) override;

    virtual void setTextIndicator(Ref<WebCore::TextIndicator>, WebCore::TextIndicatorLifetime = WebCore::TextIndicatorLifetime::Permanent) override;
    virtual void clearTextIndicator(WebCore::TextIndicatorDismissalAnimation = WebCore::TextIndicatorDismissalAnimation::FadeOut) override;
    virtual void setTextIndicatorAnimationProgress(float) override;

    virtual void enterAcceleratedCompositingMode(const LayerTreeContext&) override;
    virtual void exitAcceleratedCompositingMode() override;
    virtual void updateAcceleratedCompositingMode(const LayerTreeContext&) override;

    virtual void didFinishLoadingDataForCustomContentProvider(const String& suggestedFilename, const IPC::DataReference&) override;

    virtual void navigationGestureDidBegin() override;
    virtual void navigationGestureWillEnd(bool willNavigate, WebBackForwardListItem&) override;
    virtual void navigationGestureDidEnd(bool willNavigate, WebBackForwardListItem&) override;
    virtual void navigationGestureDidEnd() override;
    virtual void willRecordNavigationSnapshot(WebBackForwardListItem&) override;

    virtual void didFirstVisuallyNonEmptyLayoutForMainFrame() override;
    virtual void didFinishLoadForMainFrame() override;
    virtual void didFailLoadForMainFrame() override;
    virtual void didSameDocumentNavigationForMainFrame(SameDocumentNavigationType) override;

    virtual void didChangeBackgroundColor() override;

    virtual WebFullScreenManagerProxyClient& fullScreenManagerProxyClient() override;

    // WebFullScreenManagerProxyClient
    virtual void closeFullScreenManager() override;
    virtual void enterFullScreen() override;
    virtual void exitFullScreen() override;
    virtual void beganEnterFullScreen(const WebCore::IntRect& initialFrame, const WebCore::IntRect& finalFrame) override;
    virtual void beganExitFullScreen(const WebCore::IntRect& initialFrame, const WebCore::IntRect& finalFrame) override;

#if ENABLE(WIRELESS_PLAYBACK_TARGET)
    virtual WebCore::WebMediaSessionManager& mediaSessionManager() override;
#endif

#if ENABLE(VIDEO)
    virtual void mediaDocumentNaturalSizeChanged(const WebCore::IntSize&) override;
#endif

    virtual void handleCertificateVerificationRequest(WebFrameProxy*, uint32_t error, const String& url, const Vector<CString>& certificates, PassRefPtr<Messages::WebPageProxy::CertificateVerificationRequest::DelayedReply>) override;

    virtual void refView() override;
    virtual void derefView() override;

    void updateViewportSize();
    WebCore::FloatSize dipSize() const;

    bool useFixedLayout() const;

#if USE(COORDINATED_GRAPHICS)
    CoordinatedGraphicsScene* coordinatedGraphicsScene();
#endif

    RefPtr<WebPageProxy> m_page;
    WebViewClient m_client;
    WebPopupMenuProxyManx* m_popupMenu;
    WebPopupMenuClient m_popupMenuClient;
#if HAVE(ACCESSIBILITY)
    WebViewAccessibilityClient m_axClient;
#endif
    WebCore::IntSize m_size; // Size in device units.
    WebCore::FloatPoint m_contentPosition; // Position in UI units.
    WebCore::IntSize m_contentsSize;
    WebCore::IntPoint m_lastMousePosition;
    bool m_focused;
    bool m_active;
    bool m_visible;
    bool m_isFullScreen;
    bool m_acceleratedCompositingEnabled;

    RefPtr<Messages::WebPageProxy::CertificateVerificationRequest::DelayedReply> m_certReply;

    EditorState m_editorState;

#if USE(COORDINATED_GRAPHICS_MULTIPROCESS) && OS(ORBIS)
    WebCore::GLMemoryInfo m_glMemoryInfo;
#endif

    PageViewportControllerClientManx m_viewportControllerClient;
    std::unique_ptr<PageViewportController> m_viewportController;
};

} // namespace WebKit

#endif // ManxWebView_h
