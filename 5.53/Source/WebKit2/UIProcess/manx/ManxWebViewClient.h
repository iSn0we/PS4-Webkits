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

#ifndef ManxWebViewClient_h
#define ManxWebViewClient_h

#include "APIClient.h"
#include "Cursor.h"
#include "EditorState.h"
#include <WKView.h>
#include <WebCore/IntSize.h>
#include <wtf/Forward.h>
#include <wtf/Vector.h>

namespace WebCore {
class FloatPoint;
class IntPoint;
class IntRect;
}

namespace API {
template<> struct ClientTraits<WKViewClientBase> {
    typedef std::tuple<WKViewClientV0> Versions;
};
}

namespace WebKit {

class NativeWebKeyboardEvent;
class WebFrameProxy;
class WebView;

class WebViewClient : public API::Client<WKViewClientBase> {
public:
    void setViewNeedsDisplay(WebView*, const WebCore::IntRect&);
    void didChangeContentsSize(WebView*, const WebCore::IntSize&);
    void didChangeContentsVisibility(WebView*, const WebCore::IntSize&, const WebCore::FloatPoint& position, float scale);

    void enterAcceleratedCompositingMode(WebView*);
    void exitAcceleratedCompositingMode(WebView*);

    void enterFullScreen(WebView*);
    void exitFullScreen(WebView*);
    void closeFullScreen(WebView*);
    void beganEnterFullScreen(WebView*, const WebCore::IntRect&, const WebCore::IntRect&);
    void beganExitFullScreen(WebView*, const WebCore::IntRect&, const WebCore::IntRect&);
    void didChangeEditorState(WebView*, int eventType, const WebKit::EditorState&);
    void didChangeCompositionState(WebView*, const WebCore::IntRect& compositonRect);
    void didChangeSelectionState(WebView*, const String& selectedText, const WebCore::IntRect& selectedRect);
    void setCursor(WebView*, const WebCore::Cursor&);
    void setCursorPosition(WebView*, const WebCore::IntPoint& cursorPosition);

    void didUpdateBackingStoreState(WebView*);
    void didCommitCoordinatedGraphicsState(WebView*);

    void certificateVerificationRequest(WebView*, uint32_t error, const String& url, const Vector<CString>& certificates, WebFrameProxy*);
    void doneWithKeyEvent(WebView*, const NativeWebKeyboardEvent&, bool wasEventHandled);
    void doneWithMouseDownEvent(WebView*, bool wasEventHandled);
    void doneWithMouseUpEvent(WebView*, bool wasEventHandled);

private:
    static WKCursorType convertCursorToWKCursorType(const WebCore::Cursor&);
};

} // namespace WebKit

#endif // ManxWebViewClient_h
