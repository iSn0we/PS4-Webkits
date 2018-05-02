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

#include "config.h"

#if USE(TEXTURE_MAPPER) && USE(TEXTURE_MAPPER_GL) && ENABLE(VIDEO)
#include "VideoLayerManx.h"

#include "GLDefs.h"
#include "NotImplemented.h"
#include "OpenGLManx.h"
#include "TextureMapperGL.h"

namespace WebCore {

#define INVALID_CANVAS_HANDLE_VALUE 0xFFFFFFFFU

VideoLayerManx::VideoLayerManx()
    : m_isCanvasMode(false)
    , m_canvasHandle(INVALID_CANVAS_HANDLE_VALUE)
{
}

VideoLayerManx::~VideoLayerManx()
{
}

void VideoLayerManx::updateCanvasHandle(bool isCanvasMode, uint32_t canvasHandle)
{
    if (isCanvasMode == m_isCanvasMode && canvasHandle == m_canvasHandle)
        return;

    if (isCanvasMode && canvasHandle != INVALID_CANVAS_HANDLE_VALUE) {
        m_isCanvasMode = true;
        m_canvasHandle = canvasHandle;
    } else {
        m_isCanvasMode = false;
        m_canvasHandle = INVALID_CANVAS_HANDLE_VALUE;
    }
}

void VideoLayerManx::paintToTextureMapper(TextureMapper* textureMapper, const FloatRect& targetRect, const TransformationMatrix& matrix, float opacity)
{
    if (!isCanvasMode())
        return;

    (void)targetRect;
    (void)matrix;
    (void)opacity;
}

#if USE(GRAPHICS_SURFACE)
IntSize VideoLayerManx::platformLayerSize() const
{
    return IntSize(1, 1);
}

uint32_t VideoLayerManx::copyToGraphicsSurface()
{
    return m_canvasHandle;
}

GraphicsSurfaceToken VideoLayerManx::graphicsSurfaceToken() const
{
    return GraphicsSurfaceToken(m_canvasHandle);
}

GraphicsSurface::Flags VideoLayerManx::graphicsSurfaceFlags() const
{
    return GraphicsSurface::SupportsTextureSource | GraphicsSurface::SupportsSharing;
}
#endif

}
#endif
