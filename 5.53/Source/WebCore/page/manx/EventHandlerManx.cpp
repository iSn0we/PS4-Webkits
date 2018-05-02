/*
 * Copyright (C) 2008 Apple Computer, Inc.  All rights reserved.
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
#include "EventHandler.h"

#include "DataTransfer.h"
#include "FocusController.h"
#include "Frame.h"
#include "FrameView.h"
#include "KeyboardEvent.h"
#include "MouseEventWithHitTestResults.h"
#include "NotImplemented.h"
#include "Page.h"
#include "PlatformKeyboardEvent.h"
#include "RenderWidget.h"
#include "Scrollbar.h"

namespace WebCore {

#if ENABLE(DRAG_SUPPORT)
const double EventHandler::TextDragDelay = 0.0;
#endif

bool EventHandler::passMousePressEventToSubframe(MouseEventWithHitTestResults& mev, Frame* subframe)
{
    subframe->eventHandler().handleMousePressEvent(mev.event());
    return true;
}

bool EventHandler::passMouseMoveEventToSubframe(MouseEventWithHitTestResults& mev, Frame* subframe, WebCore::HitTestResult* hittest)
{
    subframe->eventHandler().handleMouseMoveEvent(mev.event(), hittest);
    return true;
}

bool EventHandler::passMouseReleaseEventToSubframe(MouseEventWithHitTestResults& mev, Frame* subframe)
{
    subframe->eventHandler().handleMouseReleaseEvent(mev.event());
    return true;
}

bool EventHandler::passWidgetMouseDownEventToWidget(const MouseEventWithHitTestResults& event)
{
    // Figure out which view to send the event to.
    RenderObject* target = event.targetNode() ? event.targetNode()->renderer() : 0;
    if (!target || !target->isWidget())
        return false;
    return passMouseDownEventToWidget(downcast<RenderWidget>(*target).widget());
}

bool EventHandler::passWidgetMouseDownEventToWidget(RenderWidget* renderWidget)
{
    return passMouseDownEventToWidget(renderWidget->widget());
}

bool EventHandler::passWheelEventToWidget(const PlatformWheelEvent& event, Widget& widget)
{
    if (!is<FrameView>(widget))
        return false;

    return downcast<FrameView>(widget).frame().eventHandler().handleWheelEvent(event);
}

bool EventHandler::tabsToAllFormControls(KeyboardEvent* event) const
{
    return tabsToLinks(event);
}

bool EventHandler::passSubframeEventToSubframe(MouseEventWithHitTestResults&, Frame* subframe, HitTestResult*)
{
    notImplemented();
    return false;
}

bool EventHandler::passMouseDownEventToWidget(Widget*)
{
    notImplemented();
    return false;
}

void EventHandler::focusDocumentView()
{
    if (Page* page = m_frame.page())
        page->focusController().setFocusedFrame(&m_frame);
}

bool EventHandler::eventActivatedView(const PlatformMouseEvent&) const
{
    return false;
}

#if ENABLE(DRAG_SUPPORT)
PassRefPtr<DataTransfer> EventHandler::createDraggingDataTransfer() const
{
    return DataTransfer::createForDragAndDrop();
}
#endif

unsigned EventHandler::accessKeyModifiers()
{
    return PlatformKeyboardEvent::AltKey;
}

}