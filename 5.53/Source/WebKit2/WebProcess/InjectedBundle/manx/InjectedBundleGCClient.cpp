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
 * THIS SOFTWARE IS PROVIDED BY SONY INTERACTIVE ENTERTAINMENT INC. ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL SONY INTERACTIVE ENTERTAINMENT INC.
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "InjectedBundleGCClient.h"

#include "InjectedBundle.h"
#include "WKBundleAPICast.h"
#include <wtf/CurrentTime.h>

using namespace WebKit;

InjectedBundleGCClient::InjectedBundleGCClient(JSC::Heap* heap)
    : JSC::FullGCActivityCallback(heap)
{
}

InjectedBundleGCClient::~InjectedBundleGCClient()
{
}

void InjectedBundleGCClient::willCollect()
{
    willGarbageCollect(0 /*not tracked*/, monotonicallyIncreasingTime());

    // call the base class impl
    JSC::FullGCActivityCallback::willCollect();
}

void InjectedBundleGCClient::willGarbageCollect(InjectedBundle* bundle, double timestamp)
{
    if (!m_client.willGarbageCollect)
        return;

    m_client.willGarbageCollect(toAPI(bundle), timestamp, m_client.base.clientInfo);
}
