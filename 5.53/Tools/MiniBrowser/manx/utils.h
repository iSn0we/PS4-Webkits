/*
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

#pragma once
#include <WebKit/WebKit2_C.h>
#include <manx/RunLoop.h>
#include <string>

std::string createString(WKStringRef);
std::string createString(WKURLRef);

std::string toUtf8(const std::wstring&);
std::wstring fromUtf8(const std::string&);

template <typename T>
class Timer {
    class TimerFunction : public Manx::RunLoop::Function {
    public:
        TimerFunction(Timer* t)
            : m_timer(t) { }
    private:
        void call() { m_timer->fired(); }
        class Timer<T>* m_timer;
    };

public:
    Timer(T* t, void (T::*m)())
        : m_this(t), m_method(m), m_timer(0) { }
    ~Timer()
    {
        if (m_timer)
            delete m_timer;
    }
    void init()
    {
        m_timer = Manx::RunLoop::current()->createTimer(new TimerFunction(this));
    }
    void startOneShot(double delay)
    {
        m_timer->startOneShot(delay);
    }
    void startRepeating(double interval)
    {
        m_timer->startRepeating(interval);
    }
    void stop()
    {
        m_timer->stop();
    }
    bool isActive()
    {
        return m_timer->isActive();
    }

    void fired() { (m_this->*m_method)(); }

private:
    T* m_this;
    void (T::*m_method)();
    Manx::RunLoop::Timer* m_timer;
};
