/*
 * Copyright (C) 2015 Sony Interactive Entertainment Inc.
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
#include "URLRequestListenerNtf.h"

#include "ResourceHandle.h"
#include "SocketStreamHandle.h"
#include "WTF/MainThread.h"

namespace WebCore {

// URLRequestListenerNtf ------------------------------------------------------

URLRequestListenerNtf::URLRequestListenerNtf(ResourceHandle* resourceHandle)
    : m_resourceHandle(resourceHandle)
    , m_socketStreamHandle(0)
{
    // We call ref() for ResourceHandle not to be deleted
    // while ResourceHandle is being treated as void* in worker thread.
    // So we must call handle->deref() to decrement the ref count
    // when resource loading is completed.
    refHandle(this);
}

URLRequestListenerNtf::URLRequestListenerNtf(SocketStreamHandle* socketStreamHandle)
    : m_resourceHandle(0)
    , m_socketStreamHandle(socketStreamHandle)
{
    // We call ref() for SocketStreamHandle not to be deleted
    // while ResourceHandle is being treated as void* in worker thread.
    // So we must call handle->deref() to decrement the ref count
    // when resource loading is completed.
    refHandle(this);
}

URLRequestListenerNtf::~URLRequestListenerNtf()
{
    m_resourceHandle = 0;
    m_socketStreamHandle = 0;
}

void URLRequestListenerNtf::notifyResponseMessage(NTF::URLResponseMessage* message, bool isSyncRequest)
{
    ASSERT(message);

    // We call ref() for SocketStreamHandle not to be deleted.
    // Memo :
    //   We does not call ref() of ResourceHandleManager in this timing.
    //   Because ResourceHandleManager does not inherited ThreadSafeRefCounted.
    //   So, ref() of ResouceHandleManager is not thread safe.
    refSocketStreamHandleIfExists(this);

    // Pack to a MessageContainer
    MessageContainer* container = new MessageContainer(this, message);
    ASSERT(container);

    // Execute function on MainThread immediately.
    if (isSyncRequest)
        notifyResponseMessageCallback(container);
    else
        WTF::callOnMainThread(notifyResponseMessageCallback, container);
}

void URLRequestListenerNtf::notifyResponseMessageCallback(void* userdata)
{
    ASSERT(userdata);

    // Run on Main thread.
    ASSERT(isMainThread());

    // Unpack from a MessageContainer
    MessageContainer* container = static_cast<MessageContainer*>(userdata);
    ASSERT(container);

    URLRequestListenerNtf* listener = container->getRequestListener();
    ASSERT(listener);

    NTF::URLResponseMessage* responseMessage = container->getResponseMessage();
    ASSERT(responseMessage);

    delete container;
    container = 0;

    // Handle a response message
    bool canReleaseHandle = listener->didReceiveResponseMessage(responseMessage);

    NTF::URLResponseMessage::destruct(responseMessage);
    responseMessage = 0;

    // This is a pair of the refSocketStreamHandleIfExists() in notifyResponseMessage().
    derefSocketStreamHandleIfExists(listener);

    // This is a pair of the refHandle() in constructor.
    if (canReleaseHandle)
        derefHandle(listener);
}

ResourceHandle* URLRequestListenerNtf::getResourceHandle()
{
    return m_resourceHandle;
}

SocketStreamHandle* URLRequestListenerNtf::getSocketStreamHandle()
{
    return m_socketStreamHandle;
}

void URLRequestListenerNtf::refHandle(URLRequestListenerNtf* listener)
{
    if (!listener)
        return;

    ResourceHandle* resoruceHandle = listener->getResourceHandle();
    SocketStreamHandle* socketStreamHandle = listener->getSocketStreamHandle();

    if (resoruceHandle)
        resoruceHandle->ref();

    if (socketStreamHandle)
        socketStreamHandle->ref();
}

void URLRequestListenerNtf::derefHandle(URLRequestListenerNtf* listener)
{
    if (!listener)
        return;

    ResourceHandle* resoruceHandle = listener->getResourceHandle();
    SocketStreamHandle* socketStreamHandle = listener->getSocketStreamHandle();

    if (resoruceHandle)
        resoruceHandle->deref();

    if (socketStreamHandle)
        socketStreamHandle->deref();
}

void URLRequestListenerNtf::refSocketStreamHandleIfExists(URLRequestListenerNtf* listener)
{
    if (!listener)
        return;

    SocketStreamHandle* socketStreamHandle = listener->getSocketStreamHandle();
    if (socketStreamHandle)
        socketStreamHandle->ref();
}

void URLRequestListenerNtf::derefSocketStreamHandleIfExists(URLRequestListenerNtf* listener)
{
    if (!listener)
        return;

    SocketStreamHandle* socketStreamHandle = listener->getSocketStreamHandle();
    if (socketStreamHandle)
        socketStreamHandle->deref();
}



// URLRequestListenerNtf::MessageContainer -----------------------------------

URLRequestListenerNtf::MessageContainer::MessageContainer(URLRequestListenerNtf* requestListener, NTF::URLResponseMessage* responseMessage)
    : m_requestListener(requestListener)
    , m_responseMessage(responseMessage)
{

}

URLRequestListenerNtf::MessageContainer::~MessageContainer()
{
    m_requestListener = 0;
    m_responseMessage = 0;
}

inline URLRequestListenerNtf* URLRequestListenerNtf::MessageContainer::getRequestListener()
{
    return m_requestListener;
}

inline NTF::URLResponseMessage* URLRequestListenerNtf::MessageContainer::getResponseMessage()
{
    return m_responseMessage;
}

} // namespace WebCore
