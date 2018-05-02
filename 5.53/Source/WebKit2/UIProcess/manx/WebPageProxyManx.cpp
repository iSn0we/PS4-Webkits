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
#include "WebPageProxy.h"

#include "CoordinatedDrawingAreaProxy.h"
#include "CoordinatedLayerTreeHostProxy.h"
#include "NativeWebKeyboardEvent.h"
#include "NotImplemented.h"
#include "PageClient.h"
#include "WebAccessibilityObject.h"
#include "WebKitVersion.h"
#include "WebPageMessages.h"
#include "WebProcessProxy.h"
#include <manx/System.h>
#include <wtf/text/StringConcatenate.h>

using namespace WebCore;

namespace WebKit {

void WebPageProxy::platformInitialize()
{
}

static String osVersionForUAString()
{
    return String(Manx::System::osName());
}

static String userVisibleWebKitVersionString()
{
    return String::format("%d.%d", WEBKIT_MAJOR_VERSION, WEBKIT_MINOR_VERSION);
}

String WebPageProxy::standardUserAgent(const String& applicationNameForUserAgent)
{
    DEPRECATED_DEFINE_STATIC_LOCAL(String, osVersion, (osVersionForUAString()));
    DEPRECATED_DEFINE_STATIC_LOCAL(String, webKitVersion, (userVisibleWebKitVersionString()));
    DEPRECATED_DEFINE_STATIC_LOCAL(String, autoupdateVersion, (String(" (patch-01)")));

    return makeString("Mozilla/5.0 (", osVersion, ") AppleWebKit/", webKitVersion, " (KHTML, like Gecko)", applicationNameForUserAgent.isEmpty() ? "" : " ", applicationNameForUserAgent, autoupdateVersion);
}

void WebPageProxy::handleKeyboardEventManx(const NativeWebKeyboardEvent& event, int caretOffset)
{
    if (!isValid())
        return;

    m_keyEventQueue.append(event);

    process().responsivenessTimer()->start();
    if (m_keyEventQueue.size() == 1) // Otherwise, sent from DidReceiveEvent message handler.
        process().send(Messages::WebPage::KeyEventManx(event, caretOffset), m_pageID);
}

void WebPageProxy::setComposition(const String& text, Vector<CompositionUnderline> underlines, uint64_t selectionStart, uint64_t selectionEnd, uint64_t replacementRangeStart, uint64_t replacementRangeEnd)
{
    // FIXME: We need to find out how to proper handle the crashes case.
    if (!isValid())
        return;

    process().send(Messages::WebPage::SetComposition(text, underlines, selectionStart, selectionEnd, replacementRangeStart, replacementRangeEnd), m_pageID);
}

void WebPageProxy::confirmComposition(const String& compositionString, int64_t selectionStart, int64_t selectionLength)
{
    if (!isValid())
        return;

    process().send(Messages::WebPage::ConfirmComposition(compositionString, selectionStart, selectionLength), m_pageID);
}

void WebPageProxy::cancelComposition()
{
    if (!isValid())
        return;

    process().send(Messages::WebPage::CancelComposition(), m_pageID);
}

void WebPageProxy::exitComposition()
{
    if (!isValid())
        return;

    process().send(Messages::WebPage::ExitComposition(), m_pageID);
}

void WebPageProxy::setValueToFocusedNode(const String& value)
{
    if (!isValid())
        return;

    process().send(Messages::WebPage::SetValueToFocusedNode(value), m_pageID);
}

void WebPageProxy::clearSelectionWithoutBlur()
{
    if (!isValid())
        return;
    process().send(Messages::WebPage::ClearSelectionWithoutBlur(), m_pageID);
}

void WebPageProxy::setCaretVisible(bool visible)
{
    if (!isValid())
        return;
    process().send(Messages::WebPage::SetCaretVisible(visible), m_pageID);
}

void WebPageProxy::saveRecentSearches(const String&, const Vector<String>&)
{
    notImplemented();
}

void WebPageProxy::loadRecentSearches(const String&, Vector<String>&)
{
    notImplemented();
}

void WebPageProxy::didUpdateBackingStoreState()
{
    m_pageClient.didUpdateBackingStoreState();
}

void WebPageProxy::didCommitCoordinatedGraphicsState()
{
    m_pageClient.didCommitCoordinatedGraphicsState();
}

uint32_t WebPageProxy::acceleratedCompositingBackgroundColor()
{
#if USE(COORDINATED_GRAPHICS)
    CoordinatedDrawingAreaProxy* coordDrawingArea = static_cast<CoordinatedDrawingAreaProxy*>(drawingArea());
    if (!coordDrawingArea)
        return 0;
    CoordinatedGraphicsScene* scene = coordDrawingArea->coordinatedLayerTreeHostProxy().coordinatedGraphicsScene();
    if (!scene)
        return 0;

    WebCore::Color color = scene->viewBackgroundColor();
    if (color.isValid())
        return color.rgb(); // Alpha channel included.
#endif
    return 0;
}

void WebPageProxy::editorStateChanged(const EditorState& editorState)
{
    m_editorState = editorState;
    m_pageClient.didChangeEditorState(m_editorState);
}

void WebPageProxy::didFindZoomableArea(const IntPoint& target, const IntRect& area)
{
    m_pageClient.didFindZoomableArea(target, area);
}

void WebPageProxy::certificateVerificationRequest(uint64_t frameID, uint32_t error, const String& url, const Vector<CString>& certificates, PassRefPtr<Messages::WebPageProxy::CertificateVerificationRequest::DelayedReply> reply)
{
    WebFrameProxy* frame = m_process->webFrame(frameID);
    ASSERT(frame);

    m_process->responsivenessTimer()->stop();
    m_pageClient.handleCertificateVerificationRequest(frame, error, url, certificates, reply);
}

#if HAVE(ACCESSIBILITY)
void WebPageProxy::accessibilityNotification(const WebAccessibilityObject::Data& data, uint32_t notification)
{
    WebFrameProxy* frame = m_process->webFrame(data.webFrameID());
    if (!frame || frame->frameLoadState().state() != FrameLoadState::State::Finished)
        return;

    RefPtr<WebAccessibilityObject> webAXObject = WebAccessibilityObject::create(data, frame);
    m_pageClient.handleAccessibilityNotification(webAXObject.get(), static_cast<WebCore::AXObjectCache::AXNotification>(notification));
}

void WebPageProxy::accessibilityTextChange(const WebAccessibilityObject::Data& data, uint32_t textChange, uint32_t offset, const String& text)
{
    WebFrameProxy* frame = m_process->webFrame(data.webFrameID());
    if (!frame || frame->frameLoadState().state() != FrameLoadState::State::Finished)
        return;

    RefPtr<WebAccessibilityObject> webAXObject = WebAccessibilityObject::create(data, frame);
    m_pageClient.handleAccessibilityTextChange(webAXObject.get(), static_cast<WebCore::AXTextChange>(textChange), offset, text);
}

void WebPageProxy::accessibilityLoadingEvent(const WebAccessibilityObject::Data& data, uint32_t loadingEvent)
{
    WebFrameProxy* frame = m_process->webFrame(data.webFrameID());
    if (!frame)
        return;

    RefPtr<WebAccessibilityObject> webAXObject = WebAccessibilityObject::create(data, frame);
    m_pageClient.handleAccessibilityLoadingEvent(webAXObject.get(), static_cast<WebCore::AXObjectCache::AXLoadingEvent>(loadingEvent));
}

void WebPageProxy::accessibilityRootObject()
{
    if (!isValid())
        return;

    process().send(Messages::WebPage::AccessibilityRootObject(), m_pageID);
}

void WebPageProxy::didReceiveAccessibilityRootObject(const WebAccessibilityObject::Data& data)
{
    WebFrameProxy* frame = m_process->webFrame(data.webFrameID());
    if (!frame)
        return;

    RefPtr<WebAccessibilityObject> webAXFocusedObject = WebAccessibilityObject::create(data, frame);
    m_pageClient.handleAccessibilityRootObject(webAXFocusedObject.release().leakRef());
}

void WebPageProxy::accessibilityFocusedObject()
{
    if (!isValid())
        return;

    process().send(Messages::WebPage::AccessibilityFocusedObject(), m_pageID);
}

void WebPageProxy::didReceiveAccessibilityFocusedObject(const WebAccessibilityObject::Data& data)
{
    WebFrameProxy* frame = m_process->webFrame(data.webFrameID());
    if (!frame)
        return;

    RefPtr<WebAccessibilityObject> webAXFocusedObject = WebAccessibilityObject::create(data, frame);
    m_pageClient.handleAccessibilityFocusedObject(webAXFocusedObject.release().leakRef());
}

void WebPageProxy::accessibilityHitTest(const IntPoint& point)
{
    if (!isValid())
        return;

    process().send(Messages::WebPage::AccessibilityHitTest(point), m_pageID);
}

void WebPageProxy::didReceiveAccessibilityHitTest(const WebAccessibilityObject::Data& data)
{
    WebFrameProxy* frame = m_process->webFrame(data.webFrameID());
    if (!frame)
        return;

    RefPtr<WebAccessibilityObject> webAXFocusedObject = WebAccessibilityObject::create(data, frame);
    m_pageClient.handleAccessibilityHitTest(webAXFocusedObject.release().leakRef());
}
#endif // HAVE(ACCESSIBILITY) 

} // namespace WebKit
