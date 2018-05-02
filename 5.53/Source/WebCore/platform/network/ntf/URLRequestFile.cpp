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
#include "URLRequestFile.h"

#include "HTTPParsers.h"
#include "MIMETypeRegistry.h"
#include "ResourceHandle.h"
#include "ResourceHandleClient.h"
#include "ResourceHandleInternal.h"
#include "ResourceRequest.h"

// #define LISTENER_TRACE_ON

#if defined(LISTENER_TRACE_ON)
#define TRACE(...)   { printf("[Listener 0x%p] ", this); printf(__VA_ARGS__); }
#else
#define TRACE(...) ((void)0)
#endif

namespace WebCore {

// URLRequestFileFactory ------------------------------------------------------

NTF::URLRequestMessage* URLRequestFileFactory::createRequestMessage(ResourceRequest& resourceRequest, ResourceHandleInternal* d)
{
    // Prepare request message

    bool isMainResource = resourceRequest.isMainResource();

    URL requestUrl = resourceRequest.url();
    String url = requestUrl.string();
    String httpMethod = resourceRequest.httpMethod();

    // Create RequestMessage

    NTF::URLRequestFileMessage* message = NTF::URLRequestFileMessage::create();
    ASSERT(message);

    message->setIsMainResource(isMainResource);
    message->setSchedulerId(1);
    // url is in ASCII so latin1() will only convert it to char* without character translation.
    message->setUrl(url.latin1().data());

    // Set response ------------------

    d->m_response.setIsMainResource(isMainResource);

    return static_cast<NTF::URLRequestMessage*>(message);
}



// URLRequestFileListener -----------------------------------------------------

URLRequestFileListener::URLRequestFileListener(ResourceHandle* resourceHandle)
    : URLRequestListenerNtf(resourceHandle)
{

}

URLRequestFileListener::~URLRequestFileListener()
{

}

bool URLRequestFileListener::didReceiveResponseMessage(NTF::URLResponseMessage* responseMessage)
{
    ASSERT(responseMessage);

    bool canReleaseHandle = false;

    switch (responseMessage->getMessageType()) {
    case NTF::URLResponseMessage::FileDetermineMimeType :
        canReleaseHandle = didReceiveDetermineMimeTypeMessage(static_cast<NTF::URLResponseFileDetermineMimeTypeMessage*>(responseMessage));
        break;

    case NTF::URLResponseMessage::FileResponseBody :
        canReleaseHandle = didReceiveResponseBodyMessage(static_cast<NTF::URLResponseFileBodyMessage*>(responseMessage));
        break;

    case NTF::URLResponseMessage::FileResponseFinish :
        canReleaseHandle = didReceiveResponseFinishMessage(static_cast<NTF::URLResponseFileFinishMessage*>(responseMessage));
        break;

    case NTF::URLResponseMessage::FileResponseFail :
        canReleaseHandle = didReceiveResponseFailMessage(static_cast<NTF::URLResponseFileFailMessage*>(responseMessage));
        break;
    default:
        ASSERT(false);
    }

    return canReleaseHandle;
}


// Determine MimeType -------------------------------------

bool URLRequestFileListener::didReceiveDetermineMimeTypeMessage(NTF::URLResponseFileDetermineMimeTypeMessage* message)
{
    TRACE("[IN] %s(%d)\n", __FUNCTION__, __LINE__);

    ResourceHandle* resourceHandle = getResourceHandle();
    ASSERT(resourceHandle);
    ResourceHandleInternal* d = resourceHandle->getInternal();
    ASSERT(d);
    ResourceHandleClient* client = d->client();

    if (d->m_cancelled)
        return false;

    String url(message->url());

    d->m_response.setURL(URL(URL(), url));
    d->m_response.setMimeType(MIMETypeRegistry::getMIMETypeForPath(url));

    if (client)
        client->didReceiveResponse(resourceHandle, d->m_response);

    return false;
}


// Response Body ------------------------------------------

bool URLRequestFileListener::didReceiveResponseBodyMessage(NTF::URLResponseFileBodyMessage* message)
{
    TRACE("[IN] %s(%d)\n", __FUNCTION__, __LINE__);

    ResourceHandle* resourceHandle = getResourceHandle();
    ASSERT(resourceHandle);
    ResourceHandleInternal* d = resourceHandle->getInternal();
    ASSERT(d);
    ResourceHandleClient* client = d->client();

    if (d->m_cancelled)
        return false;

    const char* data = static_cast<const char*>(message->data());
    int dataSize = static_cast<int>(message->dataSize());

    if (client)
        client->didReceiveData(resourceHandle, data, dataSize, dataSize);

    return false;
}


// Response Finish ----------------------------------------

bool URLRequestFileListener::didReceiveResponseFinishMessage(NTF::URLResponseFileFinishMessage* message)
{
    TRACE("[IN] %s(%d)\n", __FUNCTION__, __LINE__);

    ResourceHandle* resourceHandle = getResourceHandle();
    ASSERT(resourceHandle);
    ResourceHandleInternal* d = resourceHandle->getInternal();
    ASSERT(d);
    ResourceHandleClient* client = d->client();

    if (!d->m_cancelled) {
        if (client)
            client->didFinishLoading(resourceHandle, 0);
    }

    return true;
}


// Response Fail ------------------------------------------

bool URLRequestFileListener::didReceiveResponseFailMessage(NTF::URLResponseFileFailMessage* message)
{
    TRACE("[IN] %s(%d)\n", __FUNCTION__, __LINE__);

    ResourceHandle* resourceHandle = getResourceHandle();
    ASSERT(resourceHandle);
    ResourceHandleInternal* d = resourceHandle->getInternal();
    ASSERT(d);
    ResourceHandleClient* client = d->client();

    // Memo : In this case, we does not check the cancelled flag.
    //        We will notify the cancellation using the ResourceError.

    int errorCode = static_cast<int>(message->resultCode());
    String failingURL(message->url());
    String localizedDescription(message->resultString());

    ResourceError resourceError(String(), errorCode, failingURL, localizedDescription);

    if (message->isCancellation())
        resourceError.setIsCancellation(true);

    if (client)
        client->didFail(resourceHandle, resourceError);

    return true;
}


} // namespace WebCore
