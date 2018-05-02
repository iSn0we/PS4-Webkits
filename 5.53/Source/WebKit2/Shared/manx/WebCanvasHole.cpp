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


#include "config.h"
#include "WebCanvasHole.h"

#if ENABLE(MANX_INDIRECT_COMPOSITING)

#include "ArgumentDecoder.h"
#include "ArgumentEncoder.h"
#include "WebCoreArgumentCoders.h"

using namespace WebCore;

namespace IPC {

template<> struct ArgumentCoder<WebCore::FloatQuad> {
    static void encode(ArgumentEncoder& encoder, const WebCore::FloatQuad& floatQuad)
    {
        SimpleArgumentCoder<FloatQuad>::encode(encoder, floatQuad);
    }
    static bool decode(ArgumentDecoder& decoder, WebCore::FloatQuad& floatQuad)
    {
        return SimpleArgumentCoder<FloatQuad>::decode(decoder, floatQuad);
    }
};

}

namespace WebKit {

void WebCanvasHole::encode(IPC::ArgumentEncoder& encoder) const
{
    encoder << m_data.quad;
    encoder << m_data.opacity;
    encoder << m_data.canvasHandle;
    encoder << m_data.flags;
}

bool WebCanvasHole::decode(IPC::ArgumentDecoder& decoder, WebCanvasHole& result)
{
    if (!decoder.decode(result.m_data.quad))
        return false;
    if (!decoder.decode(result.m_data.opacity))
        return false;
    if (!decoder.decode(result.m_data.canvasHandle))
        return false;
    if (!decoder.decode(result.m_data.flags))
        return false;

    return true;
}

} // namespace WebKit

#endif
