/*
 * Copyright (C) 2014 Apple Inc. All rights reserved.
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
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

#ifndef StaticMutex_h
#define StaticMutex_h

#include "BAssert.h"
#include "BPlatform.h"
#include <atomic>
#include <mutex>
#include <thread>

#if BOS(ORBIS)
#include <pthread.h>
#include <pthread_np.h>
#endif

// A fast replacement for std::mutex, for use in static storage.

// Use StaticMutex in static storage, where global constructors and exit-time
// destructors are prohibited, but all memory is zero-initialized automatically.

namespace bmalloc {

class StaticMutex {
protected:
    // Subclasses that support non-static storage must use explicit initialization.
    void init();

public:
    void lock();
#if !BOS(ORBIS)
    bool try_lock();
#endif
    void unlock();

#if BOS(ORBIS)
    StaticMutex();
    ~StaticMutex();
#endif

private:
#if BOS(ORBIS)
    pthread_mutex_t m_mutex { PTHREAD_MUTEX_INITIALIZER };
#else
    void lockSlowCase();

    std::atomic_flag m_flag;
#endif
};

static inline void sleep(
    std::unique_lock<StaticMutex>& lock, std::chrono::milliseconds duration)
{
    if (duration == std::chrono::milliseconds(0))
        return;
    
    lock.unlock();
    std::this_thread::sleep_for(duration);
    lock.lock();
}

static inline void waitUntilFalse(
    std::unique_lock<StaticMutex>& lock, std::chrono::milliseconds sleepDuration,
    bool& condition)
{
    while (condition) {
        condition = false;
        sleep(lock, sleepDuration);
    }
}

#if BOS(ORBIS)
inline StaticMutex::StaticMutex()
{
    pthread_mutex_init(&m_mutex, nullptr);
#if !BOS(ORBIS_PUBLIC)
    pthread_mutex_setname_np(&m_mutex, "SceBMalloc");
#endif
}
inline StaticMutex::~StaticMutex()
{
    pthread_mutex_destroy(&m_mutex);
}

inline void StaticMutex::init()
{
}

inline void StaticMutex::lock()
{
    pthread_mutex_lock(&m_mutex);
}

inline void StaticMutex::unlock()
{
    pthread_mutex_unlock(&m_mutex);
}
#else
inline void StaticMutex::init()
{
    m_flag.clear();
}

inline bool StaticMutex::try_lock()
{
    return !m_flag.test_and_set(std::memory_order_acquire);
}

inline void StaticMutex::lock()
{
    if (!try_lock())
        lockSlowCase();
}

inline void StaticMutex::unlock()
{
    m_flag.clear(std::memory_order_release);
}
#endif

} // namespace bmalloc

#endif // StaticMutex_h
