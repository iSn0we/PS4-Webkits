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


#include "config.h"
#include "OpenGLManx.h"

#include <EGL/egl.h>

template <typename T>
T getProcAddress(const char* name)
{
    T proc = (T)eglGetProcAddress(name);
    if (!proc) {
        WTFReportError(__FILE__, __LINE__, __PRETTY_FUNCTION__, "eglGetProcAddress(%s) failed.\n", name);
        CRASH();
    }
    return proc;
}

#if GL_SCE_orbis_canvas_texture
extern "C" void glOrbisTexImageCanvas2DSCE(GLenum target, GLuint uCanvasHandle)
{
    static PFNGLGLTEXIMAGE2DCANVASORBISSCEPROC proc = getProcAddress<PFNGLGLTEXIMAGE2DCANVASORBISSCEPROC>("glOrbisTexImageCanvas2DSCE");
    if (proc)
        proc(target, uCanvasHandle);
}
#endif

#if GL_SCE_texture_resource
extern "C" void glOrbisTexImageResourceSCE(GLenum target, const GLvoid *texture_resource_descriptor, GLsizei descriptor_size)
{
    static PFNGLTEXIMAGERESOURCESCEPROC proc = getProcAddress<PFNGLTEXIMAGERESOURCESCEPROC>("glTexImageResourceSCE");
    if (proc)
        proc(target, texture_resource_descriptor, descriptor_size);
}

extern "C" void glOrbisMapTextureResourceSCE(GLenum target, GLvoid* texture_resource_descriptor, GLsizei *descriptor_size)
{
    static PFNGLMAPTEXTURERESOURCESCEPROC proc = getProcAddress<PFNGLMAPTEXTURERESOURCESCEPROC>("glMapTextureResourceSCE");
    if (proc)
        proc(target, texture_resource_descriptor, descriptor_size);
}

extern "C" void glOrbisUnmapTextureResourceSCE(GLenum target)
{
    static PFNGLUNMAPTEXTURERESOURCESCEPROC proc = getProcAddress<PFNGLUNMAPTEXTURERESOURCESCEPROC>("glUnmapTextureResourceSCE");
    if (proc)
        proc(target);
}
#endif

#if EGL_SCE_piglet_memory_info
extern "C" void eglPigletMemoryInfoSCE(EGLPIGLETMEMORYINFOSCE_t* pInfo)
{
    static PFNEGLPIGLETMEMORYINFOSCEPROC proc = getProcAddress<PFNEGLPIGLETMEMORYINFOSCEPROC>("eglPigletMemoryInfoSCE");
    if (proc)
        proc(pInfo);
}
#endif
