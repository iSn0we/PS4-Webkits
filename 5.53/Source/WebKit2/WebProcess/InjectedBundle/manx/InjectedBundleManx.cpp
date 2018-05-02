/*
 * Copyright (C) 2010 Apple Inc. All rights reserved.
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

#include "config.h"
#include "InjectedBundle.h"

#include "InjectedBundleGCClient.h"
#include "Logging.h"
#include "WKBundleAPICast.h"
#include "WKBundleInitialize.h"
#include "WebCertificateInfo.h"
#include "WebDiskCacheManager.h"
#include "WebProcess.h"
#include <WebCore/JSDOMWindow.h>
#include <WebCore/NotImplemented.h>
#include <WebCore/ResourceHandle.h>
#include <wtf/FastMalloc.h>
#include <wtf/text/CString.h>

using namespace WebCore;
using namespace WTF;

#define WebKit2LogInjectedBundle WebKit2LogContextMenu // TODO: should be defined in Source/WebKit2/Platform/Logging.h

namespace WebKit {

bool InjectedBundle::initialize(const WebProcessCreationParameters& parameters, API::Object* initializationUserData)
{
    m_platformBundle = Manx::Bundle::create();
    if (!m_platformBundle) {
        LOG(InjectedBundle, "Manx::Bundle::create failed");
        return false;
    }
    bool ok = m_platformBundle->load(m_path.utf8().data());
    if (!ok) {
        LOG(InjectedBundle, "Manx::Bundle::load failed");
        return false;
    }
    WKBundleInitializeFunctionPtr initializeFunction = reinterpret_cast<WKBundleInitializeFunctionPtr>(m_platformBundle->resolve("WKBundleInitialize"));
    if (!initializeFunction) {
        LOG(InjectedBundle, "Manx::Bundle::resolve failed");
        return false;
    }        
    initializeFunction(toAPI(this), toAPI(initializationUserData));
    return true;
}

void InjectedBundle::setBundleParameter(const String& /*key*/, const IPC::DataReference& /*data*/)
{
}

void InjectedBundle::getFastMallocStatistics(size_t* usedBytes, size_t* reservedBytes)
{
    FastMallocStatistics stats = fastMallocStatistics();
    if (usedBytes)
        *usedBytes = stats.committedVMBytes;
    if (reservedBytes)
        *reservedBytes = stats.reservedVMBytes;
}

void InjectedBundle::javaScriptHeapStatistics(size_t* usedBytes, size_t* reservedBytes)
{
    JSC::VM& vm = JSDOMWindow::commonVM();

    if (usedBytes)
        *usedBytes = vm.heap.size();
    if (reservedBytes)
        *reservedBytes = vm.heap.capacity();
}

void InjectedBundle::clearAllDiskCaches(const String& pattern, const String& excludePattern)
{
    WebProcess::singleton().supplement<WebDiskCacheManager>()->deleteAllDiskCaches(pattern, excludePattern);
}

void InjectedBundle::initializeGCClient(WKBundleGarbageCollectClientBase* client)
{
    InjectedBundleGCClient* gcClient = new InjectedBundleGCClient(&JSDOMWindow::commonVM().heap);
    gcClient->initialize(client);
    JSDOMWindow::commonVM().heap.setFullActivityCallback(adoptRef(gcClient));
}

void InjectedBundle::shutdownProcess()
{
    RunLoop::main().stop();
}

} // namespace WebKit
