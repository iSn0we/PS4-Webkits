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
#include "SocketStreamHandleNtf.h"

#include "SocketStreamError.h"
#include "SocketStreamHandleClient.h"

#include <wtf/text/CString.h>


// #define LISTENER_TRACE_ON

#if defined(LISTENER_TRACE_ON)
#define TRACE(...)   { printf("[Listener 0x%p] ", this); printf(__VA_ARGS__); }
#else
#define TRACE(...) ((void)0)
#endif


namespace WebCore {

// SocketStreamHandle ---------------------------------------------------------

Ref<SocketStreamHandle> SocketStreamHandle::create(const URL& url, SocketStreamHandleClient* client, NetworkingContext&)
{
    return adoptRef(*new SocketStreamHandleNtf(url, client));
}

PassRefPtr<SocketStreamHandle> SocketStreamHandle::create(int socketFd, SocketStreamHandleClient* client)
{
    return adoptRef(new SocketStreamHandleNtf(socketFd, client));
}



// SocketStreamHandleNtf ------------------------------------------------------

SocketStreamHandleNtf::SocketStreamHandleNtf(const URL& url, SocketStreamHandleClient* client)
    : SocketStreamHandle(url, client)
    , URLRequestListenerNtf(this)
    , m_socketStreamJob(0)
{
    ASSERT(url.protocolIs("ws") || url.protocolIs("wss"));
    ASSERT(client);

    m_socketStreamJob = NTF::SocketStreamJob::create(url.string().latin1().data(), this);
}

SocketStreamHandleNtf::SocketStreamHandleNtf(int socketfd, SocketStreamHandleClient *client)
    : SocketStreamHandle(URL(), client)
    , URLRequestListenerNtf(this)
    , m_socketStreamJob(0)
{
    ASSERT(socketfd >= 0);
    ASSERT(client);

    m_socketStreamJob = NTF::SocketStreamJob::create(socketfd, this);
}

SocketStreamHandleNtf::~SocketStreamHandleNtf()
{
    if (m_socketStreamJob) {
        NTF::SocketStreamJob::destruct(m_socketStreamJob);
        m_socketStreamJob = 0;
    }
}



// SocketStreamHandle ---------------------------------------------------------

int SocketStreamHandleNtf::platformSend(const char* data, int length)
{
    int sendDataLength = -1;

    if (m_socketStreamJob)
        sendDataLength = m_socketStreamJob->streamSendData(data, length);

    return sendDataLength;
}

void SocketStreamHandleNtf::platformClose()
{
    if (m_socketStreamJob)
        m_socketStreamJob->streamClose();
}



// URLRequestListenerNtf ------------------------------------------------------

bool SocketStreamHandleNtf::didReceiveResponseMessage(NTF::URLResponseMessage* responseMessage)
{
    ASSERT(responseMessage);

    bool canReleaseHandle = false;

    switch (responseMessage->getMessageType()) {
    case NTF::URLResponseMessage::SocketStreamOpen:
        canReleaseHandle = didReceiveOpenMessage(static_cast<NTF::SocketStreamOpenMessage*>(responseMessage));
        break;

    case NTF::URLResponseMessage::SocketStreamClose:
        canReleaseHandle = didReceiveCloseMessage(static_cast<NTF::SocketStreamCloseMessage*>(responseMessage));
        break;

    case NTF::URLResponseMessage::SocketStreamDisconnect:
        canReleaseHandle = didReceiveDisconnectMessage(static_cast<NTF::SocketStreamDisconnectMessage*>(responseMessage));
        break;

    case NTF::URLResponseMessage::SocketStreamReadyToReceive:
        canReleaseHandle = didReceiveReadyToReceiveMessage(static_cast<NTF::SocketStreamReadyToReceiveMessage*>(responseMessage));
        break;

    case NTF::URLResponseMessage::SocketStreamReadyToSend:
        canReleaseHandle = didReceiveReadyToSendMessage(static_cast<NTF::SocketStreamReadyToSendMessage*>(responseMessage));
        break;

    case NTF::URLResponseMessage::SocketStreamFail:
        canReleaseHandle = didReceiveFailMessage(static_cast<NTF::SocketStreamFailMessage*>(responseMessage));
        break;

    default:
        ASSERT(false);
    }

    return canReleaseHandle;
}


// Open a socket ------------------------------------------

bool SocketStreamHandleNtf::didReceiveOpenMessage(NTF::SocketStreamOpenMessage* message)
{
    TRACE("[IN] %s(%d)\n", __FUNCTION__, __LINE__);

    m_state = SocketStreamHandle::Open;

    if (client())
        client()->didOpenSocketStream(this);

    return false;
}


// Close a socket -----------------------------------------

bool SocketStreamHandleNtf::didReceiveCloseMessage(NTF::SocketStreamCloseMessage* message)
{
    TRACE("[IN] %s(%d)\n", __FUNCTION__, __LINE__);

    if (client())
        client()->didCloseSocketStream(this);

    return true;
}


// Disconnect from server ---------------------------------

bool SocketStreamHandleNtf::didReceiveDisconnectMessage(NTF::SocketStreamDisconnectMessage* message)
{
    TRACE("[IN] %s(%d)\n", __FUNCTION__, __LINE__);

    platformClose();

    return false;
}


// Ready to receive ---------------------------------------

bool SocketStreamHandleNtf::didReceiveReadyToReceiveMessage(NTF::SocketStreamReadyToReceiveMessage* message)
{
    TRACE("[IN] %s(%d)\n", __FUNCTION__, __LINE__);

    bool result = false;
    char* buffer = 0;
    int dataSize = 0;

    if (m_socketStreamJob)
        result = m_socketStreamJob->streamReceiveData(&buffer, &dataSize);

    if (result) {
        if (client() && buffer)
            client()->didReceiveSocketStreamData(this, buffer, dataSize);
    } else
        platformClose();

    if (m_socketStreamJob)
        m_socketStreamJob->streamReleaseReceivedData(buffer);

    return false;
}


// Ready to send ------------------------------------------

bool SocketStreamHandleNtf::didReceiveReadyToSendMessage(NTF::SocketStreamReadyToSendMessage* message)
{
    TRACE("[IN] %s(%d)\n", __FUNCTION__, __LINE__);

    sendPendingData();

    return false;
}


// Connection fail ----------------------------------------

bool SocketStreamHandleNtf::didReceiveFailMessage(NTF::SocketStreamFailMessage* message)
{
    TRACE("[IN] %s(%d)\n", __FUNCTION__, __LINE__);

    int errorCode = message->errorCode();

    if (client())
        client()->didFailSocketStream(this, SocketStreamError(errorCode));

    return false;
}

} // namespace WebCore
