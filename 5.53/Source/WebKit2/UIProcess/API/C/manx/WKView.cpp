/*
 * Copyright (C) 2010 Apple Inc. All rights reserved.
 * Portions Copyright (c) 2010 Motorola Mobility, Inc.  All rights reserved.
 * Copyright (C) 2011 Igalia S.L.
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
#include "WKView.h"

#include "ManxWebView.h"
#include "WKAPICast.h"
#include "WebContextMenuItem.h"
#include "WebContextMenuItemData.h"
#include "WebPageGroup.h"
#include "WebProcessPool.h"
#include <WebCore/Color.h>
#include <WebCore/IntRect.h>

using namespace WebKit;
using namespace WebCore;

inline WKCanvasHole toAPI(const WebCore::CanvasHole& hole)
{
    WKCanvasHole wkCanvasHole;
    wkCanvasHole.quad[0][0] = hole.quad.p1().x();
    wkCanvasHole.quad[0][1] = hole.quad.p1().y();

    wkCanvasHole.quad[1][0] = hole.quad.p2().x();
    wkCanvasHole.quad[1][1] = hole.quad.p2().y();

    wkCanvasHole.quad[2][0] = hole.quad.p3().x();
    wkCanvasHole.quad[2][1] = hole.quad.p3().y();

    wkCanvasHole.quad[3][0] = hole.quad.p4().x();
    wkCanvasHole.quad[3][1] = hole.quad.p4().y();

    wkCanvasHole.opacity = hole.opacity;
    wkCanvasHole.canvasHandle = hole.canvasHandle;
    wkCanvasHole.flags = hole.flags;
    return wkCanvasHole;
}

inline WebCore::GLMemoryInfo toGLMemoryInfo(WKGLMemoryInfo info)
{
    WebCore::GLMemoryInfo glMemoryInfo;
    glMemoryInfo.heap = info.heap;
    glMemoryInfo.texture = info.texture;
    glMemoryInfo.surfaces = info.surfaces;
    glMemoryInfo.programs = info.programs;
    glMemoryInfo.buffers = info.buffers;
    glMemoryInfo.commandBuffers = info.commandBuffers;
    glMemoryInfo.total = info.total;
    glMemoryInfo.maxAllowed = info.maxAllowed;
    glMemoryInfo.fmemMappedSizeTotal = info.fmemMappedSizeTotal;
    glMemoryInfo.fmemMappedSizeLimit = info.fmemMappedSizeLimit;
    return glMemoryInfo;
}

WKViewRef WKViewCreate(WKContextRef contextRef, WKPageGroupRef pageGroupRef, const WKViewClientBase* client)
{
    RefPtr<WebView> view = WebView::create(toImpl(contextRef), toImpl(pageGroupRef), client);
    return toAPI(view.release().leakRef());
}

WKPageRef WKViewGetPage(WKViewRef viewRef)
{
    return toAPI(toImpl(viewRef)->page());
}

void WKViewPaint(WKViewRef view, unsigned char* surfaceData, WKSize surfaceSize, WKRect paintRect)
{
    toImpl(view)->paint(surfaceData, toIntSize(surfaceSize), toIntRect(paintRect));
}

void WKViewPaintToCurrentGLContext(WKViewRef view, WKRect clipRect, WKViewPaintFlags flags)
{
    TextureMapper::PaintFlags paintFlags = 0;
    if (flags & kWKViewPaintFlagVerticalFlip)
        paintFlags |= TextureMapper::PaintingMirrored;
#if ENABLE(MANX_INDIRECT_COMPOSITING)
    if (flags & kWKViewPaintFlagIndirectCompositing)
        paintFlags |= TextureMapper::PaintingForIndirectCompositing;
#endif
    if (flags & kWKViewPaintFlagSyncOnly)
        paintFlags |= TextureMapper::PaintingSyncOnly;
    if (flags & kWKViewPaintFlagWithoutSync)
        paintFlags |= TextureMapper::PaintingWithoutSync;

    toImpl(view)->paintToCurrentGLContext(toFloatRect(clipRect), paintFlags);
}

void WKViewSetACMemoryInfo(WKViewRef view, WKGLMemoryInfo info)
{
    toImpl(view)->setACMemoryInfo(toGLMemoryInfo(info));
}

WKCanvasHole WKViewGetCanvasHole(WKViewRef view, uint32_t index)
{
#if ENABLE(MANX_INDIRECT_COMPOSITING)
    return toAPI(toImpl(view)->canvasHole(index).data());
#else
    WKCanvasHole hole = { 0 };
    hole.canvasHandle = sce::Canvas::kInvalidHandle;
    return hole;
#endif
}

void WKViewScrollBy(WKViewRef view, int x, int y)
{
    toImpl(view)->scrollBy(x, y);
}

void WKViewHandleImeEvent(WKViewRef view, const Manx::ImeEvent* event)
{
    toImpl(view)->handleImeEvent(*event);
}

void WKViewHandleKeyboardEvent(WKViewRef view, const Manx::KeyboardEvent* event)
{
    toImpl(view)->handleKeyboardEvent(*event);
}

void WKViewClearSelection(WKViewRef view)
{
    toImpl(view)->clearSelection();
}

void WKViewHandleMouseEvent(WKViewRef view, const Manx::MouseEvent* mouseEvent)
{
    toImpl(view)->handleMouseEvent(*mouseEvent);
}

void WKViewHandleWheelEvent(WKViewRef view, const Manx::WheelEvent* wheelEvent)
{
    toImpl(view)->handleWheelEvent(*wheelEvent);
}

void WKViewSetDrawsBackground(WKViewRef viewRef, bool drawsBackground)
{
    toImpl(viewRef)->page()->setDrawsBackground(drawsBackground);
}

void WKViewSetDrawsTransparentBackground(WKViewRef viewRef, bool drawsTransparentBackground)
{
    toImpl(viewRef)->page()->setDrawsTransparentBackground(drawsTransparentBackground);
}

void WKViewSetBackgroundColor(WKViewRef viewRef, int red, int green, int blue, int alpha)
{
    toImpl(viewRef)->setViewBackgroundColor(WebCore::Color(red, green, blue, alpha));
}

void WKViewSetSize(WKViewRef view, int width, int height)
{
    toImpl(view)->setSize(width, height);
}

void WKViewSetViewPopupMenuClient(WKViewRef view, const WKViewPopupMenuClientBase* client)
{
    toImpl(view)->setPopupMenuClient(client);
}

void WKViewValueChangedForPopupMenu(WKViewRef view, int selectedIndex)
{
    toImpl(view)->valueChangedForPopupMenu(selectedIndex);
}

void WKViewSetFocused(WKViewRef view, bool isFocused)
{
    toImpl(view)->setFocused(isFocused);
}

void WKViewSetActive(WKViewRef view, bool isActive)
{
    toImpl(view)->setActive(isActive);
}

void WKViewSetIsVisible(WKViewRef view, bool isVisible)
{
    toImpl(view)->setIsVisible(isVisible);
}

void WKViewReplyCertificateVerification(WKViewRef view, bool result)
{
    toImpl(view)->replyCertificateVerificationRequest(result);
}

void WKViewWillEnterFullScreen(WKViewRef view)
{
#if ENABLE(FULLSCREEN_API)
    toImpl(view)->willEnterFullScreen();
#endif

}

void WKViewDidEnterFullScreen(WKViewRef view)
{
#if ENABLE(FULLSCREEN_API)
    toImpl(view)->didEnterFullScreen();
#endif
}

void WKViewWillExitFullScreen(WKViewRef view)
{
#if ENABLE(FULLSCREEN_API)
    toImpl(view)->willExitFullScreen();
#endif
}

void WKViewDidExitFullScreen(WKViewRef view)
{
#if ENABLE(FULLSCREEN_API)
    toImpl(view)->didExitFullScreen();
#endif
}

void WKViewRequestExitFullScreen(WKViewRef view)
{
#if ENABLE(FULLSCREEN_API)
    toImpl(view)->requestExitFullScreen();
#endif

}

bool WKViewIsFullScreen(WKViewRef view)
{
#if ENABLE(FULLSCREEN_API)
    return toImpl(view)->isFullScreen();
#else
    return false;
#endif
}

// coordinated graphics api
void WKViewClearBackingStores(WKViewRef view)
{
    toImpl(view)->clearBackingStores();
}

void WKViewRestoreBackingStores(WKViewRef view)
{
    toImpl(view)->restoreBackingStores();
}

void WKViewSyncCoordinatedGraphicsState(WKViewRef view)
{
    toImpl(view)->syncCoordinatedGraphicsState();
}

// accessibility api
void WKViewSetViewAccessibilityClient(WKViewRef view, const WKViewAccessibilityClientBase* client)
{
#if HAVE(ACCESSIBILITY)
    toImpl(view)->setAccessibilityClient(client);
#endif
}

void WKViewAccessibilityRootObject(WKViewRef view)
{
#if HAVE(ACCESSIBILITY)
    toImpl(view)->accessibilityRootObject();
#else
    return 0;
#endif
}

void WKViewAccessibilityFocusedObject(WKViewRef view)
{
#if HAVE(ACCESSIBILITY)
    toImpl(view)->accessibilityFocusedObject();
#else
    return 0;
#endif
}

void WKViewAccessibilityHitTest(WKViewRef view, const WKPoint& point)
{
#if HAVE(ACCESSIBILITY)
    return toImpl(view)->accessibilityHitTest(toIntPoint(point));
#else
    return 0;
#endif
}

void WKViewSetPageScaleFactor(WKViewRef view, double scale)
{
    toImpl(view)->setPageScaleFactor((float)scale);
}
