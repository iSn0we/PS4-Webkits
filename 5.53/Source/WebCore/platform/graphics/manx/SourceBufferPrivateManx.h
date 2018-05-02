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
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */
#ifndef SourceBufferPrivateManx_h
#define SourceBufferPrivateManx_h

#if ENABLE(MEDIA_SOURCE)

#include "SourceBufferPrivate.h"
#include <manx/SourceBuffer.h>

namespace WebCore {

class SourceBufferPrivateManx : public SourceBufferPrivate, public Manx::SourceBufferClient {
public:
    static SourceBufferPrivateManx* create();

    SourceBufferPrivateManx();
    ~SourceBufferPrivateManx();

    void setClient(SourceBufferPrivateClient*);

    void append(const unsigned char* data, unsigned length);
    void abort();
    void removedFromMediaSource();

    MediaPlayer::ReadyState readyState() const;
    void setReadyState(MediaPlayer::ReadyState);

    Manx::SourceBuffer* platform() { return m_platform; }

    void timerFired();

    void sourceBufferDidReceiveInitializationSegment(InitializationSegmentInfo&);
    void sourceBufferDidReceiveSample(MediaSampleInfo&);
    void sourceBufferAppendComplete(bool isSucceeded);
    void sourceBufferDidReceiveRenderingError(int error);
    
private:
    SourceBufferPrivateClient* m_client;
    Manx::SourceBuffer* m_platform;
    MediaPlayer::ReadyState m_readyState;
    Timer m_timer;
};

}

#endif
#endif
