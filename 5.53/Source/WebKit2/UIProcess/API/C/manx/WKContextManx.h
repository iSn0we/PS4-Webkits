/*
 * Copyright (C) 2010 Apple Inc. All rights reserved.
 * Copyright (C) 2013 Sony Interactive Entertainment Inc.
 * Copyright (C) 2014 Sony Interactive Entertainment Inc.
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

#ifndef WKContextManx_h
#define WKContextManx_h

#include <WebKit/WKBase.h>
#include <WebKit/WKContext.h>

#ifdef __cplusplus
extern "C" {
#endif

enum {
    kWKNetworkBandwidthModeNormal = 1,
    kWKNetworkBandwidthModePrivileged = 2,
};

typedef uint32_t WKNetworkBandwidthMode;

WK_EXPORT void WKContextSetUserID(WKContextRef, int userID);
WK_EXPORT void WKContextSetWebProcessPath(WKContextRef, WKStringRef path);
WK_EXPORT void WKContextSetUserStorageDirectory(WKContextRef, WKStringRef userStorageDirectory);
WK_EXPORT void WKContextSetPrivilegedNetworkBandwidth(WKContextRef, WKNetworkBandwidthMode bandwithMode);
WK_EXPORT void WKContextSetWebSecurityFilter(WKContextRef, int filterMode);
WK_EXPORT void WKContextSetIFilterHTTPProxy(WKContextRef, bool enable, WKStringRef server, int port, WKStringRef userId, WKStringRef password);
WK_EXPORT void WKContextSetHTTPProxy(WKContextRef, bool enable, WKStringRef server, int port, WKStringRef userId, WKStringRef password);
WK_EXPORT void WKContextSetCookieMode(WKContextRef, int cookieMode);
WK_EXPORT void WKContextEnableAccessibility(WKContextRef context);
WK_EXPORT bool WKContextAccessibilityEnabled(WKContextRef context);
WK_EXPORT void WKContextPauseHTMLTiles(WKContextRef context);
WK_EXPORT void WKContextResumeHTMLTiles(WKContextRef context);
WK_EXPORT bool WKContextGetMemoryCacheDisabled(WKContextRef context);

#ifdef __cplusplus
}
#endif

#endif /* WKContextManx_h */
