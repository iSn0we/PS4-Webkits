/*
 * Copyright (C) 2011 Samsung Electronics. All rights reserved.
 * Copyright (C) 2012 Sony Interactive Entertainment Inc.
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
 * THIS SOFTWARE IS PROVIDED BY MOTOROLA INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL MOTOROLA INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "WebProcess.h"

#include "WebProcessCreationParameters.h"
#include <WebCore/AXObjectCache.h>
#include <WebCore/FileSystem.h>
#include <WebCore/MemoryCache.h>
#include <WebCore/NotImplemented.h>
#include <WebCore/PageCache.h>
#include <wtf/RAMSize.h>
#include <wtf/text/CString.h>

#if USE(NTF)
#include <ntf/Common.h>
#include <ntf/NetworkControl.h>
#include <ntf/Proxy.h>
#include <ntf/WebSecurity.h>
#endif

#include <manx/ScopedTime.h>

namespace WebKit {

void WebProcess::platformSetCacheModel(CacheModel cacheModel)
{
    unsigned cacheTotalCapacity = 64 * WTF::MB;
    unsigned cacheMinDeadCapacity = 0;
    unsigned cacheMaxDeadCapacity = cacheTotalCapacity;
    auto deadDecodedDataDeletionInterval = std::chrono::seconds { 0 };
    unsigned pageCacheSize = 0;
    bool memoryCacheDisabled = false;

    const char* razorCpuSettingString = getenv("MANX_RAZOR_CPU_SETTING");
    int razorLevel = razorCpuSettingString ? atoi(razorCpuSettingString) : 0;
    if (razorLevel == 3) {
        memoryCacheDisabled = true;
        cacheTotalCapacity = 0;
        cacheMinDeadCapacity = 0;
        cacheMaxDeadCapacity = 0;
    } else {
        const char* memoryCacheString = getenv("MANX_MEMORYCACHE_TOTAL_CAPACITY");
        if (memoryCacheString) {
            cacheTotalCapacity = static_cast<unsigned>(atoi(memoryCacheString)) * WTF::MB;
            cacheMinDeadCapacity = 0;
            cacheMaxDeadCapacity = cacheTotalCapacity;
        }
    }

    auto& memoryCache = WebCore::MemoryCache::singleton();
    memoryCache.setDisabled(memoryCacheDisabled);
    memoryCache.setCapacities(cacheMinDeadCapacity, cacheMaxDeadCapacity, cacheTotalCapacity);
    memoryCache.setDeadDecodedDataDeletionInterval(deadDecodedDataDeletionInterval);
    WebCore::PageCache::singleton().setMaxSize(pageCacheSize);
}

void WebProcess::platformClearResourceCaches(ResourceCachesToClear)
{
    notImplemented();
}

void WebProcess::platformInitializeWebProcess(WebProcessCreationParameters&& parameters)
{
    MANX_MEASURE_FUNCTION_TIME;

    CString userIDString = String::number(parameters.userID).ascii();
    setenv("WEBKIT_USER_ID", userIDString.data(), 1);
    setenv("WEBKIT_NET_SKIP_CLEAR_SESSCOOKIE", "1", 1);

#if USE(NTF)
    // Initialization of the NTF
    NTF::CreationParameters ntfCreationParameters;

    const char* secureWebProcess = getenv("MANX_SECURE_WEB_PROCESS");
    if (secureWebProcess && !strcmp(secureWebProcess, "1"))
        ntfCreationParameters.isSecureWebProcess = true;

    const char* webProcessHTMLTile = getenv("MANX_WEB_PROCESS_HTML_TILE");
    if (webProcessHTMLTile && !strcmp(webProcessHTMLTile, "1"))
        ntfCreationParameters.isWebProcessHTMLTile = true;

    const char* systemProxyOnSecureEnv = getenv("MANX_PROXY_SETTING");
    if (systemProxyOnSecureEnv && !strcmp(systemProxyOnSecureEnv, "1"))
        ntfCreationParameters.useSystemProxyOnSecureEnv = true;

    size_t length = parameters.cookieDatabaseDirectory.utf8().length();
    size_t arraySize = sizeof(ntfCreationParameters.cookieStorageDirectory);

    if ((length > 0) && (length < arraySize)) {
        strncpy(ntfCreationParameters.cookieStorageDirectory, parameters.cookieDatabaseDirectory.utf8().data(), arraySize - 1);
        ntfCreationParameters.cookieStorageDirectory[arraySize - 1] = '\0';
        WebCore::makeAllDirectories(parameters.cookieDatabaseDirectory);
    }

    ntfCreationParameters.cookieJarDBMode = parameters.cookieMode;

    length = parameters.diskCacheDirectory.utf8().length();
    arraySize = sizeof(ntfCreationParameters.diskCacheDirectory);

    if ((length > 0) && (length < arraySize)) {
        strncpy(ntfCreationParameters.diskCacheDirectory, parameters.diskCacheDirectory.utf8().data(), arraySize - 1);
        ntfCreationParameters.diskCacheDirectory[arraySize - 1] = '\0';
    }

    NTF::initialize(ntfCreationParameters);
#endif

#if HAVE(ACCESSIBILITY)
    if (parameters.enableAccessibility)
        WebCore::AXObjectCache::enableAccessibility();
#endif
}

void WebProcess::platformTerminate()
{
#if USE(NTF)
    NTF::finalize();
#endif
}

#if USE(NTF)
void WebProcess::setPrivilegedNetworkBandwidth(uint32_t bandwidthMode)
{
    NTF::NetworkControl::setPrivilegedNetworkBandwidth(bandwidthMode);
}

void WebProcess::setWebSecurityFilter(int filterMode)
{
    NTF::WebSecurity::setUrlFilterMode(filterMode);
}

void WebProcess::setIFilterHTTPProxy(bool enable, String server, int port, String userId, String password)
{
    NTF::Proxy::setIFilterHTTPProxy(
        enable,
        server.latin1().data(), 
        server.length(),
        port,
        userId.latin1().data(), 
        userId.length(),
        password.latin1().data(), 
        password.length());
}

void WebProcess::setHTTPProxy(bool enable, String server, int port, String userId, String password)
{
    NTF::Proxy::setHTTPProxy(
        enable,
        server.latin1().data(), 
        server.length(),
        port,
        userId.latin1().data(), 
        userId.length(),
        password.latin1().data(), 
        password.length());
}
#endif

} // namespace WebKit
