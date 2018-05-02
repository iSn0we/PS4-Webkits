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

#ifndef WKView_h
#define WKView_h

#include <WebKit/WKAXObject.h>
#include <WebKit/WKBase.h>
#include <WebKit/WKEvent.h>
#include <WebKit/WKGLMemoryInfo.h>
#include <WebKit/WKGeometry.h>
#include <WebKit/WKPopupMenuItem.h>
#include <manx/ImeEvent.h>
#include <manx/KeyboardEvent.h>
#include <manx/MouseEvent.h>
#include <manx/WheelEvent.h>

#ifdef __cplusplus
extern "C" {
#endif

enum {
    kWKCursorTypePointer = 0,
    kWKCursorTypeCross = 1,
    kWKCursorTypeHand = 2,
    kWKCursorTypeIBeam = 3,
    kWKCursorTypeWait = 4,
    kWKCursorTypeHelp = 5,
    kWKCursorTypeEastResize = 6,
    kWKCursorTypeNorthResize = 7,
    kWKCursorTypeNorthEastResize = 8,
    kWKCursorTypeNorthWestResize = 9,
    kWKCursorTypeSouthResize = 10,
    kWKCursorTypeSouthEastResize = 11,
    kWKCursorTypeSouthWestResize = 12,
    kWKCursorTypeWestResize = 13,
    kWKCursorTypeNorthSouthResize = 14,
    kWKCursorTypeEastWestResize = 15,
    kWKCursorTypeNorthEastSouthWestResize = 16,
    kWKCursorTypeNorthWestSouthEastResize = 17,
    kWKCursorTypeColumnResize = 18,
    kWKCursorTypeRowResize = 19,
    kWKCursorTypeMiddlePanning = 20,
    kWKCursorTypeEastPanning = 21,
    kWKCursorTypeNorthPanning = 22,
    kWKCursorTypeNorthEastPanning = 23,
    kWKCursorTypeNorthWestPanning = 24,
    kWKCursorTypeSouthPanning = 25,
    kWKCursorTypeSouthEastPanning = 26,
    kWKCursorTypeSouthWestPanning = 27,
    kWKCursorTypeWestPanning = 28,
    kWKCursorTypeMove = 29,
    kWKCursorTypeVerticalText = 30,
    kWKCursorTypeCell = 31,
    kWKCursorTypeContextMenu = 32,
    kWKCursorTypeAlias = 33,
    kWKCursorTypeProgress = 34,
    kWKCursorTypeNoDrop = 35,
    kWKCursorTypeCopy = 36,
    kWKCursorTypeNone = 37,
    kWKCursorTypeNotAllowed = 38,
    kWKCursorTypeZoomIn = 39,
    kWKCursorTypeZoomOut = 40,
    kWKCursorTypeGrab = 41,
    kWKCursorTypeGrabbing = 42,
    kWKCursorTypeCustom = 43
};
typedef uint32_t WKCursorType;

typedef void (*WKViewSetViewNeedsDisplayCallback)(WKViewRef view, WKRect rect, const void* clientInfo);
typedef void (*WKViewSetCursorPositionCallback)(WKViewRef view, WKPoint cursorPosition, const void* clientInfo);
typedef void (*WKViewSetCursorCallback)(WKViewRef view, WKCursorType cursorType, const void* clientInfo);

typedef void (*WKViewDoneWithKeyEventCallback)(WKViewRef view, const Manx::KeyboardEvent* event, bool wasEventHandled, const void* clientInfo);
typedef void (*WKViewDoneWithMouseEventCallback)(WKViewRef view, bool wasEventHandled, const void* clientInfo);

// fullscreen callback
typedef void (*WKViewEnterFullScreenCallback)(WKViewRef view, const void* clientInfo);
typedef void (*WKViewExitFullScreenCallback)(WKViewRef view, const void* clientInfo);
typedef void (*WKViewCloseFullScreenCallback)(WKViewRef view, const void* clientInfo);
typedef void (*WKViewBeganEnterFullScreenCallback)(WKViewRef view, const WKRect initialFrame, const WKRect finalFrame, const void* clientInfo);
typedef void (*WKViewBeganExitFullScreenCallback)(WKViewRef view, const WKRect initialFrame, const WKRect finalFrame, const void* clientInfo);

// IME callback
typedef void (*WKViewDidChangeEditorStateCallback)(WKViewRef view, Manx::Ime::EditorState editorState, WKRect inputRect, WKStringRef inputText, const void* clientInfo);
typedef void (*WKViewDidChangeCompositionStateCallback)(WKViewRef view, WKRect compositionRect, const void* clientInfo);
typedef void (*WKViewDidChangeSelectionStateCallback)(WKViewRef view, WKRect selectedRect, WKStringRef selectedText, const void* clientInfo);

// sync callback
typedef void (*WKViewRunCertificateVerificationRequestCallback)(WKViewRef view, WKFrameRef frame, int error, WKStringRef url, WKArrayRef certificates, const void* clientInfo);

// Accelerated Compositing callback
typedef void (*WKViewEnterAcceleratedCompositingModeCallback)(WKViewRef view, const void *clientInfo);
typedef void (*WKViewExitAcceleratedCompositingModeCallback)(WKViewRef view, const void *clientInfo);

// TiledBackingStore callback
typedef void (*WKViewDidUpdateBackingStoreStateCallback)(WKViewRef view, const void *clientInfo);
// CoordinatedGraphics callback
typedef void (*WKViewDidCommitCoordinatedGraphicsStateCallback)(WKViewRef view, const void *clientInfo);
typedef void (*WKViewPageDidChangeContentsVisibilityCallback)(WKViewRef view, WKSize size, WKPoint position, float scale, const void* clientInfo);

typedef struct WKViewClientBase {
    int                                                                 version;
    const void *                                                        clientInfo;
} WKViewClientBase;

typedef struct WKViewClientV0 {
    WKViewClientBase                                                    base;

    WKViewSetViewNeedsDisplayCallback                                   setViewNeedsDisplay;
    WKViewSetCursorPositionCallback                                     setCursorPosition;
    WKViewEnterFullScreenCallback                                       enterFullScreen;
    WKViewExitFullScreenCallback                                        exitFullScreen;
    WKViewCloseFullScreenCallback                                       closeFullScreen;
    WKViewBeganEnterFullScreenCallback                                  beganEnterFullScreen;
    WKViewBeganExitFullScreenCallback                                   beganExitFullScreen;
    WKViewRunCertificateVerificationRequestCallback                     runCertificateVerificationRequest;
    WKViewEnterAcceleratedCompositingModeCallback                       enterAcceleratedCompositingMode;
    WKViewExitAcceleratedCompositingModeCallback                        exitAcceleratedCompositingMode;
    WKViewSetCursorCallback                                             setCursor;
    WKViewDoneWithKeyEventCallback                                      doneWithKeyEvent;
    WKViewDoneWithMouseEventCallback                                    doneWithMouseDownEvent;
    WKViewDoneWithMouseEventCallback                                    doneWithMouseUpEvent;
    WKViewDidChangeEditorStateCallback                                  didChangeEditorState;
    WKViewDidChangeSelectionStateCallback                               didChangeSelectionState;
    WKViewDidChangeCompositionStateCallback                             didChangeCompositionState;
    WKViewDidUpdateBackingStoreStateCallback                            didUpdateBackingStoreState;
    WKViewDidCommitCoordinatedGraphicsStateCallback                     didCommitCoordinatedGraphicsState;
    WKViewPageDidChangeContentsVisibilityCallback                       didChangeContentsVisibility;
} WKViewClientV0;

typedef void (*WKViewShowPopupMenuCallback)(WKViewRef view, WKArrayRef popupMenuItem, WKRect rect, int32_t selectedIndex, const void* clientInfo);
typedef void (*WKViewHidePopupMenuCallback)(WKViewRef view, const void* clientInfo);

typedef struct WKViewPopupMenuClientBase {
    int                                                                 version;
    const void *                                                        clientInfo;
} WKViewPopupMenuClientBase;

typedef struct WKViewPopupMenuClientV0 {
    WKViewPopupMenuClientBase                                           base;
    WKViewShowPopupMenuCallback                                         showPopupMenu;
    WKViewHidePopupMenuCallback                                         hidePopupMenu;
} WKViewPopupMenuClientV0;

typedef void(*WKViewAccessibilityCallback)(WKViewRef view, WKAXObjectRef axObject, WKAXNotification notification, const void* clientInfo);
typedef void(*WKViewAccessibilityTextChangeCallback)(WKViewRef view, WKAXObjectRef axObject, WKAXTextChange textChange, uint32_t offset, WKStringRef text, const void* clientInfo);
typedef void(*WKViewAccessibilityLoadingEventCallback)(WKViewRef view, WKAXObjectRef axObject, WKAXLoadingEvent loadingEvent, const void* clientInfo);
typedef void(*WKViewAccessibilityRootObjectCallback)(WKViewRef view, WKAXObjectRef axObject, const void* clientInfo);
typedef void(*WKViewAccessibilityFocusedObjectCallback)(WKViewRef view, WKAXObjectRef axObject, const void* clientInfo);
typedef void(*WKViewAccessibilityHitTestCallback)(WKViewRef view, WKAXObjectRef axObject, const void* clientInfo);

typedef struct WKViewAccessibilityClientBase {
    int                                                                 version;
    const void *                                                        clientInfo;
} WKViewAccessibilityClientBase;

typedef struct WKViewAccessibilityClientV0 {
    WKViewAccessibilityClientBase                                       base;
    WKViewAccessibilityCallback                                         notification;
    WKViewAccessibilityTextChangeCallback                               textChanged;
    WKViewAccessibilityLoadingEventCallback                             loadingEvent;
    WKViewAccessibilityRootObjectCallback                               rootObject;
    WKViewAccessibilityFocusedObjectCallback                            focusedObject;
    WKViewAccessibilityHitTestCallback                                  hitTest;
} WKViewAccessibilityClientV0;

WK_EXPORT WKViewRef WKViewCreate(WKContextRef context, WKPageGroupRef pageGroup, const WKViewClientBase* client);

WK_EXPORT WKPageRef WKViewGetPage(WKViewRef view);

WK_EXPORT void WKViewPaint(WKViewRef view, unsigned char* surfaceData, WKSize surfaceSize, WKRect);
WK_EXPORT void WKViewScrollBy(WKViewRef view, int x, int y);
WK_EXPORT void WKViewHandleKeyboardEvent(WKViewRef view, const Manx::KeyboardEvent*);
WK_EXPORT void WKViewHandleImeEvent(WKViewRef view, const Manx::ImeEvent*);
WK_EXPORT void WKViewClearSelection(WKViewRef view);
WK_EXPORT void WKViewHandleMouseEvent(WKViewRef view, const Manx::MouseEvent*);
WK_EXPORT void WKViewHandleWheelEvent(WKViewRef view, const Manx::WheelEvent*);
WK_EXPORT void WKViewSetDrawsBackground(WKViewRef view, bool drawsBackground);
WK_EXPORT void WKViewSetDrawsTransparentBackground(WKViewRef view, bool drawsTransparentBackground);
WK_EXPORT void WKViewSetSize(WKViewRef view, int width, int height);
WK_EXPORT void WKViewSetViewPopupMenuClient(WKViewRef view, const WKViewPopupMenuClientBase* client);
WK_EXPORT void WKViewValueChangedForPopupMenu(WKViewRef view, int selectedIndex);
WK_EXPORT void WKViewSetFocused(WKViewRef view, bool isFocused);
WK_EXPORT void WKViewSetActive(WKViewRef view, bool isActive);
WK_EXPORT void WKViewSetIsVisible(WKViewRef view, bool isVisible);
WK_EXPORT void WKViewReplyCertificateVerification(WKViewRef view, bool result);

// fullscreen api
WK_EXPORT void WKViewWillEnterFullScreen(WKViewRef view);
WK_EXPORT void WKViewDidEnterFullScreen(WKViewRef view);
WK_EXPORT void WKViewWillExitFullScreen(WKViewRef view);
WK_EXPORT void WKViewDidExitFullScreen(WKViewRef view);
WK_EXPORT void WKViewRequestExitFullScreen(WKViewRef view);
WK_EXPORT bool WKViewIsFullScreen(WKViewRef view);

// coordinated graphics api
WK_EXPORT void WKViewClearBackingStores(WKViewRef view);
WK_EXPORT void WKViewRestoreBackingStores(WKViewRef view);
WK_EXPORT void WKViewSyncCoordinatedGraphicsState(WKViewRef view);
WK_EXPORT void WKViewSetPageScaleFactor(WKViewRef view, double scale);


WK_EXPORT void WKViewSetBackgroundColor(WKViewRef, int red, int green, int blue, int alpha);

enum {
    kWKViewPaintFlagVerticalFlip = 1 << 0,
    kWKViewPaintFlagIndirectCompositing = 1 << 1,
    kWKViewPaintFlagSyncOnly = 1 << 2,
    kWKViewPaintFlagWithoutSync = 1 << 3
};
typedef uint32_t WKViewPaintFlags;

WK_EXPORT void WKViewPaintToCurrentGLContext(WKViewRef, WKRect, WKViewPaintFlags);
WK_EXPORT void WKViewSetACMemoryInfo(WKViewRef, WKGLMemoryInfo);

// Indirect compositing API
struct WKCanvasHole {
    float quad[4][2];
    float opacity;
    uint32_t canvasHandle;
    uint32_t flags;
};
typedef struct WKCanvasHole WKCanvasHole;
// @sa WKViewPaintToCurrentGLContext
WK_EXPORT WKCanvasHole WKViewGetCanvasHole(WKViewRef view, uint32_t index);

// accessibility api
WK_EXPORT void WKViewSetViewAccessibilityClient(WKViewRef view, const WKViewAccessibilityClientBase* client);
WK_EXPORT void WKViewAccessibilityRootObject(WKViewRef view);
WK_EXPORT void WKViewAccessibilityFocusedObject(WKViewRef view);
WK_EXPORT void WKViewAccessibilityHitTest(WKViewRef view, const WKPoint& point);

#ifdef __cplusplus
}
#endif

#endif /* WKView_h */
