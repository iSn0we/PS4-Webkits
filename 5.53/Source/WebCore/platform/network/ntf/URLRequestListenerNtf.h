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

#ifndef URLRequestListenerNtf_h
#define URLRequestListenerNtf_h

#include <ntf/URLRequestListener.h>
#include <ntf/URLRequestMessage.h>

namespace WebCore {

class ResourceHandle;
class SocketStreamHandle;

// URLRequestListenerNtf ------------------------------------------------------

class URLRequestListenerNtf : public NTF::URLRequestListener {
public:
    // Constructor/Destructor
    URLRequestListenerNtf(ResourceHandle*);
    URLRequestListenerNtf(SocketStreamHandle*);
    virtual ~URLRequestListenerNtf();

    // Notify a response message
    void notifyResponseMessage(NTF::URLResponseMessage*, bool);

protected:
    // Receive a response message
    virtual bool didReceiveResponseMessage(NTF::URLResponseMessage*) = 0;

    // Get a handle
    ResourceHandle* getResourceHandle();
    SocketStreamHandle* getSocketStreamHandle();

private:
    // URLRequestListenerNtf::MessageContainer -------------------------------
    class MessageContainer {
    public:
        MessageContainer(URLRequestListenerNtf*, NTF::URLResponseMessage*);
        virtual ~MessageContainer();

        URLRequestListenerNtf* getRequestListener();
        NTF::URLResponseMessage* getResponseMessage();

    private:
        URLRequestListenerNtf* m_requestListener;
        NTF::URLResponseMessage* m_responseMessage;
    };

private:
    // Callback function of notify a response message
    static void notifyResponseMessageCallback(void*);

    // Ref/Deref a count of a ResourceHandle or a SocketStreamHandle
    static void refHandle(URLRequestListenerNtf*);
    static void derefHandle(URLRequestListenerNtf*);

    // Ref/Deref a count of a a SocketStreamHandle
    static void refSocketStreamHandleIfExists(URLRequestListenerNtf*);
    static void derefSocketStreamHandleIfExists(URLRequestListenerNtf*);

private:
    ResourceHandle* m_resourceHandle;
    SocketStreamHandle* m_socketStreamHandle;
};

} // namespace WebCore

#endif // URLRequestListenerNtf_h
