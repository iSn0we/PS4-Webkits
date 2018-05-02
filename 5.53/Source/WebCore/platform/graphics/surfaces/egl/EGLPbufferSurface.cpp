/*
 * Copyright (C) 2013 Intel Corporation. All rights reserved.
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
#include "EGLPbufferSurface.h"

#if PLATFORM(MANX) && USE(EGL) && USE(GRAPHICS_SURFACE)

#include "EGLConfigSelector.h"
#include "EGLHelper.h"
#include "GLPlatformContext.h"
#include "PlatformDisplay.h"
#include "PlatformDisplayManx.h"

namespace WebCore {

EGLWindowTransportSurface::EGLWindowTransportSurface(const IntSize& size, GLPlatformSurface::SurfaceAttributes attributes)
    : EGLTransportSurface(size, attributes)
{
    if (!m_configSelector)
        return;

    if (!m_configSelector->surfaceContextConfig()) {
        destroy();
        return;
    }

    EGLint visualId = m_configSelector->nativeVisualId(m_configSelector->surfaceContextConfig());

    if (visualId == -1) {
        destroy();
        return;
    }

    auto& sharedDisplay = static_cast<PlatformDisplayManx&>(PlatformDisplay::sharedDisplay());
    m_drawable = eglCreateWindowSurface(sharedDisplay.eglDisplay(), m_configSelector->surfaceContextConfig(), sharedDisplay.native(), 0);

    if (m_drawable == EGL_NO_SURFACE) {
        LOG_ERROR("Failed to create EGL surface(%d).", eglGetError());
        destroy();
    }
}

EGLWindowTransportSurface::~EGLWindowTransportSurface()
{
}

void EGLWindowTransportSurface::swapBuffers()
{
    if (!eglSwapBuffers(EGLHelper::eglDisplay(), m_drawable))
        LOG_ERROR("Failed to SwapBuffers(%d).", eglGetError());
}

void EGLWindowTransportSurface::destroy()
{
    EGLTransportSurface::destroy();

    if (m_bufferHandle) {
        m_bufferHandle = 0;
    }
}

EGLPbufferSurface::EGLPbufferSurface(GLPlatformSurface::SurfaceAttributes surfaceAttributes)
    : EGLOffScreenSurface(surfaceAttributes)
{
    if (!m_configSelector)
        return;

    EGLConfig config = m_configSelector->pbufferContextConfig();

    if (!config) {
        destroy();
        return;
    }

    EGLint visualId = m_configSelector->nativeVisualId(config);

    if (visualId == -1) {
        destroy();
        return;
    }

    m_drawable = eglCreatePbufferSurface(PlatformDisplay::sharedDisplay().eglDisplay(), config, 0);

    if (m_drawable == EGL_NO_SURFACE) {
        LOG_ERROR("Failed to create EGL surface(%d).", eglGetError());
        destroy();
    }
}

EGLPbufferSurface::~EGLPbufferSurface()
{
}

bool EGLPbufferSurface::isCurrentDrawable() const
{
    return m_drawable == eglGetCurrentSurface(EGL_DRAW);
}

PlatformSurfaceConfig EGLPbufferSurface::configuration()
{
    return m_configSelector->pbufferContextConfig();
}

void EGLPbufferSurface::destroy()
{
    EGLOffScreenSurface::destroy();

    // There is no buffer assigned to the surface from the client
    ASSERT(m_bufferHandle == 0);
}

EGLPbufferTransportSurfaceClient::EGLPbufferTransportSurfaceClient(const PlatformBufferHandle handle, const IntSize& size, bool hasAlpha)
    : GLTransportSurfaceClient()
    , m_size(size)
    , m_totalBytes(0)
{
    if (!handle)
        return;

    createTexture();
    GLPlatformSurface::SurfaceAttributes sharedSurfaceAttributes = GLPlatformSurface::Default;

    if (hasAlpha)
        sharedSurfaceAttributes = GLPlatformSurface::SupportAlpha;

    EGLConfigSelector configSelector(sharedSurfaceAttributes);
    EGLConfig config = configSelector.pbufferContextConfig();
    m_eglImage = std::make_unique<EGLTextureFromPbuffer>(hasAlpha, config);

    if (!m_eglImage->isValid() || eglGetError() != EGL_SUCCESS)
        destroy();

    if (m_eglImage)
        return;

    m_totalBytes = m_size.width() * m_size.height() * 4;

#if USE(OPENGL_ES_2)
    m_format = GraphicsContext3D::RGBA;
    static bool bgraSupported = GLPlatformContext::supportsGLExtension("GL_EXT_texture_format_BGRA8888");
    if (bgraSupported)
        m_format = GraphicsContext3D::BGRA;
#endif

    createTexture();
    prepareTexture();
}

EGLPbufferTransportSurfaceClient::~EGLPbufferTransportSurfaceClient()
{
}

void EGLPbufferTransportSurfaceClient::destroy()
{
    GLTransportSurfaceClient::destroy();

    if (m_eglImage) {
        m_eglImage->destroy();
        m_eglImage = nullptr;
    }

    eglWaitGL();
}

void EGLPbufferTransportSurfaceClient::prepareTexture()
{
    ::glBindTexture(GL_TEXTURE_2D, m_texture);

    if (m_eglImage) {
        m_eglImage->reBindTexImage();
        return;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, m_format, m_size.width(), m_size.height(), 0, m_format, GL_UNSIGNED_BYTE, 0);
}

EGLTextureFromPbuffer::EGLTextureFromPbuffer(bool hasAlpha, EGLConfig config)
    : m_eglImage(0)
    , m_surface(EGL_NO_SURFACE)
{
    const EGLint pbufferAttribs[] = { EGL_TEXTURE_FORMAT, hasAlpha ? EGL_TEXTURE_RGBA : EGL_TEXTURE_RGB, EGL_TEXTURE_TARGET, EGL_TEXTURE_2D, EGL_NONE };
    m_surface = eglCreatePbufferSurface(EGLHelper::eglDisplay(), config, pbufferAttribs);

    if (m_surface != EGL_NO_SURFACE && !eglBindTexImage(EGLHelper::eglDisplay(), m_surface, EGL_BACK_BUFFER))
        destroy();

    if (m_surface != EGL_NO_SURFACE)
        return;

    static const EGLint imageAttrs[] = { EGL_IMAGE_PRESERVED_KHR, EGL_TRUE, EGL_NONE };
    EGLHelper::createEGLImage(&m_eglImage, EGL_NATIVE_PIXMAP_KHR, (EGLClientBuffer)(0), imageAttrs);

    if (m_eglImage) {
        EGLHelper::imageTargetTexture2DOES(m_eglImage);
        EGLint error = eglGetError();

        if (error != EGL_SUCCESS)
            destroy();
    }
}

EGLTextureFromPbuffer::~EGLTextureFromPbuffer()
{
}

void EGLTextureFromPbuffer::destroy()
{
    eglWaitNative(EGL_CORE_NATIVE_ENGINE);

    if (m_surface != EGL_NO_SURFACE)
        eglReleaseTexImage(EGLHelper::eglDisplay(), m_surface, EGL_BACK_BUFFER);

    if (m_eglImage) {
        EGLHelper::destroyEGLImage(m_eglImage);
        m_eglImage = 0;
    }

    if (m_surface != EGL_NO_SURFACE) {
        eglDestroySurface(EGLHelper::eglDisplay(), m_surface);
        m_surface = EGL_NO_SURFACE;
    }

    eglWaitGL();
}

bool EGLTextureFromPbuffer::isValid() const
{
    if (m_surface || m_eglImage)
        return true;

    return false;
}

bool EGLTextureFromPbuffer::bindTexImage()
{
    if (m_surface != EGL_NO_SURFACE) {
        bool success = eglBindTexImage(EGLHelper::eglDisplay(), m_surface, EGL_BACK_BUFFER);
        return success;
    }

    if (m_eglImage) {
        EGLHelper::imageTargetTexture2DOES(m_eglImage);
        return true;
    }

    return false;
}

bool EGLTextureFromPbuffer::reBindTexImage()
{
    if (m_surface != EGL_NO_SURFACE) {
        bool success = eglReleaseTexImage(EGLHelper::eglDisplay(), m_surface, EGL_BACK_BUFFER);

        if (success)
            success = eglBindTexImage(EGLHelper::eglDisplay(), m_surface, EGL_BACK_BUFFER);

        return success;
    }

    if (m_eglImage) {
        EGLHelper::imageTargetTexture2DOES(m_eglImage);
        return true;
    }

    return false;
}

}

#endif
