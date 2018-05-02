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
#include "CursorNavigation.h"

#if ENABLE(MANX_CURSOR_NAVIGATION)

#include "EventHandler.h"
#include "FocusController.h"
#include "Frame.h"
#include "FrameView.h"
#include "HTMLAreaElement.h"
#include "HTMLImageElement.h"
#include "HTMLNames.h"
#include "HitTestResult.h"
#include "InlineTextBox.h"
#include "KeyboardEvent.h"
#include "MainFrame.h"
#include "NodeTraversal.h"
#include "Page.h"
#include "RenderBox.h"
#include "RenderInline.h"
#include "RenderObject.h"
#include "RenderText.h"
#include "Settings.h"
#include "ShadowRoot.h"
#include "SpatialNavigation.h"
#include "WindowsKeyboardCodes.h"

namespace WebCore {

using namespace HTMLNames;

CursorNavigation::CursorNavigation(Page& page)
    : m_page(page)
{
}

static FocusDirection focusDirectionFromCursorNavDirection(CursorNavDirection direction)
{
    return FocusDirection(direction + FocusDirectionUp - CursorNavDirectionUp);
}

static void updateFocusCandidateIfNeeded(FocusDirection direction, const FocusCandidate& current, FocusCandidate& candidate, FocusCandidate& closest)
{
    ASSERT(candidate.visibleNode->isElementNode());
    ASSERT(candidate.visibleNode->renderer());

    // Ignore iframes that don't have a src attribute
    if (frameOwnerElement(candidate) && (!frameOwnerElement(candidate)->contentFrame() || candidate.rect.isEmpty()))
        return;

    // Ignore off screen child nodes of containers that do not scroll (overflow:hidden)
    if (candidate.isOffscreen && !canBeScrolledIntoView(direction, candidate))
        return;

    distanceDataForNode(direction, current, candidate);
    if (candidate.distance == maxDistance())
        return;

    if (candidate.isOffscreenAfterScrolling && candidate.alignment < Full)
        return;

    if (closest.isNull()) {
        closest = candidate;
        return;
    }

#if 0 // This logic selects a wrong candidate if lines overwraps by low line-height.
    LayoutRect intersectionRect = intersection(candidate.rect, closest.rect);
    if (!intersectionRect.isEmpty() && !areElementsOnSameLine(closest, candidate)) {
        // If 2 nodes are intersecting, do hit test to find which node in on top.
        LayoutUnit x = intersectionRect.x() + intersectionRect.width() / 2;
        LayoutUnit y = intersectionRect.y() + intersectionRect.height() / 2;
        HitTestResult result = candidate.visibleNode->document()->page()->mainFrame()->eventHandler()->hitTestResultAtPoint(IntPoint(x, y), false, true);
        if (candidate.visibleNode->contains(result.innerNode())) {
            closest = candidate;
            return;
        }
        if (closest.visibleNode->contains(result.innerNode()))
            return;
    }
#endif

    if (candidate.alignment == closest.alignment) {
        if (candidate.distance < closest.distance)
            closest = candidate;
        return;
    }

    if (candidate.alignment > closest.alignment)
        closest = candidate;
}

static bool isScrollableNode(const Node* node)
{
    ASSERT(!is<Document>(node));

    if (!node)
        return false;

    if (RenderObject* renderer = node->renderer())
        return is<RenderBox>(renderer) && downcast<RenderBox>(renderer)->canBeScrolledAndHasScrollableArea() && node->hasChildNodes();

    return false;
}

static void findFocusCandidateInContainer(Node* container, const LayoutRect& startingRect, FocusDirection direction, FocusCandidate& closest)
{
    ASSERT(container);
    Node* focusedNode = 0;

    Node* node = container->firstChild();
    FocusCandidate current;
    current.rect = startingRect;
    current.focusableNode = focusedNode;
    current.visibleNode = focusedNode;

    for (; node; node = (node->isFrameOwnerElement() || isScrollableNode(node)) ? NodeTraversal::nextSkippingChildren(*node, container) : NodeTraversal::next(*node, container)) {
        if (node == focusedNode)
            continue;

        if (!node->isElementNode())
            continue;

        if (!downcast<Element>(node)->supportsFocus() && !node->isFrameOwnerElement() && !isScrollableNode(node))
            continue;

        FocusCandidate candidate = FocusCandidate(node, direction);
        if (candidate.isNull())
            continue;

        candidate.enclosingScrollableBox = container;
        updateFocusCandidateIfNeeded(direction, current, candidate, closest);
    }
}

static bool advanceFocusDirectionallyInContainer(Node* container, const LayoutRect& startingRect, FocusDirection direction, FocusCandidate& candidate)
{
    if (!container)
        return false;

    // Find the closest node within current container in the direction of the navigation.
    findFocusCandidateInContainer(container, startingRect, direction, candidate);

    if (candidate.isNull())
        return scrollInDirection(container, direction);

    if (candidate.isOffscreenAfterScrolling) {
        scrollInDirection(container, direction);
        return true;
    }
    ASSERT(candidate.visibleNode->isElementNode());
    static_cast<Element*>(candidate.visibleNode)->scrollIntoViewIfNeeded(false);
    if (HTMLFrameOwnerElement* frameElement = frameOwnerElement(candidate)) {
        frameElement->contentFrame()->document()->updateLayoutIgnorePendingStylesheets();
        FocusCandidate childFrameCandidate;
        findFocusCandidateInContainer(frameElement->contentFrame()->document(), startingRect, direction, childFrameCandidate);
        if (!childFrameCandidate.isNull())
            candidate = childFrameCandidate;
    }
    return true;
}

static bool firstVisibleRenderObjectPosition(const Node* node, LayoutPoint& pos)
{
    RenderObject* renderer = node->renderer();
    if (!renderer)
        return false;
    if (renderer->isBox()) {
        pos = flooredLayoutPoint(renderer->absoluteBoundingBoxRect().center());
        return true;
    }
    if (is<RenderText>(renderer)) {
        RenderText* text = downcast<RenderText>(renderer);
        if (InlineTextBox* firstTextBox = text->firstTextBox()) {
            pos = flooredLayoutPoint(renderer->localToAbsolute(firstTextBox->logicalFrameRect().center(), UseTransforms));
            return true;
        }
    }
    for (Node* child = node->firstChild(); child; child = child->nextSibling()) {
        if (firstVisibleRenderObjectPosition(child, pos))
            return true;
    }
    return false;
}

bool cursorPositionAtCandidate(const FocusCandidate& candidate, IntPoint& cursorPosition)
{
    if (candidate.focusableNode->hasTagName(areaTag)) {
        HTMLAreaElement* area = static_cast<HTMLAreaElement*>(candidate.focusableNode);
        LayoutPoint pos = area->computeRect(area->imageElement()->renderer()).center();
        cursorPosition = roundedIntPoint(pointToAbsoluteCoordinates(candidate.visibleNode->document().frame(), pos));
        return true;
    }
    LayoutPoint pos;
    bool found = firstVisibleRenderObjectPosition(candidate.visibleNode, pos);
    if (!found)
        return false;
    cursorPosition = roundedIntPoint(pointToAbsoluteCoordinates(candidate.visibleNode->document().frame(), pos));
    return true;
}

static Node* parentContainer(Node* node)
{
    ASSERT(node);
    do {
        if (is<Document>(node))
            node = downcast<Document>(node)->document().frame()->ownerElement();
        else
            node = node->parentNode();
    } while (node && !node->isDocumentNode() && !isScrollableNode(node));

    return node;
}

static LayoutRect startingRectForHitTestResult(const HitTestResult& result, LayoutPoint pos, FocusDirection direction)
{
    LayoutRect startingRect;
    if (result.URLElement()) {
        if (result.URLElement()->hasTagName(HTMLNames::areaTag)) {
            HTMLAreaElement* area = static_cast<HTMLAreaElement*>(result.URLElement());
            return virtualRectForAreaElementAndDirection(area, direction);
        }
        if (result.URLElement()->renderer())
            return nodeRectInAbsoluteCoordinates(result.URLElement());
    }
    for (Node* node = result.innerNode(); node; node = node->parentOrShadowHostNode()) {
        if (!is<Element>(node))
            continue;
        if (downcast<Element>(node)->supportsFocus())
            return nodeRectInAbsoluteCoordinates(node);
    }
    startingRect = LayoutRect(pos, IntSize(1, 1));
    return startingRect;
}

bool CursorNavigation::moveCursor(CursorNavDirection navDirection, bool& moved, IntPoint& newCursorPosition)
{
    moved = false;
    FocusDirection direction = focusDirectionFromCursorNavDirection(navDirection);
    LayoutPoint absCursorPosition = m_page.mainFrame().view()->windowToContents(m_cursorPosition);
    HitTestResult result = m_page.mainFrame().eventHandler().hitTestResultAtPoint(absCursorPosition);
    Node* container = result.innerNode();
    if (!container)
        return false;
    container->document().updateLayoutIgnorePendingStylesheets();
    if (!isScrollableNode(container))
        container = parentContainer(container);
    LayoutRect startingRect = startingRectForHitTestResult(result, absCursorPosition, direction);
    FocusCandidate candidate;
    
    bool consumed = false;
    do {
        consumed = advanceFocusDirectionallyInContainer(container, startingRect, direction, candidate);
        if (consumed)
            break;
        container = parentContainer(container);
        if (!container)
            break;
        if (is<Document>(container))
            downcast<Document>(container)->updateLayoutIgnorePendingStylesheets();
    } while (true);

    if (candidate.visibleNode && !candidate.isOffscreenAfterScrolling) {
        IntPoint pos;
        if (cursorPositionAtCandidate(candidate, pos)) {
            newCursorPosition = m_page.mainFrame().view()->contentsToWindow(pos);
            moved = true;
            return true;
        }
    }
    return consumed;
}

bool CursorNavigation::handleKeyEvent(const PlatformKeyboardEvent& event, bool& moved, IntPoint& newCursorPosition)
{
    moved = false;
    if (event.type() != PlatformEvent::KeyDown && event.type() != PlatformEvent::RawKeyDown)
        return false;
    CursorNavDirection dir;
    switch (event.windowsVirtualKeyCode()) {
    case VK_LEFT:
        dir = CursorNavDirectionLeft;
        break;
    case VK_RIGHT:
        dir = CursorNavDirectionRight;
        break;
    case VK_UP:
        dir = CursorNavDirectionUp;
        break;
    case VK_DOWN:
        dir = CursorNavDirectionDown;
        break;
    default:
        return false;
    }
    return moveCursor(dir, moved, newCursorPosition);
}

void CursorNavigation::setCursorPosition(const PlatformMouseEvent& mouseEvent)
{
    m_cursorPosition = mouseEvent.position();
}

bool isCursorNavigationEnabled(const Frame* frame)
{
    return (frame && frame->settings().isCursorNavigationEnabled());
}

}

#endif
