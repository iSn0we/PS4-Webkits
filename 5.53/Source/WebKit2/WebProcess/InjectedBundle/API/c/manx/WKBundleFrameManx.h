/*
 * Copyright (C) 2015 Sony Interactive Entertainment Inc.
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

#ifndef WKBundleFrameManx_h
#define WKBundleFrameManx_h

#include <WebKit/WKBase.h>
#include <WebKit/WKFrame.h>
#include <WebKit/WKGLMemoryInfo.h>

#ifdef __cplusplus
extern "C" {
#endif

WK_EXPORT bool WKBundleFrameRegisterAsyncImageDecoder(WKStringRef propertyName, WKBundleFrameRef, WKBundleScriptWorldRef);

WK_EXPORT double WKBundleFrameConvertMonotonicTimeToDocumentTime(WKBundleFrameRef, double timestamp);

WK_EXPORT bool WKBundleFrameEnableHTMLTile(WKBundleFrameRef, WKBundleScriptWorldRef);

WK_EXPORT void WKBundleFrameGetACMemoryInfo(WKBundleFrameRef, WKGLMemoryInfo*);

WK_EXPORT void WKBundleFrameEnableThrottlingByTileCount(WKBundleFrameRef, int count);
WK_EXPORT void WKBundleFrameEnableThrottlingByTime(WKBundleFrameRef, double timeInMs);
WK_EXPORT void WKBundleFrameDisableThrottlingByTileCount(WKBundleFrameRef);
WK_EXPORT void WKBundleFrameDisableThrottlingByTime(WKBundleFrameRef);

#ifdef __cplusplus
}
#endif

#endif /* WKBundleFrameManx_h */
