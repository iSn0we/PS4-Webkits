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
#include "WorkQueue.h"

#include <wtf/RefPtr.h>
#include <wtf/Threading.h>

namespace WTF {

namespace {

class SocketEventFunction : public Manx::WorkQueue::Function {
public:
    SocketEventFunction(std::function<void()> function)
        : m_function(function) { }
    virtual ~SocketEventFunction() { }
    virtual void call() const
    {
        m_function();
    }
private:
    std::function<void()> m_function;
};

class TimerFunction : public Manx::WorkQueue::Function {
public:
    TimerFunction(WorkQueue& workQueue, std::function<void()> function)
        : m_workQueue(workQueue), m_function(function) { }
    virtual ~TimerFunction() { }
    virtual void call() const
    {
        m_function();
    }
private:
    Ref<WorkQueue> m_workQueue;
    std::function<void()> m_function;
};

} // namespace

// WorkQueue

void WorkQueue::platformInitialize(const char* name, Type, QOS)
{
    m_manxWorkQueue = Manx::WorkQueue::create(name);
    m_manxWorkQueue->startThread();
}

void WorkQueue::platformInvalidate()
{
    m_manxWorkQueue->destroy();
    m_manxWorkQueue = 0;
}

void WorkQueue::dispatch(std::function<void()> function)
{
    dispatchAfter(std::chrono::nanoseconds(0), function);
}

void WorkQueue::dispatchAfter(std::chrono::nanoseconds delay, std::function<void()> function)
{
    double d = std::chrono::duration_cast<std::chrono::duration<double>>(delay).count();
    m_manxWorkQueue->dispatchAfterDelay(new TimerFunction(*this, function), d);
}

void WorkQueue::registerSocketEventHandler(Manx::Pipe::Handle socket, std::function<void()> function)
{
    m_manxWorkQueue->registerSocketEventHandler(socket, new SocketEventFunction(function));
}

void WorkQueue::unregisterSocketEventHandler(Manx::Pipe::Handle socket)
{
    m_manxWorkQueue->unregisterSocketEventHandler(socket);
}

} // namespace WTF
