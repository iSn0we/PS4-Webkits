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
#include "OSAllocator.h"

#if OS(ORBIS)

#include "PageAllocation.h"
#include <errno.h>
#include <sys/mman.h>
#include <wtf/Assertions.h>

#define ENABLE_MANX_MEMORY_PROFILING 1

namespace {
#if ENABLE(MANX_MEMORY_PROFILING)
    ssize_t s_maxTotalAllocations = 0;
    ssize_t s_totalAllocations = 0;
    int s_lock = 0;
    inline void reportAllocation(ssize_t s)
    {
        int old;
        do {
            old = s_lock;
        } while (old != __sync_val_compare_and_swap(&s_lock, 0, 1));

        s_totalAllocations += s;
        s_maxTotalAllocations = s_maxTotalAllocations < s_totalAllocations ? s_totalAllocations : s_maxTotalAllocations;

        do {
            old = s_lock;
        } while (old != __sync_val_compare_and_swap(&s_lock, 1, 0));
    }
#else
#define reportAllocation(x) (void)(x)
#endif
}

namespace manx {
size_t OSAllocatorHighWatermark()
{ 
#if ENABLE(MANX_MEMORY_PROFILING)
    return s_maxTotalAllocations;
#else
    return 0;
#endif
}
size_t OSAllocatorUsage()
{
    return OSAllocatorHighWatermark();
}
}

namespace WTF {

void* OSAllocator::reserveUncommitted(size_t bytes, Usage usage, bool writable, bool executable, bool includesGuardPages)
{
    void* result = reserveAndCommit(bytes, usage, writable, executable, includesGuardPages);
    return result;
}

void* OSAllocator::reserveAndCommit(size_t bytes, Usage usage, bool writable, bool executable, bool includesGuardPages)
{
    // All POSIX reservations start out logically committed.
    int protection = PROT_READ;
    if (writable)
        protection |= PROT_WRITE;
    if (executable)
        protection |= PROT_EXEC;

    int flags = MAP_PRIVATE | MAP_ANON;
    int fd = -1;
    void* result = 0;

    result = mmap(result, bytes, protection, flags, fd, 0);
    if (result == MAP_FAILED) {
#if ENABLE(CLASSIC_INTERPRETER) || ENABLE(LLINT)
        if (executable)
            result = 0;
        else
            CRASH();
#else
        CRASH();
#endif
    }

    reportAllocation(bytes);

    if (result && includesGuardPages) {
        // We use mmap to remap the guardpages rather than using mprotect as
        // mprotect results in multiple references to the code region. This
        // breaks the madvise based mechanism we use to return physical memory
        // to the OS.
        mmap(result, pageSize(), PROT_NONE, MAP_FIXED | MAP_PRIVATE | MAP_ANON, fd, 0);
        mmap(static_cast<char*>(result) + bytes - pageSize(), pageSize(), PROT_NONE, MAP_FIXED | MAP_PRIVATE | MAP_ANON, fd, 0);
    }
    return result;
}

void OSAllocator::commit(void* address, size_t bytes, bool writable, bool executable)
{
    // Non-MADV_FREE_REUSE reservations automatically commit on demand.
    UNUSED_PARAM(address);
    UNUSED_PARAM(bytes);
    UNUSED_PARAM(writable);
    UNUSED_PARAM(executable);
}

void OSAllocator::decommit(void* address, size_t bytes)
{
    while (madvise(address, bytes, MADV_DONTNEED) == -1 && errno == EAGAIN) { }
}

void OSAllocator::releaseDecommitted(void* address, size_t bytes)
{
    int result = munmap(address, bytes);
    if (result == -1)
        CRASH();

    reportAllocation(-bytes);
}

} // namespace WTF
#endif
