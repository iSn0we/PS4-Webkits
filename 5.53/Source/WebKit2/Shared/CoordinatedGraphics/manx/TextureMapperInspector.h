/*
 * Copyright (C) 2017 Sony Computer Entertainment Inc.
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

#ifndef TextureMapperInspector_h
#define TextureMapperInspector_h

#if USE(TEXTURE_MAPPER) && PLATFORM(MANX)
#include <WebCore/FloatPoint.h>
#include <WebCore/TransformationMatrix.h>
#include <wtf/Noncopyable.h>

namespace WebCore {
class TextureMapperGL;
}

namespace WebKit {

class CoordinatedGraphicsScene;

class TextureMapperInspector {
    WTF_MAKE_NONCOPYABLE(TextureMapperInspector);
    WTF_MAKE_FAST_ALLOCATED;
public:
    TextureMapperInspector();

    void paint(CoordinatedGraphicsScene*, WebCore::TextureMapperGL*, const WebCore::TransformationMatrix&, const WebCore::FloatRect& clipRect);
    void setShowLayerTable(bool show) { m_showLayerTable = show; }

private:
    void paintLayerTable(CoordinatedGraphicsScene*, WebCore::TextureMapperGL*, const WebCore::TransformationMatrix&, const WebCore::FloatRect& clipRect);

    bool m_showLayerTable;
};

} // namespace WebKit

#endif // USE(TEXTURE_MAPPER) && PLATFORM(MANX)

#endif // TextureMapperInspector_h
