/*
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
#include "PlatformTouchPoint.h"

#include <manx/TouchEvent.h>

#if ENABLE(TOUCH_EVENTS)

namespace WebCore {

PlatformTouchPoint::PlatformTouchPoint(const Manx::TouchPoint& point)
{
    m_id = point.id;
    switch (point.state) {
    case Manx::TouchPoint::StatePressed:
        m_state = PlatformTouchPoint::TouchPressed;
        break;
    case Manx::TouchPoint::StateReleased:
        m_state = PlatformTouchPoint::TouchReleased;
        break;
    case Manx::TouchPoint::StateMoved:
        m_state = PlatformTouchPoint::TouchMoved;
        break;
    case Manx::TouchPoint::StateStationary:
        m_state = PlatformTouchPoint::TouchStationary;
        break;
    case Manx::TouchPoint::StateCancelled:
        m_state = PlatformTouchPoint::TouchCancelled;
        break;
    case Manx::TouchPoint::StateUndefined:
        break;
    }
    m_pos = IntPoint(point.position.x, point.position.y);
    m_screenPos = IntPoint(point.screenPosition.x, point.screenPosition.y);
}

}
#endif
