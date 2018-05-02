/*
 * Copyright (C) 2016 Sony Interactive Entertainment Inc.
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

#include "config.h"
#include "URLRequestJobFactory.h"

#include "ResourceHandle.h"
#include "ResourceRequest.h"
#include "URL.h"
#include "URLRequestData.h"
#include "URLRequestFile.h"
#include "URLRequestHttp.h"
#include "URLRequestUndefined.h"

namespace WebCore {

bool URLRequestJobFactory::create(ResourceHandle* resourceHandle, NTF::URLRequestJob** job, NTF::URLRequestListener** listener)
{
    ASSERT(resourceHandle && job && listener);

    ResourceRequest& resourceRequest = resourceHandle->firstRequest();
    ResourceHandleInternal* d = resourceHandle->getInternal();
    ASSERT(d);

    NTF::URLRequestMessage* message = 0;

    *job = 0;
    *listener = 0;

    URL requestURL = resourceHandle->firstRequest().url();
    if (requestURL.protocolIsInHTTPFamily()) {
        // HTTP
        message = URLRequestHttpFactory::createRequestMessage(resourceRequest, d);
        *listener = new URLRequestHttpListener(resourceHandle);
    } else if (requestURL.protocolIsData()) {
        // Data URI scheme
        message = URLRequestDataFactory::createRequestMessage(resourceRequest, d);
        *listener = new URLRequestDataListener(resourceHandle);
    } else if (requestURL.isLocalFile()) {
        // FILE
        message = URLRequestFileFactory::createRequestMessage(resourceRequest, d);
        *listener = new URLRequestFileListener(resourceHandle);
    } else {
        // UNDEFINED
        message = URLRequestUndefinedFactory::createRequestMessage(resourceRequest, d);
        *listener = new URLRequestUndefinedListener(resourceHandle);
    }

    if (!message || !(*listener)) {
        if (message) {
            NTF::URLRequestMessage::destruct(message);
            message = 0;
        }

        if (*listener) {
            delete *listener;
            *listener = 0;
        }

        return false;
    }

    *job = NTF::URLRequestJob::create(message, *listener);

    return true;
}

} // namespace WebCore
