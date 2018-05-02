/*
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
#include "WebDiskCacheManager.h"

#include "WebProcess.h"
#include "WebProcessCreationParameters.h"

#if USE(NTF)
#include <ntf/DiskCacheUtility.h>
#endif

using namespace WebCore;

namespace WebKit {

const char* WebDiskCacheManager::supplementName()
{
    return "WebDiskCacheManager";
}

WebDiskCacheManager::WebDiskCacheManager(WebProcess* process)
    : m_process(process)
{
}

void WebDiskCacheManager::initialize(const WebProcessCreationParameters& parameters)
{
}

void WebDiskCacheManager::deleteAllDiskCaches(const String& pattern, const String& excludePattern) const
{
#if USE(NTF)
    NTF::Cache::DiskCacheUtility::deleteAll(
        pattern.isEmpty() ? 0 : pattern.utf8().data(),
        excludePattern.isEmpty() ? 0 : excludePattern.utf8().data());
#endif
}

}
