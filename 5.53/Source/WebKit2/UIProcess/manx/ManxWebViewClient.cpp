/*
 * Copyright (C) 2013 Sony Interactive Entertainment Inc.
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
#include "ManxWebViewClient.h"

#include "ManxWebView.h"
#include "NativeWebKeyboardEvent.h"
#include "WKAPICast.h"
#include "WKArray.h"
#include "WKString.h"

using namespace WebCore;

namespace WebKit {

void WebViewClient::setViewNeedsDisplay(WebView* view, const IntRect& rect)
{
    if (!m_client.setViewNeedsDisplay)
        return;
    m_client.setViewNeedsDisplay(toAPI(view), toAPI(rect), m_client.base.clientInfo);
}

void WebViewClient::didChangeContentsSize(WebView*, const WebCore::IntSize&)
{
    // FIXME: The corresponding I/F should be added to WKViewClient.
}

void WebViewClient::didChangeContentsVisibility(WebView* view, const WebCore::IntSize& size, const WebCore::FloatPoint& position, float scale)
{
    if (!m_client.didChangeContentsVisibility)
        return;
    m_client.didChangeContentsVisibility(toAPI(view), toAPI(size), toAPI(roundedIntPoint(position)), scale, m_client.base.clientInfo);
}

void WebViewClient::enterAcceleratedCompositingMode(WebView* view)
{
    if (!m_client.enterAcceleratedCompositingMode)
        return;
    m_client.enterAcceleratedCompositingMode(toAPI(view), m_client.base.clientInfo);
}

void WebViewClient::exitAcceleratedCompositingMode(WebView* view)
{
    if (!m_client.exitAcceleratedCompositingMode)
        return;
    m_client.exitAcceleratedCompositingMode(toAPI(view), m_client.base.clientInfo);
}

void WebViewClient::enterFullScreen(WebView* view)
{
    if (!m_client.enterFullScreen)
        return;
    m_client.enterFullScreen(toAPI(view), m_client.base.clientInfo);
}

void WebViewClient::exitFullScreen(WebView* view)
{
    if (!m_client.exitFullScreen) {
        view->willExitFullScreen();
        return;
    }
    m_client.exitFullScreen(toAPI(view), m_client.base.clientInfo);
}

void WebViewClient::closeFullScreen(WebView* view)
{
    if (!m_client.closeFullScreen)
        return;
    m_client.closeFullScreen(toAPI(view), m_client.base.clientInfo);
}

void WebViewClient::beganEnterFullScreen(WebView* view, const IntRect& initialFrame, const IntRect& finalFrame)
{
    if (!m_client.beganEnterFullScreen) {
        view->didEnterFullScreen();
        return;
    }
    m_client.beganEnterFullScreen(toAPI(view), toAPI(initialFrame), toAPI(finalFrame), m_client.base.clientInfo);
}

void WebViewClient::beganExitFullScreen(WebView* view, const IntRect& initialFrame, const IntRect& finalFrame)
{
    if (!m_client.beganExitFullScreen) {
        view->didExitFullScreen();
        return;
    }
    m_client.beganExitFullScreen(toAPI(view), toAPI(initialFrame), toAPI(finalFrame), m_client.base.clientInfo);
}

void WebViewClient::setCursorPosition(WebView* view, const WebCore::IntPoint& cursorPosition)
{
    if (!m_client.setCursorPosition)
        return;
    m_client.setCursorPosition(toAPI(view), toAPI(cursorPosition), m_client.base.clientInfo);
}

void WebViewClient::setCursor(WebView* view, const WebCore::Cursor& cursor)
{
    if (!m_client.setCursor)
        return;
    m_client.setCursor(toAPI(view), convertCursorToWKCursorType(cursor), m_client.base.clientInfo);
}

#define DEFINE_CURSOR_MAP(name) \
    case WebCore::Cursor::name: \
        return kWKCursorType##name

WKCursorType WebViewClient::convertCursorToWKCursorType(const WebCore::Cursor& cursor)
{
    switch (cursor.type()) {
        DEFINE_CURSOR_MAP(Pointer);
        DEFINE_CURSOR_MAP(Cross);
        DEFINE_CURSOR_MAP(Hand);
        DEFINE_CURSOR_MAP(IBeam);
        DEFINE_CURSOR_MAP(Wait);
        DEFINE_CURSOR_MAP(Help);
        DEFINE_CURSOR_MAP(EastResize);
        DEFINE_CURSOR_MAP(NorthResize);
        DEFINE_CURSOR_MAP(NorthEastResize);
        DEFINE_CURSOR_MAP(NorthWestResize);
        DEFINE_CURSOR_MAP(SouthResize);
        DEFINE_CURSOR_MAP(SouthEastResize);
        DEFINE_CURSOR_MAP(SouthWestResize);
        DEFINE_CURSOR_MAP(WestResize);
        DEFINE_CURSOR_MAP(NorthSouthResize);
        DEFINE_CURSOR_MAP(EastWestResize);
        DEFINE_CURSOR_MAP(NorthEastSouthWestResize);
        DEFINE_CURSOR_MAP(NorthWestSouthEastResize);
        DEFINE_CURSOR_MAP(ColumnResize);
        DEFINE_CURSOR_MAP(RowResize);
        DEFINE_CURSOR_MAP(MiddlePanning);
        DEFINE_CURSOR_MAP(EastPanning);
        DEFINE_CURSOR_MAP(NorthPanning);
        DEFINE_CURSOR_MAP(NorthEastPanning);
        DEFINE_CURSOR_MAP(NorthWestPanning);
        DEFINE_CURSOR_MAP(SouthPanning);
        DEFINE_CURSOR_MAP(SouthEastPanning);
        DEFINE_CURSOR_MAP(SouthWestPanning);
        DEFINE_CURSOR_MAP(WestPanning);
        DEFINE_CURSOR_MAP(Move);
        DEFINE_CURSOR_MAP(VerticalText);
        DEFINE_CURSOR_MAP(Cell);
        DEFINE_CURSOR_MAP(ContextMenu);
        DEFINE_CURSOR_MAP(Alias);
        DEFINE_CURSOR_MAP(Progress);
        DEFINE_CURSOR_MAP(NoDrop);
        DEFINE_CURSOR_MAP(Copy);
        DEFINE_CURSOR_MAP(None);
        DEFINE_CURSOR_MAP(NotAllowed);
        DEFINE_CURSOR_MAP(ZoomIn);
        DEFINE_CURSOR_MAP(ZoomOut);
        DEFINE_CURSOR_MAP(Grab);
        DEFINE_CURSOR_MAP(Grabbing);
        DEFINE_CURSOR_MAP(Custom);
    }
    return kWKCursorTypePointer;
}

void WebViewClient::didChangeEditorState(WebView* view, int eventType, const WebKit::EditorState& editorState)
{
    if (!m_client.didChangeEditorState)
        return;

    CString psOSKAttr = editorState.psOSKAttr.utf8();

    Manx::Ime::EditorState imeEditorState;
    imeEditorState.m_eventType = (Manx::Ime::EditorState::EventType)eventType;
    imeEditorState.m_contentEditable = editorState.isContentEditable;
    imeEditorState.m_type = (Manx::InputFieldTypes)editorState.fieldType;
    imeEditorState.m_lang = (Manx::InputLanguages)editorState.fieldLang;
    imeEditorState.m_caretOffset = editorState.caretOffset;
    imeEditorState.m_hasPreviousNode = editorState.hasPreviousNode;
    imeEditorState.m_hasNextNode = editorState.hasNextNode;
    imeEditorState.m_psOSKAttr = psOSKAttr.data();
    imeEditorState.m_isInLoginForm = editorState.isInLoginForm;
    imeEditorState.m_canSubmitImplicitly = editorState.canSubmitImplicitly;
    imeEditorState.m_isFocusedByTabKey = editorState.isFocusedByTabKey;
    m_client.didChangeEditorState(toAPI(view), imeEditorState, toAPI(editorState.fieldRect), toAPI(editorState.fieldText.impl()), m_client.base.clientInfo);
}

void WebViewClient::didChangeCompositionState(WebView* view, const WebCore::IntRect& compositionRect)
{
    if (m_client.didChangeCompositionState)
        m_client.didChangeCompositionState(toAPI(view), toAPI(compositionRect), m_client.base.clientInfo);
}

void WebViewClient::didChangeSelectionState(WebView* view, const String& selectedText, const WebCore::IntRect& selectedRect)
{
    if (m_client.didChangeSelectionState)
        m_client.didChangeSelectionState(toAPI(view), toAPI(selectedRect), toAPI(selectedText.impl()), m_client.base.clientInfo);
}

void WebViewClient::didUpdateBackingStoreState(WebView* view)
{
    if (!m_client.didUpdateBackingStoreState)
        return;
    m_client.didUpdateBackingStoreState(toAPI(view), m_client.base.clientInfo);
}

void WebViewClient::didCommitCoordinatedGraphicsState(WebView* view)
{
    if (!m_client.didCommitCoordinatedGraphicsState)
        return;
    m_client.didCommitCoordinatedGraphicsState(toAPI(view), m_client.base.clientInfo);
}

void WebViewClient::certificateVerificationRequest(WebView* view, uint32_t error, const String& url, const Vector<CString>& certificates, WebFrameProxy* frame)
{
    if (!m_client.runCertificateVerificationRequest) {
        view->replyCertificateVerificationRequest(false);
        return;
    }

    Vector<RefPtr<API::Object> > certs;

    for (int i = 0; i < certificates.size(); i++)
        certs.append(API::String::create(certificates[i].data()));

    RefPtr<API::Array> certsArray;
    if (!certs.isEmpty())
        certsArray = API::Array::create(WTF::move(certs));

    m_client.runCertificateVerificationRequest(toAPI(view), toAPI(frame), error, toAPI(url.impl()), toAPI(certsArray.get()), m_client.base.clientInfo);
}

void WebViewClient::doneWithKeyEvent(WebView* view, const NativeWebKeyboardEvent& event, bool wasEventHandled)
{
    if (!m_client.doneWithKeyEvent)
        return;
    m_client.doneWithKeyEvent(toAPI(view), event.nativeEvent(), wasEventHandled, m_client.base.clientInfo);
}

void WebViewClient::doneWithMouseDownEvent(WebView* view, bool wasEventHandled)
{
    if (!m_client.doneWithMouseDownEvent)
        return;
    m_client.doneWithMouseDownEvent(toAPI(view), wasEventHandled, m_client.base.clientInfo);
}

void WebViewClient::doneWithMouseUpEvent(WebView* view, bool wasEventHandled)
{
    if (!m_client.doneWithMouseUpEvent)
        return;
    m_client.doneWithMouseUpEvent(toAPI(view), wasEventHandled, m_client.base.clientInfo);
}

} // namespace WebKit
