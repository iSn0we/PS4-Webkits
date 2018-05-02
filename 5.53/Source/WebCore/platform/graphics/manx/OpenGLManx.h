/*
 * Copyright (C) 2013 Sony Interactive Entertainment Inc.
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


#ifndef OpenGLManx_h
#define OpenGLManx_h

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#if GL_SCE_orbis_canvas_texture
extern "C" void glOrbisTexImageCanvas2DSCE(GLenum target, GLuint uCanvasHandle);
#define glTexImageCanvas2D glOrbisTexImageCanvas2DSCE
#endif

#if GL_SCE_texture_resource
extern "C" void glOrbisTexImageResourceSCE(GLenum target, const GLvoid *texture_resource_descriptor, GLsizei descriptor_size);
extern "C" void glOrbisMapTextureResourceSCE(GLenum target, GLvoid* texture_resource_descriptor, GLsizei *descriptor_size);
extern "C" void glOrbisUnmapTextureResourceSCE(GLenum target);
#define glTexImageResource glOrbisTexImageResourceSCE
#define glMapTextureResource glOrbisMapTextureResourceSCE
#define glUnmapTextureResource glOrbisUnmapTextureResourceSCE
#endif

#if EGL_SCE_piglet_memory_info
#define EGLMEMORYINFO_t EGLPIGLETMEMORYINFOSCE_t
#define eglMemoryInfo eglPigletMemoryInfoSCE
MANX_WEBCORE_EXPORT extern "C" void eglPigletMemoryInfoSCE(EGLPIGLETMEMORYINFOSCE_t* pInfo);
#endif

#endif
