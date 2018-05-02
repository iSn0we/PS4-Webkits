/*
 * Copyright (C) 2017 Sony Interactive Entertainment Inc.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef WebTileProxy_h
#define WebTileProxy_h

#if ENABLE(MANX_HTMLTILE)

#include "PageClient.h"
#include "WebFullScreenManagerProxy.h"

#include <WebCore/IntSize.h>
#include <WebCore/URL.h>
#include <wtf/RefCounted.h>
#include <wtf/RefPtr.h>

namespace WebKit {

class WebPageGroup;
class WebPageProxy;
class WebTileManagerProxy;

class WebTileProxy : public RefCounted<WebTileProxy>, public PageClient
#if ENABLE(FULLSCREEN_API)
    , public WebFullScreenManagerProxyClient
#endif
{
public:
    static PassRefPtr<WebTileProxy> create(PassRefPtr<WebTileManagerProxy>, WebPageGroup*);
    ~WebTileProxy();

    WebTileManagerProxy* tileManager() const { return m_tileManager.get(); }
    WebPageProxy* page() const { return m_page.get(); }

    uint64_t parentPageID() const { return m_parentPageID; }
    void setParentPageID(uint64_t);
    WebPageProxy* parentPage() const;

    uint64_t tileID() const;
    void setTileID(uint64_t);

    void valid();

    WebCore::IntSize tileSize() const;
    void setTileSize(int, int);

    bool isVisible() const;
    void setVisible(bool);
    bool isFocused() const;
    void setFocused(bool);
    bool isClicked() const;
    void setClicked(bool);

    void pause();
    void resume();

    void evalScript(const String& jsScript, uint64_t callbackID);

    void setTileURL(const String&);
    const WebCore::URL& tileURL() const;
    String actualURL() const;

    void didUpdate();

private:
    WebTileProxy(PassRefPtr<WebTileManagerProxy>, WebPageGroup*);

    // PageClient interface
    virtual std::unique_ptr<DrawingAreaProxy> createDrawingAreaProxy();
    virtual void setViewNeedsDisplay(const WebCore::IntRect&);
    virtual void displayView();
    virtual bool canScrollView();
    virtual void scrollView(const WebCore::IntRect&, const WebCore::IntSize&);
    virtual void requestScroll(const WebCore::FloatPoint& scrollPosition, const WebCore::IntPoint& scrollOrigin, bool isProgrammaticScroll);
    virtual WebCore::IntSize viewSize();
    virtual bool isViewWindowActive();
    virtual bool isViewFocused();
    virtual bool isViewVisible();
    virtual bool isViewInWindow();
    virtual bool isLoaded();

    virtual void processDidExit();
    virtual void didRelaunchProcess();
    virtual void pageClosed();

    virtual void preferencesDidChange();

    virtual void toolTipChanged(const String&, const String&);

    virtual void didCommitLoadForMainFrame(const String& mimeType, bool useCustomContentProvider);

#if USE(COORDINATED_GRAPHICS_MULTIPROCESS)
    virtual void pageDidRequestScroll(const WebCore::IntPoint&);
    virtual void didRenderFrame(const WebCore::IntSize& contentsSize, const WebCore::IntRect& coveredRect);
    virtual void pageTransitionViewportReady();
    virtual void didFindZoomableArea(const WebCore::IntPoint&, const WebCore::IntRect&);
#endif

    virtual void handleDownloadRequest(DownloadProxy*);

    virtual void didChangeContentSize(const WebCore::IntSize&);

#if ENABLE(MANX_CURSOR_NAVIGATION)
    virtual void setCursorPosition(const WebCore::IntPoint&);
#endif

#if PLATFORM(MANX)
    virtual void didUpdateBackingStoreState();
    virtual void didCommitCoordinatedGraphicsState();
#endif

    virtual void setCursor(const WebCore::Cursor&);
    virtual void setCursorHiddenUntilMouseMoves(bool);
    virtual void didChangeViewportProperties(const WebCore::ViewportAttributes&);

    virtual void registerEditCommand(PassRefPtr<WebEditCommandProxy>, WebPageProxy::UndoOrRedo);
    virtual void clearAllEditCommands();
    virtual bool canUndoRedo(WebPageProxy::UndoOrRedo);
    virtual void executeUndoRedo(WebPageProxy::UndoOrRedo);

    virtual WebCore::FloatRect convertToDeviceSpace(const WebCore::FloatRect&);
    virtual WebCore::FloatRect convertToUserSpace(const WebCore::FloatRect&);
    virtual WebCore::IntPoint screenToRootView(const WebCore::IntPoint&);
    virtual WebCore::IntRect rootViewToScreen(const WebCore::IntRect&);

    virtual void doneWithKeyEvent(const NativeWebKeyboardEvent&, bool);
#if ENABLE(TOUCH_EVENTS)
    virtual void doneWithTouchEvent(const NativeWebTouchEvent&, bool wasEventHandled);
#endif
#if PLATFORM(MANX)
    virtual void doneWithMouseDownEvent(bool wasEventHandled);
    virtual void doneWithMouseUpEvent(bool wasEventHandled);
#endif

    virtual PassRefPtr<WebPopupMenuProxy> createPopupMenuProxy(WebPageProxy*);
    virtual PassRefPtr<WebContextMenuProxy> createContextMenuProxy(WebPageProxy*);

    virtual void setTextIndicator(Ref<WebCore::TextIndicator>, WebCore::TextIndicatorLifetime = WebCore::TextIndicatorLifetime::Permanent);
    virtual void clearTextIndicator(WebCore::TextIndicatorDismissalAnimation = WebCore::TextIndicatorDismissalAnimation::FadeOut);
    virtual void setTextIndicatorAnimationProgress(float);

    virtual void enterAcceleratedCompositingMode(const LayerTreeContext&);
    virtual void exitAcceleratedCompositingMode();
    virtual void updateAcceleratedCompositingMode(const LayerTreeContext&);

#if ENABLE(FULLSCREEN_API)
    virtual WebFullScreenManagerProxyClient& fullScreenManagerProxyClient() final;

    // WebFullScreenManagerProxyClient interface
    virtual void closeFullScreenManager();
    virtual bool isFullScreen();
    virtual void enterFullScreen();
    virtual void exitFullScreen();
    virtual void beganEnterFullScreen(const WebCore::IntRect& initialFrame, const WebCore::IntRect& finalFrame);
    virtual void beganExitFullScreen(const WebCore::IntRect& initialFrame, const WebCore::IntRect& finalFrame);
#endif

    virtual void didFinishLoadingDataForCustomContentProvider(const String& suggestedFilename, const IPC::DataReference&);

    virtual void navigationGestureDidBegin();
    virtual void navigationGestureWillEnd(bool willNavigate, WebBackForwardListItem&);
    virtual void navigationGestureDidEnd(bool willNavigate, WebBackForwardListItem&);
    virtual void navigationGestureDidEnd();
    virtual void willRecordNavigationSnapshot(WebBackForwardListItem&);

    virtual void didFirstVisuallyNonEmptyLayoutForMainFrame();
    virtual void didFinishLoadForMainFrame();
    virtual void didFailLoadForMainFrame();
    virtual void didSameDocumentNavigationForMainFrame(SameDocumentNavigationType);

    virtual void didChangeBackgroundColor();

#if ENABLE(VIDEO)
    virtual void mediaDocumentNaturalSizeChanged(const WebCore::IntSize&);
#endif

#if PLATFORM(MANX)
    virtual void didChangeEditorState(const WebKit::EditorState&);
    virtual void didChangeCompositionState(const WebCore::IntRect& compositionRect);
    virtual void didChangeSelectionState(const String& selectedText, const WebCore::IntRect& selectedRect);
    virtual void handleCertificateVerificationRequest(WebFrameProxy*, uint32_t error, const String& url, const Vector<CString>& certificates, PassRefPtr<Messages::WebPageProxy::CertificateVerificationRequest::DelayedReply>);
#if HAVE(ACCESSIBILITY)
    virtual void handleAccessibilityNotification(WebAccessibilityObject* axObject, WebCore::AXObjectCache::AXNotification);
    virtual void handleAccessibilityTextChange(WebAccessibilityObject* axObject, WebCore::AXTextChange, uint32_t offset, const String& text);
    virtual void handleAccessibilityLoadingEvent(WebAccessibilityObject* axObject, WebCore::AXObjectCache::AXLoadingEvent);
    virtual void handleAccessibilityRootObject(WebAccessibilityObject*);
    virtual void handleAccessibilityFocusedObject(WebAccessibilityObject*);
    virtual void handleAccessibilityHitTest(WebAccessibilityObject*);
#endif // HAVE(ACCESSIBILITY)
#endif

    virtual void refView();
    virtual void derefView();

    // End PageClient interface

    // WKPageLoaderClient callbacks
    static void didFinishLoadForFrame(WKPageRef, WKFrameRef, WKTypeRef userData, const void *clientInfo);
    static void didFailProvisionalLoadWithErrorForFrame(WKPageRef, WKFrameRef, WKErrorRef, WKTypeRef userData, const void *clientInfo);

    // WKPagePolicyClient callbacks
    static void decidePolicyForNavigationAction(WKPageRef, WKFrameRef, WKFrameNavigationType, WKEventModifiers, WKEventMouseButton, WKFrameRef originatingFrame, WKURLRequestRef, WKFramePolicyListenerRef, WKTypeRef userData, const void* clientInfo);

    // WKPageUIClient callbacks
    static bool shouldInterruptJavaScript(WKPageRef, const void* clientInfo);
    static unsigned long long exceededDatabaseQuota(WKPageRef, WKFrameRef, WKSecurityOriginRef, WKStringRef databaseName, WKStringRef displayName, unsigned long long currentQuota, unsigned long long currentOriginUsage, unsigned long long currentDatabaseUsage, unsigned long long expectedUsage, const void *clientInfo);

    RefPtr<WebTileManagerProxy> m_tileManager;

    bool m_isVisible;
    bool m_isFocused;
    bool m_isClicked;
    bool m_isLoaded;

    RefPtr<WebPageProxy> m_page;

    uint64_t m_parentPageID;
    uint64_t m_tileID;
    WebCore::IntSize m_tileSize;
    WebCore::URL m_tileURL;
};

} // namespace WebKit

#endif // ENABLE(MANX_HTMLTILE)

#endif // WebTileProxy_h
