/*
 * Copyright (C) 2014 Apple Inc. All rights reserved.
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
#include "StorageNamespaceProvider.h"

#include "Document.h"
#include "StorageArea.h"
#include "StorageNamespace.h"

namespace WebCore {

// Suggested by the HTML5 spec.
unsigned localStorageDatabaseQuotaInBytes = 5 * 1024 * 1024;

StorageNamespaceProvider::StorageNamespaceProvider()
{
#if PLATFORM(MANX)
    const char* secureWebProcess = getenv("MANX_SECURE_WEB_PROCESS");
    bool isSecureWebProcess = secureWebProcess && !strcmp(secureWebProcess, "1");
    const char* webProcessHTMLTile = getenv("MANX_WEB_PROCESS_HTML_TILE");
    bool isWebProcessHTMLTile = webProcessHTMLTile && !strcmp(webProcessHTMLTile, "1");

    if (isWebProcessHTMLTile) {
        localStorageDatabaseQuotaInBytes = 150 * 1024 * 1024; // Expanded for supporting more tiles (bug 97756)
    } else if (isSecureWebProcess) {
        localStorageDatabaseQuotaInBytes = 100 * 1024 * 1024; // For secure contents, bug 120906
    } else {
        localStorageDatabaseQuotaInBytes = 20 * 1024 * 1024; // bug 43753
    }
#endif
}

StorageNamespaceProvider::~StorageNamespaceProvider()
{
    ASSERT(m_pages.isEmpty());
}

void StorageNamespaceProvider::addPage(Page& page)
{
    ASSERT(!m_pages.contains(&page));

    m_pages.add(&page);
}

void StorageNamespaceProvider::removePage(Page& page)
{
    ASSERT(m_pages.contains(&page));

    m_pages.remove(&page);
}

RefPtr<StorageArea> StorageNamespaceProvider::localStorageArea(Document& document)
{
    auto& storageNamespace = document.securityOrigin()->canAccessLocalStorage(document.topOrigin()) ? localStorageNamespace() : transientLocalStorageNamespace(*document.topOrigin());

    return storageNamespace.storageArea(document.securityOrigin());
}

StorageNamespace& StorageNamespaceProvider::localStorageNamespace()
{
    if (!m_localStorageNamespace)
        m_localStorageNamespace = createLocalStorageNamespace(localStorageDatabaseQuotaInBytes);

    return *m_localStorageNamespace;
}

StorageNamespace& StorageNamespaceProvider::transientLocalStorageNamespace(SecurityOrigin& securityOrigin)
{
    auto& slot = m_transientLocalStorageMap.add(&securityOrigin, nullptr).iterator->value;
    if (!slot)
        slot = createTransientLocalStorageNamespace(securityOrigin, localStorageDatabaseQuotaInBytes);

    return *slot;
}

}
