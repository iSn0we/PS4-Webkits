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

#ifndef SocketStreamHandleNtf_h
#define SocketStreamHandleNtf_h

#include "SocketStreamHandle.h"
#include "URLRequestListenerNtf.h"

#include "ntf/SocketStreamJob.h"

namespace WebCore {

class SocketStreamHandleNtf : public SocketStreamHandle, public URLRequestListenerNtf {
public:
    SocketStreamHandleNtf(const URL&, SocketStreamHandleClient*);
    SocketStreamHandleNtf(int socketfd, SocketStreamHandleClient*);

    ~SocketStreamHandleNtf();

private:
    int platformSend(const char* data, int length);
    void platformClose();

    // Receive a response message
    bool didReceiveResponseMessage(NTF::URLResponseMessage*);

    bool didReceiveOpenMessage(NTF::SocketStreamOpenMessage*);
    bool didReceiveCloseMessage(NTF::SocketStreamCloseMessage*);
    bool didReceiveDisconnectMessage(NTF::SocketStreamDisconnectMessage*);
    bool didReceiveReadyToReceiveMessage(NTF::SocketStreamReadyToReceiveMessage*);
    bool didReceiveReadyToSendMessage(NTF::SocketStreamReadyToSendMessage*);
    bool didReceiveFailMessage(NTF::SocketStreamFailMessage*);

private:
    NTF::SocketStreamJob* m_socketStreamJob;
};

} // namespace WebCore

#endif // SocketStreamHandleNtf_h
