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
#include "config.h"

#if ENABLE(VIDEO_TRACK) && ENABLE(MEDIA_SOURCE)
#include "VideoTrackPrivateManx.h"

namespace WebCore {

VideoTrackPrivateManx::VideoTrackPrivateManx(Manx::SourceBufferClient::VideoTrackInfo& info)
    : m_info(info)
{
}

VideoTrackPrivateManx::~VideoTrackPrivateManx()
{
}

VideoTrackPrivate::Kind VideoTrackPrivateManx::kind() const
{
    return convertToVideoTrackKind(m_info.videoTrackKind);
}

AtomicString VideoTrackPrivateManx::id() const
{
    return AtomicString(m_info.id.c_str());
}

AtomicString VideoTrackPrivateManx::label() const
{
    return AtomicString(m_info.label.c_str());
}

AtomicString VideoTrackPrivateManx::language() const
{
    return AtomicString(m_info.language.c_str());
}

int VideoTrackPrivateManx::trackIndex() const
{
    return m_info.trackIndex;
}

MediaTime VideoTrackPrivateManx::startTimeVariance() const
{
    return MediaTime::createWithDouble(m_info.startTime);
}

VideoTrackPrivate::Kind VideoTrackPrivateManx::convertToVideoTrackKind(Manx::SourceBufferClient::TrackKind videoTrackKind) const
{
    switch (videoTrackKind) {
    case Manx::SourceBufferClient::None             : return None;
    case Manx::SourceBufferClient::Alternative      : return Alternative;
    case Manx::SourceBufferClient::Captions         : return Captions;
    case Manx::SourceBufferClient::Main             : return Main;
    case Manx::SourceBufferClient::Sign             : return Sign;
    case Manx::SourceBufferClient::Subtitles        : return Subtitles;
    case Manx::SourceBufferClient::Commentary       : return Commentary;
    default                                         : return None;
    }
}

} // namespace WebCore

#endif // ENABLE(VIDEO_TRACK) && ENABLE(MEDIA_SOURCE)
