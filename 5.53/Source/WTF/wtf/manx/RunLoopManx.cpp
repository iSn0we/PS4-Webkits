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
#include "RunLoop.h"

using namespace std;

namespace WTF {

namespace {

class TimerFunction : public Manx::RunLoop::Function {
public:
    TimerFunction(std::function<void()> function)
        : m_function(function) { }
    virtual ~TimerFunction() { }
    virtual void call() { m_function(); }
private:
    std::function<void()> m_function;
};

} // namespace

// RunLoop

RunLoop::RunLoop()
    : m_manxRunLoop(Manx::RunLoop::current())
    , m_wakeUpTimer(m_manxRunLoop->createTimer(new TimerFunction(bind(&RunLoop::performWork, this))))
{
}

RunLoop::~RunLoop()
{
}

void RunLoop::wakeUp()
{
    m_wakeUpTimer->startOneShot(0);
}

// RunLoop::Timer

RunLoop::TimerBase::TimerBase(RunLoop& runLoop)
    : m_runLoop(runLoop)
    , m_timer(m_runLoop.m_manxRunLoop->createTimer(new TimerFunction(bind(&RunLoop::TimerBase::fired, this))))
{
}

RunLoop::TimerBase::~TimerBase()
{
}

void RunLoop::TimerBase::start(double nextFireInterval, bool repeat)
{
    if (repeat)
        m_timer->startRepeating(nextFireInterval);
    else
        m_timer->startOneShot(nextFireInterval);
}

void RunLoop::TimerBase::stop()
{
    m_timer->stop();
}

bool RunLoop::TimerBase::isActive() const
{
    return m_timer->isActive();
}

void RunLoop::run()
{
    Manx::RunLoop::run();
}

void RunLoop::stop()
{
    m_manxRunLoop->quit();
}

} // namespace WebCore
