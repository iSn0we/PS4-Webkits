/*
 * Copyright (C) 2010 Apple Inc. All rights reserved.
 * Copyright (C) 2012 Sony Interactive Entertainment Inc.
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
#include "ManxWebView.h"

#include "CoordinatedDrawingAreaProxy.h"
#include "CoordinatedGraphicsScene.h"
#include "DrawingAreaProxyImpl.h"
#include "NativeWebKeyboardEvent.h"
#include "NativeWebMouseEvent.h"
#include "NativeWebWheelEvent.h"
#include "TextureMapperGL.h"
#include "WebContextMenuProxy.h"
#include "WebPageCreationParameters.h"
#include "WebPageGroup.h"
#include "WebPopupMenuProxyManx.h"
#include "WebProcessPool.h"
#include <WebCore/Editor.h>
#include <WebCore/IntRect.h>
#include <WebCore/Region.h>
#include <cairo.h>
#include <manx/KeyboardCodes.h>

#if USE(COORDINATED_GRAPHICS_MULTIPROCESS)
#include "CoordinatedLayerTreeHostMessages.h"
#include "CoordinatedLayerTreeHostProxy.h"
#if OS(ORBIS)
#include <WebCore/OpenGLManx.h>
#endif
#endif


using namespace WebCore;

namespace WebKit {

WebView::WebView(WebProcessPool* context, WebPageGroup* pageGroup, const WKViewClientBase* client)
    : m_popupMenu(0)
    , m_focused(false)
    , m_active(false)
    , m_visible(false)
    , m_isFullScreen(false)
    , m_acceleratedCompositingEnabled(false)
    , m_viewportControllerClient(this)
{
    WebPageConfiguration webPageConfiguration;
    webPageConfiguration.pageGroup = pageGroup;

    m_client.initialize(client);
    m_page = context->createWebPage(*this, WTF::move(webPageConfiguration));
    m_acceleratedCompositingEnabled = m_page->pageGroup().preferences().acceleratedCompositingEnabled();
    m_page->pageGroup().preferences().setForceCompositingMode(m_acceleratedCompositingEnabled);

    m_viewportController = std::make_unique<PageViewportController>(m_page.get(), m_viewportControllerClient);
}

WebView::~WebView()
{
    m_page->close();
}

void WebView::initialize()
{
    m_page->initializeWebPage();
#if USE(COORDINATED_GRAPHICS)
    if (CoordinatedGraphicsScene* scene = coordinatedGraphicsScene())
        scene->setActive(true);
#endif
}

static void drawPageBackground(cairo_t* ctx, const WebPageProxy* page, const WebCore::IntRect& rect)
{
    if (!page->drawsBackground() || page->drawsTransparentBackground())
        return;
    cairo_set_source_rgba(ctx, 0.5, 0.5, 0.5, 1.0);
    cairo_rectangle(ctx, rect.x(), rect.y(), rect.width(), rect.height());
    cairo_set_operator(ctx, CAIRO_OPERATOR_OVER);
    cairo_fill(ctx);
}

void WebView::paint(unsigned char* surfaceData, const WebCore::IntSize& surfaceSize, const WebCore::IntRect& paintRect)
{
    if (!surfaceData || surfaceSize.isEmpty())
        return;

    m_page->endPrinting();

    const cairo_format_t format = CAIRO_FORMAT_ARGB32;
    cairo_surface_t* surface = cairo_image_surface_create_for_data(surfaceData, format, surfaceSize.width(), surfaceSize.height(), cairo_format_stride_for_width(format, surfaceSize.width()));
    if (cairo_surface_status(surface) != CAIRO_STATUS_SUCCESS)
        return;

    cairo_t* ctx = cairo_create(surface);

    if (CoordinatedDrawingAreaProxy* drawingArea = static_cast<CoordinatedDrawingAreaProxy*>(m_page->drawingArea())) {
        // FIXME: We should port WebKit1's rect coalescing logic here.
        Region unpaintedRegion;
        drawingArea->paint(ctx, paintRect, unpaintedRegion);

        Vector<IntRect> unpaintedRects = unpaintedRegion.rects();
        for (size_t i = 0; i < unpaintedRects.size(); ++i)
            drawPageBackground(ctx, m_page.get(), unpaintedRects[i]);
    } else
        drawPageBackground(ctx, m_page.get(), paintRect);

    cairo_destroy(ctx);
    cairo_surface_destroy(surface);
    m_page->didDraw();
}

#if ENABLE(MANX_INDIRECT_COMPOSITING)
WebCanvasHole WebView::canvasHole(uint32_t index) const
{
#if USE(COORDINATED_GRAPHICS)
    if (CoordinatedDrawingAreaProxy* drawingArea = static_cast<CoordinatedDrawingAreaProxy*>(m_page->drawingArea()))
        return drawingArea->canvasHole(index);
#endif

    return WebCanvasHole();
}
#endif


void WebView::scrollBy(int x, int y)
{
    if (useFixedLayout()) {
        m_viewportControllerClient.scrollPageBy(WebCore::FloatPoint(x, y));
        return;
    }
    
    if (x)
        m_page->scrollBy(x > 0 ? ScrollRight : ScrollLeft, ScrollByLine);
    if (y)
        m_page->scrollBy(y > 0 ? ScrollDown : ScrollUp, ScrollByLine);
}

void WebView::handleKeyboardEvent(const Manx::KeyboardEvent& event)
{
    NativeWebKeyboardEvent nativeEvent(event);

    if ((nativeEvent.type() == WebEvent::KeyDown) && ((nativeEvent.keyIdentifier() == "Accept") || (nativeEvent.keyIdentifier() == "Convert") || (nativeEvent.keyIdentifier() == "Cancel")))
        m_page->handleKeyboardEventManx(nativeEvent, event.m_caretOffset);

    else if ((nativeEvent.type() == WebEvent::RawKeyDown) && (nativeEvent.keyIdentifier() == "TabJump"))
        m_page->handleKeyboardEventManx(nativeEvent, event.m_caretOffset);

    else
        m_page->handleKeyboardEvent(nativeEvent);
}

void WebView::handleImeEvent(const Manx::ImeEvent& event)
{
    WTF::String compositionString = WTF::String::fromUTF8(event.m_compositionString);

    if (event.m_imeEventType == Manx::ImeEvent::ImeEventType::UpdatePreedit) {
        Vector<CompositionUnderline> underlines;
        underlines.append(CompositionUnderline(0, compositionString.length(), Color(Color::black), false));
        m_page->setComposition(compositionString, underlines, event.m_caretIndex, event.m_caretIndex, 0, 0);
        return;
    }

    if ((event.m_imeEventType == Manx::ImeEvent::ImeEventType::ConfirmPreedit)
        || (event.m_imeEventType == Manx::ImeEvent::ImeEventType::InsertText)) {
        m_page->confirmComposition(compositionString, event.m_caretIndex, 0);
        return;
    }

    if (event.m_imeEventType == Manx::ImeEvent::SetValueToFocusedNode)
        m_page->setValueToFocusedNode(compositionString);
}

void WebView::clearSelection()
{
    m_page->clearSelectionWithoutBlur();
}

void WebView::handleMouseEvent(const Manx::MouseEvent& mouseEvent)
{
    if (useFixedLayout()) {
        m_lastMousePosition = IntPoint(mouseEvent.m_x, mouseEvent.m_y);
        Manx::MouseEvent scaledMouseEvent = mouseEvent;
        IntPoint point = m_viewportControllerClient.userViewportToContents(IntPoint(mouseEvent.m_x, mouseEvent.m_y));
        scaledMouseEvent.m_x = point.x();
        scaledMouseEvent.m_y = point.y();
        m_page->handleMouseEvent(NativeWebMouseEvent(scaledMouseEvent));
        return;
    }
    m_page->handleMouseEvent(NativeWebMouseEvent(mouseEvent));
}

void WebView::handleWheelEvent(const Manx::WheelEvent& wheelEvent)
{
    if (useFixedLayout()) {
        Manx::WheelEvent scaledWheelEvent = wheelEvent;
        IntPoint point = m_viewportControllerClient.userViewportToContents(IntPoint(wheelEvent.m_x, wheelEvent.m_y));
        scaledWheelEvent.m_x = point.x();
        scaledWheelEvent.m_y = point.y();
        m_page->handleWheelEvent(NativeWebWheelEvent(scaledWheelEvent));
        return;
    }
    m_page->handleWheelEvent(NativeWebWheelEvent(wheelEvent));
}

void WebView::setSize(int width, int height)
{
    IntSize size(width, height);
    if (m_size == size)
        return;

    m_size = size;
    updateViewportSize();
}

void WebView::setFocused(bool isFocused)
{
    m_focused = isFocused;
    m_page->viewStateDidChange(ViewState::IsFocused);
}

void WebView::setActive(bool isActive)
{
    m_active = isActive;
    m_page->viewStateDidChange(ViewState::WindowIsActive);
}

void WebView::setIsVisible(bool isVisible)
{
    m_visible = isVisible;

    if (m_page)
        m_page->viewStateDidChange(ViewState::IsVisible);
}

void WebView::updateViewportSize()
{
    if (useFixedLayout()) {
        if (CoordinatedDrawingAreaProxy* drawingArea = static_cast<CoordinatedDrawingAreaProxy*>(page()->drawingArea()))
            drawingArea->setSize(roundedIntSize(dipSize()), IntSize(), IntSize());
        m_viewportController->didChangeViewportSize(dipSize());
        return;
    }
    if (CoordinatedDrawingAreaProxy* drawingArea = static_cast<CoordinatedDrawingAreaProxy*>(page()->drawingArea())) {
        // Web Process expects sizes in UI units, and not raw device units.
        drawingArea->setSize(roundedIntSize(dipSize()), IntSize(), IntSize());
        FloatRect visibleContentsRect(contentPosition(), visibleContentsSize());
        visibleContentsRect.intersect(FloatRect(FloatPoint(), contentsSize()));
        drawingArea->setVisibleContentsRect(visibleContentsRect, FloatPoint());
    }
}

inline WebCore::FloatSize WebView::dipSize() const
{
    FloatSize dipSize(size());
    dipSize.scale(1 / m_page->deviceScaleFactor());

    return dipSize;
}

bool WebView::useFixedLayout() const
{
    return m_page->isValid() && m_page->useFixedLayout();
}

PageViewportController& WebView::viewportController()
{
    return *m_viewportController;
}

void WebView::setPageScaleFactor(float newScale)
{
    if (useFixedLayout()) {
        m_viewportControllerClient.scalePage(newScale);
        return;
    }
    m_page->scalePageInViewCoordinates(newScale, IntPoint());
}

void WebView::postFakeMouseMove()
{
    if (useFixedLayout()) {
        Manx::MouseEvent fakeMouseEvent(Manx::MouseEvent::MouseMove, Manx::MouseEvent::NoButton, m_lastMousePosition.x(), m_lastMousePosition.y(), 0, false, false, false, false);
        handleMouseEvent(fakeMouseEvent);
    }
}

void WebView::didChangeContentsVisibility(const WebCore::IntSize& size, const WebCore::FloatPoint& position, float scale)
{
    m_client.didChangeContentsVisibility(this, size, position, scale);
}

WebCore::FloatSize WebView::visibleContentsSize() const
{
    FloatSize visibleContentsSize(dipSize());
    if (m_page->useFixedLayout())
        visibleContentsSize.scale(1 / contentScaleFactor());

    return visibleContentsSize;
}

void WebView::setViewBackgroundColor(WebCore::Color color)
{
    if (CoordinatedGraphicsScene* scene = coordinatedGraphicsScene())
        scene->setViewBackgroundColor(color);
}

WebCore::Color WebView::viewBackgroundColor()
{
    if (CoordinatedGraphicsScene* scene = coordinatedGraphicsScene())
        return scene->viewBackgroundColor();
    return WebCore::Color();
}

void WebView::paintToCurrentGLContext(const WebCore::FloatRect& viewport, WebCore::TextureMapper::PaintFlags paintFlags)
{
#if USE(COORDINATED_GRAPHICS)
    CoordinatedGraphicsScene* scene = coordinatedGraphicsScene();
    if (!scene)
        return;

    TransformationMatrix matrix;
    matrix.makeIdentity();
    matrix.translate(viewport.x(), viewport.y());
    WebCore::FloatPoint contentPosition(0, 0);

    if (useFixedLayout()) {
        m_viewportControllerClient.applyTransformation(matrix);
        contentPosition = m_viewportControllerClient.userViewportToContents(contentPosition);
    }

    Color backgroundColor = m_page->drawsTransparentBackground() ? Color::transparent : Color::white;
    scene->paintToCurrentGLContext(matrix, 1, viewport, backgroundColor, m_page->drawsBackground(), contentPosition, paintFlags);
#endif
}

void WebView::setACMemoryInfo(const WebCore::GLMemoryInfo& glMemoryInfo)
{
#if USE(COORDINATED_GRAPHICS_MULTIPROCESS) && OS(ORBIS)
    if (m_glMemoryInfo != glMemoryInfo) {
        m_glMemoryInfo = glMemoryInfo;
        m_page->process().send(Messages::CoordinatedLayerTreeHost::UpdateGLMemoryInfo(glMemoryInfo), m_page->pageID());
    }
#endif
}

void WebView::clearBackingStores()
{
#if ENABLE(MANX_ADVANCED_ACCELERATED_COMPOSITING)
    m_page->process().send(Messages::CoordinatedLayerTreeHost::ClearBackingStores(), m_page->pageID());
#endif
}

void WebView::restoreBackingStores()
{
#if ENABLE(MANX_ADVANCED_ACCELERATED_COMPOSITING)
    m_page->process().send(Messages::CoordinatedLayerTreeHost::RestoreBackingStores(), m_page->pageID());
#endif
}

void WebView::syncCoordinatedGraphicsState()
{
#if ENABLE(MANX_ADVANCED_ACCELERATED_COMPOSITING)
    m_page->process().send(Messages::CoordinatedLayerTreeHost::SyncFrame(), m_page->pageID());
#endif
}


#if USE(COORDINATED_GRAPHICS)
CoordinatedGraphicsScene* WebView::coordinatedGraphicsScene()
{
    if (!m_acceleratedCompositingEnabled)
        return nullptr;
    CoordinatedDrawingAreaProxy* coordDrawingArea = static_cast<CoordinatedDrawingAreaProxy*>(m_page->drawingArea());
    if (!coordDrawingArea)
        return nullptr;
    return coordDrawingArea->coordinatedLayerTreeHostProxy().coordinatedGraphicsScene();
}
#endif

// PageClient
std::unique_ptr<DrawingAreaProxy> WebView::createDrawingAreaProxy()
{
    return std::make_unique<CoordinatedDrawingAreaProxy>(*page());
}

void WebView::setViewNeedsDisplay(const WebCore::IntRect& rect)
{
    m_client.setViewNeedsDisplay(this, rect);
}

void WebView::displayView()
{
    notImplemented();
}

bool WebView::canScrollView()
{
    return false;
}

void WebView::scrollView(const WebCore::IntRect& scrollRect, const WebCore::IntSize&)
{
    setViewNeedsDisplay(scrollRect);
}

void WebView::requestScroll(const WebCore::FloatPoint& scrollPosition, const WebCore::IntPoint& scrollOrigin, bool isProgrammaticScroll)
{
}

WebCore::IntSize WebView::viewSize()
{
    return roundedIntSize(dipSize());
}

bool WebView::isViewWindowActive()
{
    return isActive(); 
}

bool WebView::isViewFocused()
{
    return isFocused();
}

bool WebView::isViewVisible()
{
    return isVisible();
}

bool WebView::isViewInWindow()
{
    notImplemented();
    return true;
}

void WebView::processDidExit()
{
    notImplemented();
}

void WebView::didRelaunchProcess()
{
#if USE(COORDINATED_GRAPHICS)
    if (CoordinatedGraphicsScene* scene = coordinatedGraphicsScene()) {
        scene->purgeGLResources();
        scene->setActive(true);
    }
#endif
}

void WebView::pageClosed()
{
    notImplemented();
}

void WebView::preferencesDidChange()
{
    notImplemented();
}

void WebView::toolTipChanged(const String&, const String&)
{
    notImplemented();
}

void WebView::didCommitLoadForMainFrame(const String& mimeType, bool useCustomContentProvider)
{
    setContentPosition(WebCore::FloatPoint());
    m_contentsSize = IntSize();

    if (useFixedLayout())
        m_viewportController->didCommitLoad();
}

void WebView::handleDownloadRequest(DownloadProxy*)
{
}

#if USE(COORDINATED_GRAPHICS_MULTIPROCESS)
void WebView::pageDidRequestScroll(const WebCore::IntPoint& cssPosition)
{
    FloatPoint uiPosition(cssPosition);
    setContentPosition(uiPosition);

    if (useFixedLayout())
        m_viewportController->pageDidRequestScroll(cssPosition);
}

void WebView::didRenderFrame(const WebCore::IntSize& contentsSize, const WebCore::IntRect& coveredRect)
{
    if (useFixedLayout())
        m_viewportController->didRenderFrame(contentsSize, coveredRect);
}

void WebView::pageTransitionViewportReady()
{
    if (useFixedLayout())
        m_viewportController->pageTransitionViewportReady();
}

void WebView::didFindZoomableArea(const WebCore::IntPoint&, const WebCore::IntRect&)
{
}
#endif

void WebView::didChangeContentSize(const WebCore::IntSize& size)
{
    if (m_contentsSize == size)
        return;

    m_contentsSize = size;
    m_client.didChangeContentsSize(this, size);

    if (useFixedLayout())
        m_viewportController->didChangeContentsSize(size);

    updateViewportSize();
}

#if ENABLE(MANX_CURSOR_NAVIGATION)
void WebView::setCursorPosition(const WebCore::IntPoint& cursorPosition)
{
    if (useFixedLayout()) {
        IntPoint clientPosition = m_viewportControllerClient.contentsToUserViewport(cursorPosition);
        m_client.setCursorPosition(this, clientPosition);
        return;
    }

    m_client.setCursorPosition(this, cursorPosition);
}
#endif

#if ENABLE(MANX_ADVANCED_ACCELERATED_COMPOSITING)
void WebView::didUpdateBackingStoreState()
{
    m_client.didUpdateBackingStoreState(this);
}

void WebView::didCommitCoordinatedGraphicsState()
{
    m_client.didCommitCoordinatedGraphicsState(this);
}
#endif

void WebView::setCursor(const Cursor& cursor)
{
    m_client.setCursor(this, cursor);
}

void WebView::setCursorHiddenUntilMouseMoves(bool)
{
    notImplemented();
}

void WebView::didChangeViewportProperties(const WebCore::ViewportAttributes& attributes)
{
    if (useFixedLayout())
        m_viewportController->didChangeViewportAttributes(attributes);
}

void WebView::registerEditCommand(PassRefPtr<WebEditCommandProxy>, WebPageProxy::UndoOrRedo)
{
    notImplemented();
}

void WebView::clearAllEditCommands()
{
    notImplemented();
}

bool WebView::canUndoRedo(WebPageProxy::UndoOrRedo)
{
    notImplemented();
    return false;
}

void WebView::executeUndoRedo(WebPageProxy::UndoOrRedo)
{
    notImplemented();
}

FloatRect WebView::convertToDeviceSpace(const FloatRect& viewRect)
{
    notImplemented();
    return viewRect;
}

FloatRect WebView::convertToUserSpace(const FloatRect& viewRect)
{
    notImplemented();
    return viewRect;
}

IntPoint WebView::screenToRootView(const IntPoint& point)
{
    notImplemented();
    return point;
}

IntRect WebView::rootViewToScreen(const IntRect& rect)
{
    notImplemented();
    return rect;
}

void WebView::didChangeEditorState(const WebKit::EditorState& editorState)
{
    WebKit::EditorState clientEditorState = editorState;
    Manx::Ime::EditorState::EventType eventType = Manx::Ime::EditorState::None;

    if (useFixedLayout()) {
        clientEditorState.fieldRect = m_viewportControllerClient.contentsToUserViewport(editorState.fieldRect);
        clientEditorState.compositionRect = m_viewportControllerClient.contentsToUserViewport(editorState.compositionRect);
        clientEditorState.selectedRect = m_viewportControllerClient.contentsToUserViewport(editorState.selectedRect);
    }

    if ((m_editorState.isContentEditable != clientEditorState.isContentEditable) && !clientEditorState.shouldIgnoreCompositionSelectionChange) {
        if (clientEditorState.isContentEditable)
            eventType = Manx::Ime::EditorState::ShouldImeOpen;
        else
            eventType = Manx::Ime::EditorState::ShouldImeClose;
    } else if (m_editorState.isContentEditable && clientEditorState.isContentEditable && !clientEditorState.shouldIgnoreCompositionSelectionChange) {
        bool isOnlyMovedOrResized = !clientEditorState.isFocusMoved
            && ((m_editorState.fieldLang == clientEditorState.fieldLang)
            && (m_editorState.fieldRect != clientEditorState.fieldRect)
            && (m_editorState.fieldType == clientEditorState.fieldType)
            && (m_editorState.hasPreviousNode == clientEditorState.hasPreviousNode)
            && (m_editorState.hasNextNode == clientEditorState.hasNextNode));

        if (isOnlyMovedOrResized)
            eventType = Manx::Ime::EditorState::ShouldImeUpdatePositionRect;
        else if ((m_editorState.fieldLang != clientEditorState.fieldLang)
            || (m_editorState.fieldRect != clientEditorState.fieldRect)
            || (m_editorState.fieldType != clientEditorState.fieldType)
            || (m_editorState.hasPreviousNode != clientEditorState.hasPreviousNode)
            || (m_editorState.hasNextNode != clientEditorState.hasNextNode))
            eventType = Manx::Ime::EditorState::ShouldImeReopen;
    }

    if (eventType != Manx::Ime::EditorState::None)
        m_client.didChangeEditorState(this, eventType, clientEditorState);

    if ((clientEditorState.caretOffset != -1)
        && ((clientEditorState.fieldText != m_editorState.fieldText) || (clientEditorState.caretOffset != m_editorState.caretOffset)))
        m_client.didChangeEditorState(this, Manx::Ime::EditorState::ShouldImeUpdateContext, clientEditorState);

    if (!clientEditorState.selectionIsRange && clientEditorState.hasComposition)
        didChangeCompositionState(clientEditorState.compositionRect);

    if (clientEditorState.selectionIsRange && !clientEditorState.shouldIgnoreCompositionSelectionChange)
        didChangeSelectionState(clientEditorState.selectedText, clientEditorState.selectedRect);
    else if (m_editorState.selectionIsRange && !m_editorState.shouldIgnoreCompositionSelectionChange && !clientEditorState.selectionIsRange)
        didChangeSelectionState("", WebCore::IntRect());

    m_editorState = clientEditorState;
}

void WebView::didChangeCompositionState(const WebCore::IntRect& compositionRect)
{
    if (useFixedLayout()) {
        WebCore::IntRect clientCompositionRect = m_viewportControllerClient.contentsToUserViewport(compositionRect);
        m_client.didChangeCompositionState(this, clientCompositionRect);
        return;
    }
    m_client.didChangeCompositionState(this, compositionRect);
}

void WebView::didChangeSelectionState(const String& selectedText, const WebCore::IntRect& selectedRect)
{
    if (useFixedLayout()) {
        WebCore::IntRect clientSelectedRect = m_viewportControllerClient.contentsToUserViewport(selectedRect);
        m_client.didChangeSelectionState(this, selectedText, clientSelectedRect);
        return;
    }
    m_client.didChangeSelectionState(this, selectedText, selectedRect);
}

void WebView::doneWithKeyEvent(const NativeWebKeyboardEvent& event, bool wasEventHandled)
{
    m_client.doneWithKeyEvent(this, event, wasEventHandled);
}

#if ENABLE(GESTURE_EVENTS)
void WebView::doneWithGestureEvent(const WebGestureEvent& event, bool wasEventHandled)
{
    notImplemented();
}
#endif

#if ENABLE(TOUCH_EVENTS)
void WebView::doneWithTouchEvent(const NativeWebTouchEvent& event, bool wasEventHandled)
{
    notImplemented();
}
#endif

void WebView::doneWithMouseDownEvent(bool wasEventHandled)
{
    m_client.doneWithMouseDownEvent(this, wasEventHandled);
}

void WebView::doneWithMouseUpEvent(bool wasEventHandled)
{
    m_client.doneWithMouseUpEvent(this, wasEventHandled);
}

PassRefPtr<WebPopupMenuProxy> WebView::createPopupMenuProxy(WebPageProxy* pageProxy)
{
    return WebPopupMenuProxyManx::create(this, pageProxy);
}

void WebView::setPopupMenuClient(const WKViewPopupMenuClientBase* client)
{
    m_popupMenuClient.initialize(client);
}

void WebView::showPopupMenu(WebPopupMenuProxyManx* popupMenu, const Vector<WebPopupItem>& items, const WebCore::IntRect& rect, int32_t selectedIndex)
{
    m_popupMenu = popupMenu;
    if (useFixedLayout()) {
        IntRect clientRect = m_viewportControllerClient.contentsToUserViewport(rect);
        m_popupMenuClient.showPopupMenu(this, items, clientRect, selectedIndex);
        return;
    }
    m_popupMenuClient.showPopupMenu(this, items, rect, selectedIndex);
    return;
}

void WebView::hidePopupMenu()
{
    m_popupMenuClient.hidePopupMenu(this);
    m_popupMenu = 0;
}

void WebView::valueChangedForPopupMenu(int selectedIndex)
{
    if (m_popupMenu)
        m_popupMenu->valueChangedForPopupMenu(selectedIndex);
}

PassRefPtr<WebContextMenuProxy> WebView::createContextMenuProxy(WebPageProxy*)
{
#if ENABLE(CONTEXT_MENUS)
    class WebContextMenuProxyManx : public WebContextMenuProxy {
    public:
        WebContextMenuProxyManx() { }
        virtual ~WebContextMenuProxyManx() { }

        virtual void showContextMenu(const WebCore::IntPoint&, const Vector<RefPtr<WebContextMenuItem>>&, const ContextMenuContextData&) { }
        virtual void hideContextMenu() { }
        virtual void cancelTracking() { }
    };
    return adoptRef(new WebContextMenuProxyManx());
#else
    return 0;
#endif // ENABLE(CONTEXT_MENUS)
}

void WebView::setTextIndicator(Ref<WebCore::TextIndicator>, WebCore::TextIndicatorLifetime)
{
    notImplemented();
}

void WebView::clearTextIndicator(WebCore::TextIndicatorDismissalAnimation)
{
    notImplemented();
}

void WebView::setTextIndicatorAnimationProgress(float)
{
    notImplemented();
}

void WebView::enterAcceleratedCompositingMode(const LayerTreeContext& layerTreeContext)
{
    m_client.enterAcceleratedCompositingMode(this);
}

void WebView::exitAcceleratedCompositingMode()
{
    m_client.exitAcceleratedCompositingMode(this);
}

void WebView::updateAcceleratedCompositingMode(const LayerTreeContext& layerTreeContext)
{
    notImplemented();
}

void WebView::didFinishLoadingDataForCustomContentProvider(const String&, const IPC::DataReference&)
{
    notImplemented();
}

void WebView::navigationGestureDidBegin()
{
}

void WebView::navigationGestureWillEnd(bool willNavigate, WebBackForwardListItem&)
{
}

void WebView::navigationGestureDidEnd(bool willNavigate, WebBackForwardListItem&)
{
}

void WebView::navigationGestureDidEnd()
{
}

void WebView::willRecordNavigationSnapshot(WebBackForwardListItem&)
{
}

void WebView::didFirstVisuallyNonEmptyLayoutForMainFrame()
{
}

void WebView::didFinishLoadForMainFrame()
{
}

void WebView::didChangeBackgroundColor()
{
}

void WebView::didFailLoadForMainFrame()
{
}

void WebView::didSameDocumentNavigationForMainFrame(SameDocumentNavigationType)
{
}

WebFullScreenManagerProxyClient& WebView::fullScreenManagerProxyClient()
{
    return *this;
}

void WebView::closeFullScreenManager()
{
    if (isFullScreen())
        m_client.closeFullScreen(this);
    m_isFullScreen = false;
}

bool WebView::isFullScreen()
{
    return m_isFullScreen;
}

void WebView::enterFullScreen()
{
    if (!isFullScreen())
        m_client.enterFullScreen(this);
}

void WebView::exitFullScreen()
{
    if (isFullScreen())
        m_client.exitFullScreen(this);
}

void WebView::beganEnterFullScreen(const WebCore::IntRect& initialFrame, const WebCore::IntRect& finalFrame)
{
    if (useFixedLayout()) {
        WebCore::IntRect clientInitialFrame = m_viewportControllerClient.contentsToUserViewport(initialFrame);
        WebCore::IntRect clientFinalFrame = m_viewportControllerClient.contentsToUserViewport(finalFrame);
        m_client.beganEnterFullScreen(this, clientInitialFrame, clientFinalFrame);
        return;
    }
    m_client.beganEnterFullScreen(this, initialFrame, finalFrame);
}
void WebView::beganExitFullScreen(const WebCore::IntRect& initialFrame, const WebCore::IntRect& finalFrame)
{
    if (useFixedLayout()) {
        WebCore::IntRect clientInitialFrame = m_viewportControllerClient.contentsToUserViewport(initialFrame);
        WebCore::IntRect clientFinalFrame = m_viewportControllerClient.contentsToUserViewport(finalFrame);
        m_client.beganExitFullScreen(this, clientInitialFrame, clientFinalFrame);
        return;
    }
    m_client.beganExitFullScreen(this, initialFrame, finalFrame);
}

void WebView::willEnterFullScreen()
{
    m_isFullScreen = true;
    m_page->fullScreenManager()->willEnterFullScreen();
}

void WebView::didEnterFullScreen()
{
    m_page->fullScreenManager()->didEnterFullScreen();
}

void WebView::willExitFullScreen()
{
    m_page->fullScreenManager()->willExitFullScreen();
}

void WebView::didExitFullScreen()
{
    m_page->fullScreenManager()->didExitFullScreen();
    m_isFullScreen = false;
}

void WebView::requestExitFullScreen()
{
    if (isFullScreen())
        m_page->fullScreenManager()->requestExitFullScreen();
}

#if ENABLE(VIDEO)
void WebView::mediaDocumentNaturalSizeChanged(const WebCore::IntSize&)
{
}
#endif

void WebView::handleCertificateVerificationRequest(WebFrameProxy* frame, uint32_t error, const String& url, const Vector<CString>& certificates, PassRefPtr<Messages::WebPageProxy::CertificateVerificationRequest::DelayedReply> reply)
{
    m_certReply = reply;
    m_client.certificateVerificationRequest(this, error, url, certificates, frame);
}

void WebView::replyCertificateVerificationRequest(bool result)
{
    if (m_certReply) {
        m_certReply->send(result);
        m_certReply = nullptr;
    }
}

#if HAVE(ACCESSIBILITY)
void WebView::setAccessibilityClient(const WKViewAccessibilityClientBase* client)
{
    m_axClient.initialize(client);
}

void WebView::handleAccessibilityNotification(WebAccessibilityObject* axObject, WebCore::AXObjectCache::AXNotification notification)
{
    m_axClient.accessibilityNotification(this, axObject, notification);
}

void WebView::handleAccessibilityTextChange(WebAccessibilityObject* axObject, WebCore::AXTextChange textChange, uint32_t offset, const String& text)
{
    m_axClient.accessibilityTextChanged(this, axObject, textChange, offset, text);
}

void WebView::handleAccessibilityLoadingEvent(WebAccessibilityObject* axObject, WebCore::AXObjectCache::AXLoadingEvent loadingEvent)
{
    m_axClient.accessibilityLoadingEvent(this, axObject, loadingEvent);
}

void WebView::accessibilityRootObject()
{
    return m_page->accessibilityRootObject();
}

void WebView::handleAccessibilityRootObject(WebAccessibilityObject* axObject)
{
    m_axClient.handleAccessibilityRootObject(this, axObject);
}

void WebView::accessibilityFocusedObject()
{
    m_page->accessibilityFocusedObject();
}

void WebView::handleAccessibilityFocusedObject(WebAccessibilityObject* axObject)
{
    m_axClient.handleAccessibilityFocusedObject(this, axObject);
}

void WebView::accessibilityHitTest(const IntPoint& point)
{
    if (useFixedLayout())
        return m_page->accessibilityHitTest(m_viewportControllerClient.userViewportToContents(point));
    m_page->accessibilityHitTest(point);
}

void WebView::handleAccessibilityHitTest(WebAccessibilityObject* axObject)
{
    m_axClient.handleAccessibilityHitTest(this, axObject);
}
#endif

void WebView::refView()
{
}

void WebView::derefView()
{
}

}
