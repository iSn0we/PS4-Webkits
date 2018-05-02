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

#ifndef EGLPbufferSurface_h
#define EGLPbufferSurface_h

#if PLATFORM(MANX) && USE(EGL) && USE(GRAPHICS_SURFACE)

#include "EGLSurface.h"

namespace WebCore {

// Contents of the surface are backed by native window.
class EGLWindowTransportSurface : public EGLTransportSurface {
public:
    EGLWindowTransportSurface(const IntSize&, GLPlatformSurface::SurfaceAttributes);
    virtual ~EGLWindowTransportSurface();
    virtual void swapBuffers() override;
    virtual void destroy() override;
};

class EGLPbufferSurface : public EGLOffScreenSurface {
public:
    EGLPbufferSurface(GLPlatformSurface::SurfaceAttributes);
    virtual ~EGLPbufferSurface();
    virtual bool isCurrentDrawable() const override;
    virtual PlatformSurfaceConfig configuration() override;
    virtual void destroy() override;
};

class EGLTextureFromPbuffer {
public:
    EGLTextureFromPbuffer(bool, EGLConfig);
    virtual ~EGLTextureFromPbuffer();
    bool bindTexImage();
    bool isValid() const;
    bool reBindTexImage();
    void destroy();

private:
    EGLImageKHR m_eglImage;
    EGLSurface m_surface;
};

class EGLPbufferTransportSurfaceClient : public GLTransportSurfaceClient {
public:
    EGLPbufferTransportSurfaceClient(const PlatformBufferHandle, const IntSize&, bool);
    virtual ~EGLPbufferTransportSurfaceClient();
    virtual void prepareTexture() override;
    virtual void destroy() override;

private:
    IntSize m_size;
    GLuint m_format;
    unsigned m_totalBytes;
    std::unique_ptr<EGLTextureFromPbuffer> m_eglImage;
};

}

#endif

#endif
