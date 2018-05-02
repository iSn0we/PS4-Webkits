/*
 * (C) 2016 Sony Interactive Entertainment Inc.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "AXObjectCache.h"

#if HAVE(ACCESSIBILITY)

#include "AccessibilityObject.h"
#include "RenderView.h"
#include "dom/Document.h"
#include "page/Chrome.h"
#include "page/ChromeClient.h"
#include "page/Frame.h"
#include "page/Page.h"

namespace WebCore {

void AXObjectCache::attachWrapper(AccessibilityObject* obj)
{
}

void AXObjectCache::detachWrapper(AccessibilityObject* obj, AccessibilityDetachmentType type)
{
}

void AXObjectCache::postPlatformNotification(AccessibilityObject* object, AXNotification notification)
{
    if (!object
        || object->document()->renderView()->layoutState()
        || object->document()->childNeedsStyleRecalc())
        return;
    ChromeClient& client = m_document.frame()->page()->chrome().client();
    client.postAccessibilityNotification(object, notification);
}

void AXObjectCache::nodeTextChangePlatformNotification(AccessibilityObject* object, AXTextChange textChange, unsigned offset, const String& text)
{
    if (!object
        || object->document()->renderView()->layoutState()
        || object->document()->childNeedsStyleRecalc())
        return;
    ChromeClient& client = m_document.frame()->page()->chrome().client();
    client.postAccessibilityNodeTextChangeNotification(object, textChange, offset, text);
}

void AXObjectCache::frameLoadingEventPlatformNotification(AccessibilityObject* object, AXLoadingEvent loadingEvent)
{
    if (!object
        || object->document()->renderView()->layoutState()
        || object->document()->childNeedsStyleRecalc())
        return;
    ChromeClient& client = m_document.frame()->page()->chrome().client();
    client.postAccessibilityFrameLoadingEventNotification(object, loadingEvent);
}

void AXObjectCache::handleScrolledToAnchor(const Node* scrolledToNode)
{
    if (!scrolledToNode)
        return;
    postPlatformNotification(AccessibilityObject::firstAccessibleObjectFromNode(scrolledToNode), AXScrolledToAnchor);
}

void AXObjectCache::platformHandleFocusedUIElementChanged(Node* oldFocusedNode, Node* newFocusedNode)
{
    if (!newFocusedNode)
        return;

    Page* page = newFocusedNode->document().page();
    if (!page || !page->chrome().platformPageClient())
        return;

    AccessibilityObject* focusedObject = focusedUIElementForPage(page);
    if (!focusedObject)
        return;

    postPlatformNotification(focusedObject, AXFocusedUIElementChanged);
}

} // namespace WebCore

#endif
