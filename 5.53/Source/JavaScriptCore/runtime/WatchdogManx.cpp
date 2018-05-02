/*
 * Copyright (C) 2017 Sony Interactive Entertainment Inc.
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
 * THIS SOFTWARE IS PROVIDED BY SONY COMPUTER ENTERTAINMENT INC. ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL SONY COMPUTER ENTERTAINMENT INC.
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "Watchdog.h"

#include <manx/RunLoop.h>
#include <wtf/Threading.h>
#include <wtf/ThreadingPrimitives.h>
#include <cstdio>

namespace JSC {

class Watchdog::WatchdogTimerHandler : public Manx::RunLoop::Function {
public:
    WatchdogTimerHandler(Watchdog*);
    ~WatchdogTimerHandler();

    void startTimer(std::chrono::microseconds timeout);
    void stopTimer();

    virtual void call() override;

private:
    static void watchdogThread(void*);

    Watchdog* m_watchdog;

    ThreadIdentifier m_watchdogThreadId;
    Mutex m_runLoopMutex;
    ThreadCondition m_runLoopCreatedCondition;

    Manx::RunLoop* m_watchdogRunLoop;
    Manx::RunLoop::Timer* m_watchdogTimer;
};

Watchdog::WatchdogTimerHandler::WatchdogTimerHandler(Watchdog* watchdog)
    : m_watchdog(watchdog)
    , m_watchdogThreadId(0)
    , m_watchdogRunLoop(0)
    , m_watchdogTimer(0)
{
    MutexLocker ml(m_runLoopMutex);
    m_watchdogThreadId = createThread(&watchdogThread, this, "JavaScriptCore::Watchdog");

    if (m_watchdogThreadId) {
        m_runLoopCreatedCondition.wait(m_runLoopMutex);
        ASSERT(m_watchdogRunLoop);
    } else {
        LOG_ERROR("WatchdogTimerHandler: couldn't create the JSC watchdog handler thread!");
    }
}

Watchdog::WatchdogTimerHandler::~WatchdogTimerHandler()
{
    if (m_watchdogRunLoop) {
        delete m_watchdogTimer;

        // m_watchdogRunLoop will self-delete when the thread exits
        m_watchdogRunLoop->quit();

        waitForThreadCompletion(m_watchdogThreadId);
    }
}

void Watchdog::WatchdogTimerHandler::startTimer(std::chrono::microseconds timeout)
{
    if (!m_watchdogRunLoop)
        return;

    if (!m_watchdogTimer)
        m_watchdogTimer = m_watchdogRunLoop->createTimer(this);

    // Manx::RunLoop::Timer uses (double)seconds
    double timeoutSeconds = (double)(timeout.count()) / 1000000.0;
    m_watchdogTimer->startOneShot(timeoutSeconds);
}

void Watchdog::WatchdogTimerHandler::stopTimer()
{
    if (!m_watchdogTimer)
        return;

    m_watchdogTimer->stop();
}

void Watchdog::WatchdogTimerHandler::call()
{
    m_watchdog->m_timerDidFire = true;
}

void Watchdog::WatchdogTimerHandler::watchdogThread(void* context)
{
    WatchdogTimerHandler* thiz = reinterpret_cast<WatchdogTimerHandler*>(context);

    {
        MutexLocker ml(thiz->m_runLoopMutex);

        thiz->m_watchdogRunLoop = Manx::RunLoop::current();
        thiz->m_runLoopCreatedCondition.signal();
    }

    Manx::RunLoop::run();
}


void Watchdog::initTimer()
{
    m_timerHandler = 0;
}

void Watchdog::destroyTimer()
{
    delete m_timerHandler;
}

void Watchdog::startTimer(std::chrono::microseconds timeout)
{
    if (!m_timerHandler)
        m_timerHandler = new WatchdogTimerHandler(this);

    m_timerHandler->startTimer(timeout);
}

void Watchdog::stopTimer()
{
    if (m_timerHandler)
        m_timerHandler->stopTimer();
}

} // namespace JSC
