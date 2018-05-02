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

#ifndef InjectedBundleGCClient_h
#define InjectedBundleGCClient_h

#include "APIClient.h"
#include "FullGCActivityCallback.h"
#include "WKBundleManx.h"

namespace API {
template<> struct ClientTraits<WKBundleGarbageCollectClientBase> {
    typedef std::tuple<WKBundleGarbageCollectClientV0> Versions;
};
}

namespace WebKit {

class InjectedBundle;

class InjectedBundleGCClient : public API::Client<WKBundleGarbageCollectClientBase>, public JSC::FullGCActivityCallback {
public:
    InjectedBundleGCClient(JSC::Heap*);
    ~InjectedBundleGCClient();
    virtual void willCollect();

private:
    void willGarbageCollect(InjectedBundle*, double timestamp);
};

} // namespace WebKit

#endif // InjectedBundleGCClient_h
