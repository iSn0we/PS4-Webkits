/*
 * Copyright (C) 2010 Apple Inc. All rights reserved.
 * Copyright (C) 2016 Sony Interactive Entertainment Inc.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
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
#include "DisplayRefreshMonitorManx.h"

#if USE(REQUEST_ANIMATION_FRAME_DISPLAY_MONITOR)
#include <wtf/CurrentTime.h>
#include <wtf/MainThread.h>

namespace WebCore {

DisplayRefreshMonitorManx::DisplayRefreshMonitorManx(PlatformDisplayID displayID)
    : DisplayRefreshMonitor(displayID)
    , m_timerThread(0)
    , m_monitor(0)
{

}

DisplayRefreshMonitorManx::~DisplayRefreshMonitorManx()
{
    setIsActive(false);
    if (m_timerThread)
        waitForThreadCompletion(m_timerThread);

    delete m_monitor;

    cancelCallOnMainThread(DisplayRefreshMonitor::handleDisplayRefreshedNotificationOnMainThread, this);
}

void DisplayRefreshMonitorManx::timerThreadFunc(void* data)
{
    DisplayRefreshMonitorManx* that = (DisplayRefreshMonitorManx*)data;

    Manx::DisplayRefreshMonitor* monitor = that->m_monitor;
    if (!monitor)
        return;

    monitor->start();
    while (that->isActive()) {
        monitor->waitVblank();
        FUNCTION_MARKER;
        that->displayLinkFired();
    }
    monitor->stop();
}

bool DisplayRefreshMonitorManx::requestRefreshCallback()
{
    if (!isActive())
        return false;

    if (!m_monitor)
        m_monitor = new Manx::DisplayRefreshMonitor;

    if (!m_monitor) {
        setIsActive(false);
        return false;
    }

    if (!m_timerThread) {
        setIsActive(true);
        m_timerThread = createThread(timerThreadFunc, this, "SceDisplayRefreshMonitor");
        if (!m_timerThread) {
            delete m_monitor;
            m_monitor = 0;
            setIsActive(false);
            return false;
        }
    }
    
    MutexLocker lock(mutex());
    setIsScheduled(true);
    return true;
}

void DisplayRefreshMonitorManx::displayLinkFired()
{
    MutexLocker lock(mutex());
    if (!isScheduled() || !isPreviousFrameDone())
        return;

    setIsPreviousFrameDone(false);

    setMonotonicAnimationStartTime(monotonicallyIncreasingTime());

    CPU_SYNC();
    callOnMainThread(handleDisplayRefreshedNotificationOnMainThread, this);
}

}

#endif // USE(REQUEST_ANIMATION_FRAME_DISPLAY_MONITOR)
