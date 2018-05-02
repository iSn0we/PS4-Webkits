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
#include "URLRequestData.h"

#include "DataURL.h"
#include "ResourceHandle.h"
#include "ResourceHandleInternal.h"

// #define LISTENER_TRACE_ON

#if defined(LISTENER_TRACE_ON)
#define TRACE(...)   { printf("[Listener 0x%p] ", this); printf(__VA_ARGS__); }
#else
#define TRACE(...) ((void)0)
#endif

namespace WebCore {

// URLRequestDataFactory ------------------------------------------------------

NTF::URLRequestMessage* URLRequestDataFactory::createRequestMessage(ResourceRequest& resourceRequest, ResourceHandleInternal* d)
{
    // Prepare request message

    bool isMainResource = resourceRequest.isMainResource();

    URL requestUrl = resourceRequest.url();
    String url = requestUrl.string();

    // Create RequestMessage

    NTF::URLRequestDataMessage* message = NTF::URLRequestDataMessage::create();
    ASSERT(message);

    message->setIsMainResource(isMainResource);
    message->setSchedulerId(0);

    // Set response ------------------

    d->m_response.setIsMainResource(isMainResource);

    return static_cast<NTF::URLRequestMessage*>(message);
}



// URLRequestDataListener -----------------------------------------------------

URLRequestDataListener::URLRequestDataListener(ResourceHandle* resourceHandle)
    : URLRequestListenerNtf(resourceHandle)
{

}

URLRequestDataListener::~URLRequestDataListener()
{

}

bool URLRequestDataListener::didReceiveResponseMessage(NTF::URLResponseMessage* responseMessage)
{
    ASSERT(responseMessage);

    bool canReleaseHandle = false;

    switch (responseMessage->getMessageType()) {
    case NTF::URLResponseMessage::DataResponseFinish :
        canReleaseHandle = didReceiveResponseFinishMessage(static_cast<NTF::URLResponseDataFinishMessage*>(responseMessage));
        break;

    default:
        ASSERT(false);
    }

    return canReleaseHandle;
}


// Response Finish ----------------------------------------

bool URLRequestDataListener::didReceiveResponseFinishMessage(NTF::URLResponseDataFinishMessage* message)
{
    TRACE("[IN] %s(%d)\n", __FUNCTION__, __LINE__);

    ResourceHandle* resourceHandle = getResourceHandle();
    ASSERT(resourceHandle);
    ResourceHandleInternal* d = resourceHandle->getInternal();
    ASSERT(d);

    if (!d->m_cancelled)
        handleDataURL(resourceHandle);

    return true;
}

} // namespace WebCore
