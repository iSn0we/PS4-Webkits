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
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef InbandTextTrackPrivateManx_h
#define InbandTextTrackPrivateManx_h

#if ENABLE(VIDEO_TRACK) && ENABLE(MEDIA_SOURCE)

#include "InbandTextTrackPrivate.h"
#include <manx/SourceBuffer.h>

namespace WebCore {

class InbandTextTrackPrivateManx : public InbandTextTrackPrivate {
public:
    static Ref<InbandTextTrackPrivateManx> create(CueFormat format, Manx::SourceBufferClient::TextTrackInfo& info)
    {
        return adoptRef(*new InbandTextTrackPrivateManx(format, info));
    }

    Kind kind() const;
    AtomicString id() const;
    AtomicString label() const;
    AtomicString language() const;
    MediaTime startTimeVariance() const;
    AtomicString inBandMetadataTrackDispatchType() const;
    int textTrackIndex() const;

private:
    InbandTextTrackPrivateManx(CueFormat, Manx::SourceBufferClient::TextTrackInfo&);
    ~InbandTextTrackPrivateManx();

    Kind convertToInbandTextTrackKind(Manx::SourceBufferClient::TrackKind) const;

private:
    Manx::SourceBufferClient::TextTrackInfo m_info;
};

}

#endif // ENABLE(VIDEO_TRACK) && ENABLE(MEDIA_SOURCE)

#endif // InbandTextTrackPrivateManx_h
