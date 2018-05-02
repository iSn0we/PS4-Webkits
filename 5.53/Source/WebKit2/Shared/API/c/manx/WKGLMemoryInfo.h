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

#ifndef WKGLMemoryInfo_h
#define WKGLMemoryInfo_h

struct WKGLMemoryInfo {
    size_t heap; /* Various runtime heap memory usage */
    size_t texture; /* Memory used for Texture surfaces */
    size_t surfaces; /* Memory used for Render and Display surfaces for Color, Depth and Stencil */
    size_t programs; /* Memory used for Shader and Programs */
    size_t buffers; /* Memory used for Buffers, vertex ringbuffers */
    size_t commandBuffers; /* Memory used for the underlying Command Buffers */
    size_t total; /* Total memory used by driver */
    size_t maxAllowed; /* Maximum memory allowed to be used, 0 if no restriction */
    uint64_t fmemMappedSizeTotal; /* Total size flexible memory used */
    uint64_t fmemMappedSizeLimit; /* Limit size flexible memory */
};
typedef struct WKGLMemoryInfo WKGLMemoryInfo;

#endif /* WKGLMemoryInfo_h */
