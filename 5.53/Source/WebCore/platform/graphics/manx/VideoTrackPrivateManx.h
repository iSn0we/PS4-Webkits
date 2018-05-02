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

#ifndef VideoTrackPrivateManx_h
#define VideoTrackPrivateManx_h

#if ENABLE(VIDEO_TRACK) && ENABLE(MEDIA_SOURCE)

#include "VideoTrackPrivate.h"
#include <manx/SourceBuffer.h>

namespace WebCore {

class VideoTrackPrivateManx : public VideoTrackPrivate {
public:
    static Ref<VideoTrackPrivateManx> create(Manx::SourceBufferClient::VideoTrackInfo& info)
    {
        return adoptRef(*new VideoTrackPrivateManx(info));
    }

    Kind kind() const;
    AtomicString id() const;
    AtomicString label() const;
    AtomicString language() const;
    int trackIndex() const;
    MediaTime startTimeVariance() const;

private:
    VideoTrackPrivateManx(Manx::SourceBufferClient::VideoTrackInfo&);
    ~VideoTrackPrivateManx();

    Kind convertToVideoTrackKind(Manx::SourceBufferClient::TrackKind videoTrackKind) const;

private:
    Manx::SourceBufferClient::VideoTrackInfo m_info;
};
}

#endif // ENABLE(VIDEO_TRACK) && ENABLE(MEDIA_SOURCE)

#endif // VideoTrackPrivateManx_h
