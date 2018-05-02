/*
 * Copyright (C) 2013 Intel Corporation. All rights reserved.
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
#include "GraphicsSurface.h"

#if USE(GRAPHICS_SURFACE)

#include "GLPlatformContext.h"
#include "GLTransportSurface.h"
#include "NotImplemented.h"
#include "TextureMapperGL.h"
#include <canvas.h>

namespace WebCore {

struct GraphicsSurfacePrivate {
    GraphicsSurfacePrivate(PlatformBufferHandle canvasHandle, const IntSize& size, GraphicsSurface::Flags flags)
        : m_flags(0)
        , m_rect(FloatPoint::zero(), size)
        , m_size(size)
        , m_sharedHandle(canvasHandle)
    {
        if (flags & GraphicsSurface::SupportsAlpha)
            m_flags |= TextureMapperGL::ShouldBlend;
        if (flags & GraphicsSurface::ShouldFlipTexture)
            m_flags |= TextureMapperGL::ShouldFlipTexture;

        sce::CanvasUtil::mapCanvas(canvasHandle);
    }

    ~GraphicsSurfacePrivate()
    {
    }

    void destroy()
    {
    }

    PlatformBufferHandle handle() const
    {
        return m_sharedHandle;
    }

    TextureMapperGL::Flags flags() const { return m_flags; }

    const FloatRect& rect() const { return m_rect; }
    const IntSize& size() const { return m_size; }

private:
    TextureMapperGL::Flags m_flags;
    FloatRect m_rect;
    IntSize m_size;
    PlatformBufferHandle m_sharedHandle;
};

GraphicsSurfaceToken GraphicsSurface::platformExport()
{
    return m_private->handle();
}

uint32_t GraphicsSurface::platformGetTextureID()
{
    notImplemented();
    return 0;
}

void GraphicsSurface::platformCopyToGLTexture(uint32_t /*target*/, uint32_t /*id*/, const IntRect& /*targetRect*/, const IntPoint& /*offset*/)
{
    notImplemented();
}

void GraphicsSurface::platformCopyFromTexture(uint32_t /*textureId*/, const IntRect&)
{
    notImplemented();
}

void GraphicsSurface::platformPaintToTextureMapper(TextureMapper* textureMapper, const FloatRect& targetRect, const TransformationMatrix& transform, float opacity)
{
    uint32_t canvasHandle = m_private->handle();
    TransformationMatrix adjustedTransform = transform;
    adjustedTransform.multiply(TransformationMatrix::rectToRect(m_private->rect(), targetRect));

    TextureMapperGL* textureMapperGL = static_cast<TextureMapperGL*>(textureMapper);
    textureMapperGL->drawCanvasTexture(canvasHandle, m_private->flags(), m_private->size(), m_private->rect(), adjustedTransform, opacity);
}

uint32_t GraphicsSurface::platformFrontBuffer() const
{
    notImplemented();
    return 0;
}

uint32_t GraphicsSurface::platformSwapBuffers()
{
    notImplemented();
    return 0;
}

IntSize GraphicsSurface::platformSize() const
{
    return m_private->size();
}

PassRefPtr<GraphicsSurface> GraphicsSurface::platformCreate(const IntSize& size, Flags flags, const PlatformGraphicsContext3D shareContext)
{
    notImplemented();
    return PassRefPtr<GraphicsSurface>();
}

PassRefPtr<GraphicsSurface> GraphicsSurface::platformImport(const IntSize& size, Flags flags, const GraphicsSurfaceToken& token)
{
    // GraphicsSurface doesn't yet support copyToTexture or singlebuffered surface.
    if (flags & SupportsCopyToTexture || flags & SupportsSingleBuffered)
        return PassRefPtr<GraphicsSurface>();

    RefPtr<GraphicsSurface> surface = adoptRef(new GraphicsSurface(size, flags));
    surface->m_private = new GraphicsSurfacePrivate(token.frontBufferHandle, size, flags);
    return surface;
}

char* GraphicsSurface::platformLock(const IntRect&, int* /*outputStride*/, LockOptions)
{
    // GraphicsSurface is currently only being used for WebGL, which does not require this locking mechanism.
    return 0;
}

void GraphicsSurface::platformUnlock()
{
    // GraphicsSurface is currently only being used for WebGL, which does not require this locking mechanism.
}

void GraphicsSurface::platformDestroy()
{
    if (!m_private)
        return;

    m_private->destroy();

    delete m_private;
    m_private = 0;
}

std::unique_ptr<GraphicsContext> GraphicsSurface::platformBeginPaint(const IntSize&, char*, int)
{
    notImplemented();
    return nullptr;
}

PassRefPtr<Image> GraphicsSurface::createReadOnlyImage(const IntRect&)
{
    notImplemented();
    return 0;
}

}

#endif
