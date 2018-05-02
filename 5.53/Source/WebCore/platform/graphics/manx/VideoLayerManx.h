/*
 * Copyright (C) 2013 Sony Computer Entertainment Inc.
 * Copyright (C) 2014 Sony Computer Entertainment Inc.
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

#ifndef VideoLayerManx_h
#define VideoLayerManx_h

#if USE(TEXTURE_MAPPER) && USE(TEXTURE_MAPPER_GL) && ENABLE(VIDEO)
#include "GraphicsLayer.h"
#include "TextureMapperPlatformLayer.h"

namespace WebCore {

class VideoLayerManx : public TextureMapperPlatformLayer {
public:
    VideoLayerManx();
    virtual ~VideoLayerManx();

    void updateCanvasHandle(bool isCanvasMode, uint32_t canvasHandle);
    bool isCanvasMode() const { return m_isCanvasMode; }
    uint32_t canvasHandle() const { return m_canvasHandle; }

    // TextureMapperPlatformLayer interface.
    virtual void paintToTextureMapper(TextureMapper*, const FloatRect&, const TransformationMatrix& modelViewMatrix = TransformationMatrix(), float opacity = 1.0);

#if USE(GRAPHICS_SURFACE)
    virtual IntSize platformLayerSize() const;
    virtual uint32_t copyToGraphicsSurface();
    virtual GraphicsSurfaceToken graphicsSurfaceToken() const;
    virtual GraphicsSurface::Flags graphicsSurfaceFlags() const;
#endif

private:
    bool m_isCanvasMode;
    uint32_t m_canvasHandle;
};

}
#endif
#endif // VideoLayerManx_h
