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

#include "config.h"
#include "WebTileProxy.h"

#if ENABLE(MANX_HTMLTILE)

#include "NativeWebMouseEvent.h"
#include "NotImplemented.h"
#include "WebContextMenuProxy.h"
#include "WebFramePolicyListenerProxy.h"
#include "WebPageGroup.h"
#include "WebPageProxy.h"
#include "WebTileDrawingAreaProxy.h"
#include "WebTileManagerProxy.h"

using namespace WebCore;

namespace WebKit {

PassRefPtr<WebTileProxy> WebTileProxy::create(PassRefPtr<WebTileManagerProxy> tileManager, WebPageGroup* pageGroup)
{
    return adoptRef(new WebTileProxy(tileManager, pageGroup));
}

WebTileProxy::WebTileProxy(PassRefPtr<WebTileManagerProxy> tileManager, WebPageGroup* pageGroup)
    : m_tileManager(tileManager)
    , m_isVisible(false)
    , m_isFocused(true)
    , m_isClicked(false)
    , m_isLoaded(false)
    , m_parentPageID(0)
    , m_tileID(0)
{
    // Disable Accelerated Compositing for Web Tiles.
    WebPreferences* preferences = &(pageGroup->preferences());
    preferences->setAcceleratedCompositingEnabled(false);
    pageGroup->setPreferences(preferences);

    WebPageConfiguration webPageConfiguration;
    webPageConfiguration.pageGroup = pageGroup;
    m_page = m_tileManager->webTilesProcessPool()->createWebPage(*this, WTF::move(webPageConfiguration));

    WKPageLoaderClientV0 loaderClient = {
        { 0, this },
        nullptr, // didStartProvisionalLoadForFrame
        nullptr, // didReceiveServerRedirectForProvisionalLoadForFrame
        didFailProvisionalLoadWithErrorForFrame,
        nullptr, // didCommitLoadForFrame
        nullptr, // didFinishDocumentLoadForFrame
        didFinishLoadForFrame,
        nullptr, // didFailLoadWithErrorForFrame
        nullptr, // didSameDocumentNavigationForFrame
        nullptr, // didReceiveTitleForFrame
        nullptr, // didFirstLayoutForFrame
        nullptr, // didFirstVisuallyNonEmptyLayoutForFrame
        nullptr, // didRemoveFrameFromHierarchy
        nullptr, // didDisplayInsecureContentForFrame
        nullptr, // didRunInsecureContentForFrame
        nullptr, // canAuthenticateAgainstProtectionSpaceInFrame
        nullptr, // didReceiveAuthenticationChallengeInFrame
        nullptr, // didStartProgress    TODO: to track progress
        nullptr, // didChangeProgress   TODO: to track progress
        nullptr, // didFinishProgress   TODO: to track progress
        nullptr, // processDidBecomeUnresponsive
        nullptr, // processDidBecomeResponsive
        nullptr, // processDidCrash
        nullptr, // didChangeBackForwardList
        nullptr, // shouldGoToBackForwardListItem
        nullptr // didFailToInitializePlugin_deprecatedForUseWithV0
    };
    WKPageSetPageLoaderClient(toAPI(m_page.get()), &loaderClient.base);

    WKPagePolicyClientV1 policyClient = {
        { 1, this },
        nullptr, // decidePolicyForNavigationAction_deprecatedForUseWithV0
        nullptr, // decidePolicyForNewWindowAction  TODO: if we want to track contents more closely.
        nullptr, // decidePolicyForResponse_deprecatedForUseWithV0
        nullptr, // unableToImplementPolicy
        decidePolicyForNavigationAction,
        nullptr // decidePolicyForResponse  TODO: if we want to track contents more closely.
    };
    WKPageSetPagePolicyClient(toAPI(m_page.get()), &policyClient.base);

    WKPageUIClientV0 uiClient = {
        { 0, this },
        nullptr, // createNewPage_deprecatedForUseWithV0
        nullptr, // showPage
        nullptr, // close
        nullptr, // takeFocus
        nullptr, // focus
        nullptr, // unfocus
        nullptr, // runJavaScriptAlert
        nullptr, // runJavaScriptConfirm
        nullptr, // runJavaScriptPrompt
        nullptr, // setStatusText
        nullptr, // mouseDidMoveOverElement_deprecatedForUseWithV0
        nullptr, // missingPluginButtonClicked_deprecatedForUseWithV0
        nullptr, // didNotHandleKeyEvent
        nullptr, // didNotHandleWheelEvent
        nullptr, // toolbarsAreVisible
        nullptr, // setToolbarsAreVisible
        nullptr, // menuBarIsVisible
        nullptr, // setMenuBarIsVisible
        nullptr, // statusBarIsVisible
        nullptr, // setStatusBarIsVisible
        nullptr, // isResizable
        nullptr, // setIsResizable
        nullptr, // getWindowFrame
        nullptr, // setWindowFrame
        nullptr, // runBeforeUnloadConfirmPanel
        nullptr, // didDraw
        nullptr, // pageDidScroll
        exceededDatabaseQuota,
        nullptr, // runOpenPanel
        nullptr, // decidePolicyForGeolocationPermissionRequest
        nullptr, // headerHeight
        nullptr, // footerHeight
        nullptr, // drawHeader
        nullptr, // drawFooter
        nullptr, // printFrame
        nullptr, // runModal
        nullptr, // unused1 (Used to be didCompleteRubberBandForMainFrame)
        nullptr, // saveDataToFileInDownloadsFolder
        nullptr // shouldInterruptJavaScript_unavailable
    };
    WKPageSetPageUIClient(toAPI(m_page.get()), &uiClient.base);

    m_page->setDrawsTransparentBackground(true);
}

WebTileProxy::~WebTileProxy()
{
}

WebPageProxy* WebTileProxy::parentPage() const
{
    return WebProcessProxy::webPage(m_parentPageID);
}

void WebTileProxy::setParentPageID(uint64_t pageID)
{
    m_parentPageID = pageID;
}

uint64_t WebTileProxy::tileID() const
{
    return m_tileID;
}

void WebTileProxy::setTileID(uint64_t tileID)
{
    m_tileID = tileID;
}

void WebTileProxy::valid()
{
}

IntSize WebTileProxy::tileSize() const
{
    return m_tileSize;
}

void WebTileProxy::setTileSize(int width, int height)
{
    const IntSize newSize(width, height);

    if (newSize == m_tileSize)
        return;

    const bool oldSizeIsZero = m_tileSize.isZero();
    m_tileSize = newSize;

    if (oldSizeIsZero) {
        m_page->initializeWebPage();

        // This reverts the initial m_focused(true) in the constructor, so that we start with an unfocused frame,
        // but with that frame having already been marked as the focus frame during initialization.
        // Otherwise we will get two window.onfocus events the first time we call setFocus(true)
        setFocused(false);
    } else if (m_page->drawingArea())
        m_page->drawingArea()->setSize(m_tileSize, IntSize(), IntSize());
}

bool WebTileProxy::isVisible() const
{
    return m_isVisible;
}

void WebTileProxy::setVisible(bool value)
{
    m_isVisible = value;
    m_page->viewStateDidChange(ViewState::IsVisible);
}

bool WebTileProxy::isFocused() const
{
    return m_isFocused;
}

void WebTileProxy::setFocused(bool value)
{
    m_isFocused = value;

    m_page->viewStateDidChange(ViewState::IsFocused);
}

bool WebTileProxy::isClicked() const
{
    return m_isClicked;
}

void WebTileProxy::setClicked(bool value)
{
    m_isClicked = value;

    Manx::MouseEvent::MouseEventType eventType = (value) ? Manx::MouseEvent::MouseDown : Manx::MouseEvent::MouseUp;
    Manx::MouseEvent mouseEvent(eventType, Manx::MouseEvent::LeftButton, -1000, -1000, 1, false, false, false, false);

    m_page->handleMouseEvent(NativeWebMouseEvent(mouseEvent));
}

bool WebTileProxy::isLoaded()
{
    return m_isLoaded;
}

void WebTileProxy::pause()
{
    setVisible(false);
}

void WebTileProxy::resume()
{
    if (isLoaded())
        setVisible(true);
}

void WebTileProxy::evalScript(const String& jsScript, uint64_t callbackID)
{
    m_page->runJavaScriptInMainFrameWithStringCallback(jsScript, [this, callbackID](const String& resultString, CallbackBase::Error) {
        if (callbackID)
            tileManager()->dispatchRunJavaScriptResultCallback(tileID(), callbackID, resultString);
    });
}

void WebTileProxy::setTileURL(const String& urlString)
{
    const URL url(URL(), urlString);

    if (url == m_tileURL)
        return;

    m_tileURL = url;
    m_page->loadRequest(url);
}

const URL& WebTileProxy::tileURL() const
{
    return m_tileURL;
}

String WebTileProxy::actualURL() const
{
    return m_page->mainFrame() ? m_page->mainFrame()->url() : String();
}

void WebTileProxy::didUpdate()
{
    if (!m_page->drawingArea())
        return;

    static_cast<WebTileDrawingAreaProxy*>(m_page->drawingArea())->didUpdate();
}

// PageClient Interface
std::unique_ptr<DrawingAreaProxy> WebTileProxy::createDrawingAreaProxy()
{
    return WebTileDrawingAreaProxy::create(*page());
}

void WebTileProxy::setViewNeedsDisplay(const IntRect&)
{
    if (!m_page->drawingArea())
        return;

    const UpdateInfo* updateInfo = static_cast<WebTileDrawingAreaProxy*>(m_page->drawingArea())->pendingUpdateInfo();
    ASSERT(updateInfo);

    m_tileManager->dispatchTileUpdated(m_tileID, *updateInfo);
}

void WebTileProxy::displayView()
{
}

bool WebTileProxy::canScrollView()
{
    return false;
}

void WebTileProxy::scrollView(const IntRect&, const IntSize&)
{
    notImplemented();
}

void WebTileProxy::requestScroll(const WebCore::FloatPoint& scrollPosition, const WebCore::IntPoint& scrollOrigin, bool isProgrammaticScroll)
{
    notImplemented();
}

IntSize WebTileProxy::viewSize()
{
    return tileSize();
}

bool WebTileProxy::isViewWindowActive()
{
    return false;
}

bool WebTileProxy::isViewFocused()
{
    return isFocused();
}

bool WebTileProxy::isViewVisible()
{
    return isVisible();
}

bool WebTileProxy::isViewInWindow()
{
    return true;
}

void WebTileProxy::processDidExit()
{
    // TODO: This function used to be called `processDidCrash` in PageClient.
    //   It was changed in https://github.com/WebKit/webkit/commit/1700c87565beb1686591026e588566e4f45bc612
    //   It used to only be called on crash, but now is called any time the process exits, we may need
    //   to re-evaluate how we dispatch the TileCrashed event...
    m_tileManager->dispatchTileCrashed(m_tileID);
}

void WebTileProxy::didRelaunchProcess()
{
}

void WebTileProxy::pageClosed()
{
}

void WebTileProxy::preferencesDidChange()
{
}

void WebTileProxy::toolTipChanged(const String&, const String&)
{
    notImplemented();
}

void WebTileProxy::didCommitLoadForMainFrame(const String& mimeType, bool useCustomContentProvider)
{
    notImplemented();
}

#if USE(COORDINATED_GRAPHICS_MULTIPROCESS)
void WebTileProxy::pageDidRequestScroll(const WebCore::IntPoint&)
{
    notImplemented();
}

void WebTileProxy::didRenderFrame(const WebCore::IntSize& contentsSize, const WebCore::IntRect& coveredRect)
{
    notImplemented();
}

void WebTileProxy::pageTransitionViewportReady()
{
    notImplemented();
}

void WebTileProxy::didFindZoomableArea(const WebCore::IntPoint&, const WebCore::IntRect&)
{
    notImplemented();
}
#endif

void WebTileProxy::handleDownloadRequest(DownloadProxy*)
{
    notImplemented();
}

void WebTileProxy::didChangeContentSize(const WebCore::IntSize&)
{
    notImplemented();
}

#if ENABLE(MANX_CURSOR_NAVIGATION)
void WebTileProxy::setCursorPosition(const IntPoint& cursorPosition)
{
    notImplemented();
}
#endif

#if PLATFORM(MANX)
void WebTileProxy::didUpdateBackingStoreState()
{
    notImplemented();
}

void WebTileProxy::didCommitCoordinatedGraphicsState()
{
    notImplemented();
}
#endif

void WebTileProxy::setCursor(const Cursor&)
{
    notImplemented();
}

void WebTileProxy::setCursorHiddenUntilMouseMoves(bool)
{
    notImplemented();
}

void WebTileProxy::didChangeViewportProperties(const ViewportAttributes&)
{
    notImplemented();
}

void WebTileProxy::registerEditCommand(PassRefPtr<WebEditCommandProxy>, WebPageProxy::UndoOrRedo)
{
    notImplemented();
}

void WebTileProxy::clearAllEditCommands()
{
    notImplemented();
}

bool WebTileProxy::canUndoRedo(WebPageProxy::UndoOrRedo)
{
    notImplemented();
    return false;
}

void WebTileProxy::executeUndoRedo(WebPageProxy::UndoOrRedo)
{
    notImplemented();
}

FloatRect WebTileProxy::convertToDeviceSpace(const FloatRect& viewRect)
{
    notImplemented();
    return viewRect;
}

FloatRect WebTileProxy::convertToUserSpace(const FloatRect& viewRect)
{
    notImplemented();
    return viewRect;
}

IntPoint WebTileProxy::screenToRootView(const IntPoint& point)
{
    notImplemented();
    return point;
}

IntRect WebTileProxy::rootViewToScreen(const IntRect& rect)
{
    notImplemented();
    return rect;
}

void WebTileProxy::doneWithKeyEvent(const NativeWebKeyboardEvent&, bool)
{
    notImplemented();
}

#if ENABLE(TOUCH_EVENTS)
void WebTileProxy::doneWithTouchEvent(const NativeWebTouchEvent&, bool wasEventHandled)
{
    notImplemented();
}
#endif

#if PLATFORM(MANX)
void WebTileProxy::doneWithMouseDownEvent(bool wasEventHandled)
{
}

void WebTileProxy::doneWithMouseUpEvent(bool wasEventHandled)
{
}
#endif

PassRefPtr<WebPopupMenuProxy> WebTileProxy::createPopupMenuProxy(WebPageProxy*)
{
    notImplemented();
    return 0;
}

PassRefPtr<WebContextMenuProxy> WebTileProxy::createContextMenuProxy(WebPageProxy*)
{
    notImplemented();
    return 0;
}

void WebTileProxy::setTextIndicator(Ref<WebCore::TextIndicator>, WebCore::TextIndicatorLifetime)
{
    notImplemented();
}

void WebTileProxy::clearTextIndicator(WebCore::TextIndicatorDismissalAnimation)
{
    notImplemented();
}

void WebTileProxy::setTextIndicatorAnimationProgress(float)
{
    notImplemented();
}

void WebTileProxy::enterAcceleratedCompositingMode(const LayerTreeContext&)
{
    notImplemented();
}

void WebTileProxy::exitAcceleratedCompositingMode()
{
    notImplemented();
}

void WebTileProxy::updateAcceleratedCompositingMode(const LayerTreeContext&)
{
    notImplemented();
}

#if ENABLE(FULLSCREEN_API)
WebFullScreenManagerProxyClient& WebTileProxy::fullScreenManagerProxyClient()
{
    notImplemented();
    return *this;
}

void WebTileProxy::closeFullScreenManager()
{
    notImplemented();
}

bool WebTileProxy::isFullScreen()
{
    notImplemented();
    return false;
}

void WebTileProxy::enterFullScreen()
{
    notImplemented();
}

void WebTileProxy::exitFullScreen()
{
    notImplemented();
}

void WebTileProxy::beganEnterFullScreen(const WebCore::IntRect& initialFrame, const WebCore::IntRect& finalFrame)
{
    notImplemented();
}

void WebTileProxy::beganExitFullScreen(const WebCore::IntRect& initialFrame, const WebCore::IntRect& finalFrame)
{
    notImplemented();
}
#endif

void WebTileProxy::didFinishLoadingDataForCustomContentProvider(const String& suggestedFilename, const IPC::DataReference&)
{
    notImplemented();
}

void WebTileProxy::navigationGestureDidBegin()
{
    notImplemented();
}

void WebTileProxy::navigationGestureWillEnd(bool willNavigate, WebBackForwardListItem&)
{
    notImplemented();
}

void WebTileProxy::navigationGestureDidEnd(bool willNavigate, WebBackForwardListItem&)
{
    notImplemented();
}

void WebTileProxy::navigationGestureDidEnd()
{
    notImplemented();
}

void WebTileProxy::willRecordNavigationSnapshot(WebBackForwardListItem&)
{
    notImplemented();
}

void WebTileProxy::didFirstVisuallyNonEmptyLayoutForMainFrame()
{
    notImplemented();
}

void WebTileProxy::didFinishLoadForMainFrame()
{
    notImplemented();
}

void WebTileProxy::didFailLoadForMainFrame()
{
    notImplemented();
}

void WebTileProxy::didSameDocumentNavigationForMainFrame(SameDocumentNavigationType)
{
    notImplemented();
}

void WebTileProxy::didChangeBackgroundColor()
{
    notImplemented();
}

#if ENABLE(VIDEO)
void WebTileProxy::mediaDocumentNaturalSizeChanged(const WebCore::IntSize&)
{
    notImplemented();
}
#endif

#if PLATFORM(MANX)
void WebTileProxy::didChangeEditorState(const WebKit::EditorState&)
{
    notImplemented();
}

void WebTileProxy::didChangeCompositionState(const WebCore::IntRect& compositionRect)
{
    notImplemented();
}

void WebTileProxy::didChangeSelectionState(const String& selectedText, const WebCore::IntRect& selectedRect)
{
    notImplemented();
}

void WebTileProxy::handleCertificateVerificationRequest(WebFrameProxy*, uint32_t error, const String &url, const Vector<CString> &certificates, PassRefPtr<Messages::WebPageProxy::CertificateVerificationRequest::DelayedReply> reply)
{
    // Always deny URLs with invalid or untrusted certificates
    reply->send(false);

    // We must notify the tile of the error ourselves, as the response above will just discard the network request silently
    // Also, we start SSL error codes at 1000 to avoid overwriting libCurl's own error codes (both start at 1)
    tileManager()->dispatchLoadFailed(tileID(), 1000 + error);
}

#if HAVE(ACCESSIBILITY)
void WebTileProxy::handleAccessibilityNotification(WebAccessibilityObject* axObject, WebCore::AXObjectCache::AXNotification notification)
{
    notImplemented();
}

void WebTileProxy::handleAccessibilityTextChange(WebAccessibilityObject* axObject, WebCore::AXTextChange textChange, uint32_t offset, const String& text)
{
    notImplemented();
}

void WebTileProxy::handleAccessibilityLoadingEvent(WebAccessibilityObject* axObject, WebCore::AXObjectCache::AXLoadingEvent loadingEvent)
{
    notImplemented();
}

void WebTileProxy::handleAccessibilityRootObject(WebAccessibilityObject*)
{
    notImplemented();
}

void WebTileProxy::handleAccessibilityFocusedObject(WebAccessibilityObject*)
{
    notImplemented();
}

void WebTileProxy::handleAccessibilityHitTest(WebAccessibilityObject*)
{
    notImplemented();
}
#endif // HAVE(ACCESSIBILITY)
#endif

void WebTileProxy::refView()
{
}

void WebTileProxy::derefView()
{
}

// WKPageLoaderClient Callbacks
void WebTileProxy::didFinishLoadForFrame(WKPageRef page, WKFrameRef frame, WKTypeRef userData, const void *clientInfo)
{
    const WebTileProxy* thiz = static_cast<const WebTileProxy*>(clientInfo);
    ASSERT(thiz->page() == toImpl(page));

    if (toImpl(frame)->isMainFrame()) {
        thiz->tileManager()->dispatchLoadCompleted(thiz->tileID());
        // enable paint updates
        if (!thiz->tileManager()->isPaused())
            const_cast<WebTileProxy*>(thiz)->setVisible(true);
        const_cast<WebTileProxy*>(thiz)->m_isLoaded = true;
    }
}

void WebTileProxy::didFailProvisionalLoadWithErrorForFrame(WKPageRef page, WKFrameRef frame, WKErrorRef error, WKTypeRef userData, const void *clientInfo)
{
    const WebTileProxy* thiz = static_cast<const WebTileProxy*>(clientInfo);
    ASSERT(thiz->page() == toImpl(page));

    // Note: errorCode() is a CurlCode, see curl.h for the list of values
    if (toImpl(frame)->isMainFrame())
        thiz->tileManager()->dispatchLoadFailed(thiz->tileID(), toImpl(error)->errorCode());
}

// WKPagePolicyClient Callbacks
void WebTileProxy::decidePolicyForNavigationAction(WKPageRef page, WKFrameRef frame, WKFrameNavigationType navigationType, WKEventModifiers modifiers, WKEventMouseButton mouseButton, WKFrameRef originatingFrame, WKURLRequestRef request, WKFramePolicyListenerRef listener, WKTypeRef userData, const void* clientInfo)
{
    const WebTileProxy* thiz = static_cast<const WebTileProxy*>(clientInfo);

    ASSERT(thiz->page() == toImpl(page));

    WebFramePolicyListenerProxy* policyListener = toImpl(listener);

    // We allow back/forward, reload, and form resubmitted by default, since they imply we allowed the initial load
    if (navigationType == kWKFrameNavigationTypeBackForward
        || navigationType == kWKFrameNavigationTypeReload
        || navigationType == kWKFrameNavigationTypeFormResubmitted) {
        policyListener->use();
        return;
    }

    const URL url = toImpl(request)->resourceRequest().url();

    WebFrameProxy* webframe = toImpl(frame);

    // Detect initial document load for the main frame and iframes
    // we don't forward such requests, just automatically allow them
    if (webframe->url().isEmpty()) {
        // printf("WebTileProxy: initial load for %s frame: %s\n", webframe->isMainFrame() ? "main" : "child", url.string().latin1().data());
        policyListener->use();
        return;
    }

    // We only allow location changes on the main frame
    if (webframe->isMainFrame()) {
        bool locationChangeAllowed = false;
        // The callback is also called whenever we call loadURL from setTileURL(),
        // we don't forward such requests, just automatically allow them
        if (url == thiz->tileURL()) {
            // printf("WebTileProxy: detect load from tile.loadURL: %s\n", url.string().latin1().data());
            locationChangeAllowed = true;
        } else {
            // printf("WebTileProxy: detect load from window.location: %s\n", url.string().latin1().data());

            // We allow fragment+query navigation within the page, e.g.:
            // http://www.foo.com/tile.html -> http://www.foo.com/tile.html#a
            // http://www.foo.com/tile.html#a -> http://www.foo.com/tile.html#b
            // http://www.foo.com/tile.html -> http://www.foo.com/tile.html?foo
            // http://www.foo.com/tile.html?foo -> http://www.foo.com/tile.html?foo&bar=1
            if (protocolHostAndPortAreEqual(url, thiz->tileURL()) && url.path() == thiz->tileURL().path())
                locationChangeAllowed = true;
            else {
                // To avoid a synchronous request which may stall the UI process, we fire this as an
                // asynchronous request instead, and consider the request denied.
                // If the JS onlocationchange handler allows the change, the tile will send a loadURL message as
                // if it would have been a simple url change on the tile side. This is effectively the same as
                // allowing the request and loading the new page, but without blocking on the UI side.
                thiz->tileManager()->dispatchLocationChangeRequested(thiz->tileID(), url);
            }
        }

        if (locationChangeAllowed) {
            // printf("URL change OK\n");
            policyListener->use();
            return;
        }
    } else {
        // printf("WebTileProxy: detect load from frame.location: %s\n", url.string().latin1().data());

        // We don't allow any modifications to iframes, except for fragments and query arguments
        const URL iframeURL(URL(), webframe->url());
        if (protocolHostAndPortAreEqual(url, iframeURL) && url.path() == iframeURL.path()) {
            // printf("URL change OK\n");
            policyListener->use();
            return;
        }
    }

    // printf("URL change DENIED\n");
    policyListener->ignore();
}

// WKPageUIClient Callbacks
unsigned long long WebTileProxy::exceededDatabaseQuota(WKPageRef, WKFrameRef, WKSecurityOriginRef, WKStringRef databaseName, WKStringRef displayName, unsigned long long currentQuota, unsigned long long currentOriginUsage, unsigned long long currentDatabaseUsage, unsigned long long expectedUsage, const void *clientInfo)
{
    // Allow 5MB of storage by default
    return 5 * 1024 * 1024;
}

} // namespace WebKit

#endif // ENABLE(MANX_HTMLTILE)
