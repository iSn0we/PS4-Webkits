/*
 * Copyright (C) 2010 Apple Inc. All rights reserved.
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
#include "WebProcessPool.h"

#include "WebProcessCreationParameters.h"
#include "WebProcessMessages.h"
#include <manx/StoragePath.h>

using namespace WebCore;

namespace WebKit {

void WebProcessPool::setUserID(int userID)
{
    m_userID = userID;
}

void WebProcessPool::setWebProcessPath(const String& path)
{
    m_webProcessPath = path;
}

void WebProcessPool::setUserStorageDirectory(const String& userStorageDirectory)
{
    m_userStorageDirectory = userStorageDirectory;
}

void WebProcessPool::setSystemStorageDirectory(const String& systemStorageDirectory)
{
    m_systemStorageDirectory = systemStorageDirectory;
}

void WebProcessPool::platformInitializeWebProcess(WebProcessCreationParameters& parameters)
{
    parameters.userID = m_userID;
#if USE(NTF)
    parameters.diskCacheDirectory = diskCacheDirectory();
    parameters.cookieDatabaseDirectory = cookieDatabaseDirectory();
    parameters.cookieMode = m_cookieMode;
#endif
#if HAVE(ACCESSIBILITY)
    parameters.enableAccessibility = accessibilityEnabled();
#endif
}

void WebProcessPool::platformInvalidateContext()
{
}

String WebProcessPool::legacyPlatformDefaultLocalStorageDirectory()
{
    return String(Manx::StoragePath::localStorage());
}

String WebProcessPool::legacyPlatformDefaultIndexedDBDatabaseDirectory()
{
    return String(Manx::StoragePath::sqlDatabase());
}

String WebProcessPool::legacyPlatformDefaultWebSQLDatabaseDirectory()
{
    return String(Manx::StoragePath::sqlDatabase());
}

String WebProcessPool::platformDefaultIconDatabasePath() const
{
    return String(Manx::StoragePath::iconDatabase());
}

String WebProcessPool::legacyPlatformDefaultMediaKeysStorageDirectory()
{
    return String(Manx::StoragePath::sqlDatabase());
}

String WebProcessPool::legacyPlatformDefaultApplicationCacheDirectory()
{
    return String(Manx::StoragePath::appCache());
}

String WebProcessPool::legacyPlatformDefaultNetworkCacheDirectory()
{
    return String(Manx::StoragePath::diskCache());
}

#if USE(NTF)
String WebProcessPool::diskCacheDirectory()
{
    if (!configuration().diskCacheDirectory().isEmpty())
        return configuration().diskCacheDirectory();

    if (!m_userStorageDirectory.isEmpty())
        Manx::StoragePath::setUserStorageBaseDir(m_userStorageDirectory.utf8().data());

    return String(Manx::StoragePath::diskCache());
}

void WebProcessPool::setPrivilegedNetworkBandwidth(uint32_t mode)
{
    bool canSendMessage = true;

    for (size_t i = 0; i < m_processes.size(); ++i) {
        canSendMessage &= m_processes[i]->canSendMessage();
        if (m_processes[i]->canSendMessage())
            m_processes[i]->send(Messages::WebProcess::SetPrivilegedNetworkBandwidth(mode), 0);
    }

    if (!m_processes.size() || !canSendMessage)
        m_pendingMessageToSetPrivilegedNetworkBandwidth = mode;
}

void WebProcessPool::setWebSecurityFilter(int filterMode)
{
    bool canSendMessage = true;

    for (size_t i = 0; i < m_processes.size(); ++i) {
        canSendMessage &= m_processes[i]->canSendMessage();
        if (m_processes[i]->canSendMessage())
            m_processes[i]->send(Messages::WebProcess::SetWebSecurityFilter(filterMode), 0);
    }

    if (!m_processes.size() || !canSendMessage)
        m_pendingMessageToSetWebSecurityFilter = filterMode;
}

void WebProcessPool::setIFilterHTTPProxy(bool enable, const String& server, int port, const String& userId, const String& password)
{
    bool canSendMessage = true;

    for (size_t i = 0; i < m_processes.size(); ++i) {
        canSendMessage &= m_processes[i]->canSendMessage();
        if (m_processes[i]->canSendMessage())
            m_processes[i]->send(Messages::WebProcess::SetIFilterHTTPProxy(enable, server, port, userId, password), 0);
    }

    if (!m_processes.size() || !canSendMessage) {
        m_pendingMessageToSetIFilterHTTPProxyEnable = enable;
        m_pendingMessageToSetIFilterHTTPProxyServer = server;
        m_pendingMessageToSetIFilterHTTPProxyPort = port;
        m_pendingMessageToSetIFilterHTTPProxyUserId = userId;
        m_pendingMessageToSetIFilterHTTPProxyPassword = password;
    }
}

void WebProcessPool::setHTTPProxy(bool enable, const String& server, int port, const String& userId, const String& password)
{
    bool canSendMessage = true;

    for (size_t i = 0; i < m_processes.size(); ++i) {
        canSendMessage &= m_processes[i]->canSendMessage();
        if (m_processes[i]->canSendMessage())
            m_processes[i]->send(Messages::WebProcess::SetHTTPProxy(enable, server, port, userId, password), 0);
    }

    if (!m_processes.size() || !canSendMessage) {
        m_pendingMessageToSetHTTPProxyEnable = enable;
        m_pendingMessageToSetHTTPProxyServer = server;
        m_pendingMessageToSetHTTPProxyPort = port;
        m_pendingMessageToSetHTTPProxyUserId = userId;
        m_pendingMessageToSetHTTPProxyPassword = password;
    }
}

String WebProcessPool::cookieDatabaseDirectory() const
{
    if (!m_overrideCookieStorageDirectory.isEmpty())
        return m_overrideCookieStorageDirectory;

    if (!m_userStorageDirectory.isEmpty())
        Manx::StoragePath::setUserStorageBaseDir(m_userStorageDirectory.utf8().data());

    return String(Manx::StoragePath::cookieStorage());
}

void WebProcessPool::setCookieMode(int cookieMode)
{
    m_cookieMode = cookieMode;
}
#endif

#if HAVE(ACCESSIBILITY)
void WebProcessPool::enableAccessibility()
{
    m_accessibilityEnabled = true;
}
#endif

} // namespace WebKit
