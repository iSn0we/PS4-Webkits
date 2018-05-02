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

#ifndef GLMemoryInfo_h
#define GLMemoryInfo_h

namespace WebCore {

class GLMemoryInfo {
public:
    GLMemoryInfo()
        : heap(0)
        , texture(0)
        , surfaces(0)
        , programs(0)
        , buffers(0)
        , commandBuffers(0)
        , total(0)
        , maxAllowed(0) { }

    uint32_t heap; /* Various runtime heap memory usage */
    uint32_t texture; /* Memory used for Texture surfaces */
    uint32_t surfaces; /* Memory used for Render and Display surfaces for Color, Depth and Stencil */
    uint32_t programs; /* Memory used for Shader and Programs */
    uint32_t buffers; /* Memory used for Buffers, vertex ringbuffers */
    uint32_t commandBuffers; /* Memory used for the underlying Command Buffers */
    uint32_t total; /* Total memory used by driver */
    uint32_t maxAllowed; /* Maximum memory allowed to be used, 0 if no restriction */
    uint64_t fmemMappedSizeTotal; /* Total size flexible memory used */
    uint64_t fmemMappedSizeLimit; /* Limit size flexible memory */
};

inline bool operator==(const GLMemoryInfo& a, const GLMemoryInfo& b)
{
    return a.heap == b.heap
        && a.texture == b.texture
        && a.surfaces == b.surfaces
        && a.programs == b.programs
        && a.buffers == b.buffers
        && a.commandBuffers == b.commandBuffers
        && a.buffers == b.buffers
        && a.total == b.total
        && a.maxAllowed == b.maxAllowed
        && a.fmemMappedSizeTotal == b.fmemMappedSizeTotal
        && a.fmemMappedSizeLimit == b.fmemMappedSizeLimit;
}

inline bool operator!=(const GLMemoryInfo& a, const GLMemoryInfo& b)
{
    return !(a == b);
}

}

#endif // GLMemoryInfo_h
