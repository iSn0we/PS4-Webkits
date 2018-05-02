/*
 * Copyright (C) 2011 Apple Inc. All rights reserved.
 * Copyright (C) 2012 Sony Interactive Entertainment Inc.
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

#ifndef WKPagePrivateManx_h
#define WKPagePrivateManx_h

#include <WebKit/WKBase.h>

#ifdef __cplusplus
extern "C" {
#endif

WK_EXPORT int WKPageGetProcessIdentifier(WKPageRef page);
WK_EXPORT void WKPageBlurFocusedNode(WKPageRef page);
WK_EXPORT void WKPageSetCaretVisible(WKPageRef pageRef, bool visible);
WK_EXPORT void WKPageSetScaleFactorKeepingScrollOffset(WKPageRef pageRef, double scale);

typedef void(*WKPageRunJavaScriptAndReturnStringFunction)(WKStringRef, WKErrorRef, void*);
WK_EXPORT void WKPageRunJavaScriptInMainFrameAndReturnString(WKPageRef page, WKStringRef script, void* context, WKPageRunJavaScriptAndReturnStringFunction function);

WK_EXPORT uint32_t WKPageGetAcceleratedCompositingBackgroundColor(WKPageRef page);

WK_EXPORT void WKPageSuspendActiveDOMObjectsAndAnimations(WKPageRef page);
WK_EXPORT void WKPageResumeActiveDOMObjectsAndAnimations(WKPageRef page);

#ifdef __cplusplus
}
#endif

#endif /* WKPagePrivateManx_h */
