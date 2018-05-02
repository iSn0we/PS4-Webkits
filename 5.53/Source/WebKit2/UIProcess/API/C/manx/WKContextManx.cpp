/*
 * Copyright (C) 2010, 2011 Apple Inc. All rights reserved.
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
#include "WKContextManx.h"

#include "WKAPICast.h"
#include "WebProcessPool.h"
#if ENABLE(MANX_HTMLTILE)
#include "WebTileManagerProxy.h"
#endif

using namespace WebKit;

void WKContextSetUserID(WKContextRef context, int userID)
{
    toImpl(context)->setUserID(userID);
}

void WKContextSetWebProcessPath(WKContextRef context, WKStringRef path)
{
    toImpl(context)->setWebProcessPath(toImpl(path)->string());
}

void WKContextSetUserStorageDirectory(WKContextRef context, WKStringRef userStorageDirectory)
{
    toImpl(context)->setUserStorageDirectory(toImpl(userStorageDirectory)->string());
}

void WKContextSetPrivilegedNetworkBandwidth(WKContextRef context, WKNetworkBandwidthMode bandwidthMode)
{
#if USE(NTF)
    toImpl(context)->setPrivilegedNetworkBandwidth(bandwidthMode);
#endif
}

void WKContextSetWebSecurityFilter(WKContextRef context, int filterMode)
{
#if USE(NTF)
    toImpl(context)->setWebSecurityFilter(filterMode);
#endif
}

void WKContextSetIFilterHTTPProxy(WKContextRef context, bool enable, WKStringRef server, int port, WKStringRef userId, WKStringRef password)
{
#if USE(NTF)
    toImpl(context)->setIFilterHTTPProxy(enable, toImpl(server)->string(), port, toImpl(userId)->string(), toImpl(password)->string());
#endif
}

void WKContextSetHTTPProxy(WKContextRef context, bool enable, WKStringRef server, int port, WKStringRef userId, WKStringRef password)
{
#if USE(NTF)
    toImpl(context)->setHTTPProxy(enable, toImpl(server)->string(), port, toImpl(userId)->string(), toImpl(password)->string());
#endif
}

void WKContextSetCookieMode(WKContextRef context, int cookieMode)
{
#if USE(NTF)
    toImpl(context)->setCookieMode(cookieMode);
#endif
}

void WKContextEnableAccessibility(WKContextRef context)
{
#if HAVE(ACCESSIBILITY)
    toImpl(context)->enableAccessibility();
#endif
}

bool WKContextAccessibilityEnabled(WKContextRef context)
{
#if HAVE(ACCESSIBILITY)
    return toImpl(context)->accessibilityEnabled();
#else
    return false;
#endif
}

void WKContextPauseHTMLTiles(WKContextRef context)
{
#if ENABLE(MANX_HTMLTILE)
    WebTileManagerProxy* proxy = toImpl(context)->supplement<WebTileManagerProxy>();
    if (proxy)
        proxy->pauseWebTiles();
#endif
}

void WKContextResumeHTMLTiles(WKContextRef context)
{
#if ENABLE(MANX_HTMLTILE)
    WebTileManagerProxy* proxy = toImpl(context)->supplement<WebTileManagerProxy>();
    if (proxy)
        proxy->resumeWebTiles();
#endif
}

bool WKContextGetMemoryCacheDisabled(WKContextRef context)
{
    return toImpl(context)->getMemoryCacheDisabled();
}
