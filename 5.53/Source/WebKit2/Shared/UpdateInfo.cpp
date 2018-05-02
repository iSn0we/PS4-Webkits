/*
 * Copyright (C) 2011 Apple Inc. All rights reserved.
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
#include "UpdateInfo.h"

#include "WebCoreArgumentCoders.h"

namespace WebKit {

void UpdateInfo::encode(IPC::ArgumentEncoder& encoder) const
{
    encoder << viewSize;
    encoder << deviceScaleFactor;
    encoder << scrollRect;
    encoder << scrollOffset;
    encoder << updateRectBounds;
    encoder << updateRects;
    encoder << updateScaleFactor;
    encoder << bitmapHandle;
    encoder << bitmapOffset;
#if ENABLE(MANX_INDIRECT_COMPOSITING)
    encoder << canvasHoles.size();
    for (size_t i = 0; i < canvasHoles.size(); i++)
        encoder << canvasHoles[i];
#endif
}

bool UpdateInfo::decode(IPC::ArgumentDecoder& decoder, UpdateInfo& result)
{
    if (!decoder.decode(result.viewSize))
        return false;
    if (!decoder.decode(result.deviceScaleFactor))
        return false;
    if (!decoder.decode(result.scrollRect))
        return false;
    if (!decoder.decode(result.scrollOffset))
        return false;
    if (!decoder.decode(result.updateRectBounds))
        return false;
    if (!decoder.decode(result.updateRects))
        return false;
    if (!decoder.decode(result.updateScaleFactor))
        return false;
    if (!decoder.decode(result.bitmapHandle))
        return false;
    if (!decoder.decode(result.bitmapOffset))
        return false;
#if ENABLE(MANX_INDIRECT_COMPOSITING)
    size_t count = 0;
    if (!decoder.decode(count))
        return false;
    for (size_t i = 0; i < count; i++) {
        WebCanvasHole canvasHole;
        if (!decoder.decode(canvasHole))
            return false;
        result.canvasHoles.append(canvasHole);
    }
#endif

    return true;
}

} // namespace WebKit