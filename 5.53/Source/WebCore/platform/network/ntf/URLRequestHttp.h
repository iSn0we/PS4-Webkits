/*
 * Copyright (C) 2017 Sony Interactive Entertainment Inc.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef URLRequestHttp_h
#define URLRequestHttp_h

#include "URLRequestListenerNtf.h"

namespace WebCore {

class ResourceRequest;
class ResourceHandleInternal;

// URLRequestHttpFactory ------------------------------------------------------

class URLRequestHttpFactory {
public:
    static NTF::URLRequestMessage* createRequestMessage(ResourceRequest&, ResourceHandleInternal*);
};



// URLRequestHttpListener -----------------------------------------------------

class URLRequestHttpListener : public URLRequestListenerNtf {
public:
    URLRequestHttpListener(ResourceHandle*);
    virtual ~URLRequestHttpListener();

private:
    // Receive a response message
    bool didReceiveResponseMessage(NTF::URLResponseMessage*);

    bool didReceiveResponseSendDataProgressMessage(NTF::URLRequestHttpSendDataProgressMessage*);
    bool didReceiveResponseHeaderFieldsMessage(NTF::URLResponseHttpHeaderFieldsMessage*);
    bool didReceiveResponseHeaderMessage(NTF::URLResponseHttpHeaderMessage*);
    bool didReceiveResponseBodyMessage(NTF::URLResponseHttpBodyMessage*);
    bool didReceiveResponseRedirectMessage(NTF::URLResponseHttpRedirectMessage*);
    bool didReceiveResponseFinishMessage(NTF::URLResponseHttpFinishMessage*);
    bool didReceiveResponseFailMessage(NTF::URLResponseHttpFailMessage*);
    bool didReceiveAuthenticationChallengeMessage(NTF::URLResponseHttpAuthenticationChallengeMessage*);
    bool didReceiveAuthenticationSuccessMessage(NTF::URLResponseHttpAuthenticationSuccessMessage*);
    bool didReceiveSslCertErrorMessage(NTF::URLResponseHttpSslCertErrorMessage*);
    bool didReceiveTimingInformationMessage(NTF::URLResponseHttpTimingInformationMessage*);
};

} // namespace WebCore

#endif // URLRequestHttp_h
