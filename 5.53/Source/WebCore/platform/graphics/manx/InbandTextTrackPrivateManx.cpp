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
#include "config.h"

#if ENABLE(VIDEO_TRACK) && ENABLE(MEDIA_SOURCE)
#include "InbandTextTrackPrivateManx.h"

namespace WebCore {

InbandTextTrackPrivateManx::InbandTextTrackPrivateManx(CueFormat format, Manx::SourceBufferClient::TextTrackInfo& info)
    : InbandTextTrackPrivate(format)
    , m_info(info)
{
}

InbandTextTrackPrivateManx::~InbandTextTrackPrivateManx()
{
}

InbandTextTrackPrivate::Kind InbandTextTrackPrivateManx::kind() const
{
    return convertToInbandTextTrackKind(m_info.textTrackKind);
}

AtomicString InbandTextTrackPrivateManx::id() const
{
    return AtomicString(m_info.id.c_str());
}

AtomicString InbandTextTrackPrivateManx::label() const
{
    return AtomicString(m_info.label.c_str());
}

AtomicString InbandTextTrackPrivateManx::language() const
{
    return AtomicString(m_info.language.c_str());
}

MediaTime InbandTextTrackPrivateManx::startTimeVariance() const
{
    return MediaTime::createWithDouble(m_info.startTime);
}

AtomicString InbandTextTrackPrivateManx::inBandMetadataTrackDispatchType() const
{
    return AtomicString(m_info.inBandMetadataTrackDispatchType.c_str());
}

int InbandTextTrackPrivateManx::textTrackIndex() const
{
    return m_info.textTrackIndex;
}

InbandTextTrackPrivate::Kind InbandTextTrackPrivateManx::convertToInbandTextTrackKind(Manx::SourceBufferClient::TrackKind textTrackKind) const
{
    switch (textTrackKind) {
    case Manx::SourceBufferClient::None         : return None;
    case Manx::SourceBufferClient::Captions     : return Captions;
    case Manx::SourceBufferClient::Subtitles    : return Subtitles;
    case Manx::SourceBufferClient::Metadata     : return Metadata;
    default                                     : return None;
    }
}

} // namespace WebCore

#endif // ENABLE(VIDEO_TRACK) && ENABLE(MEDIA_SOURCE)
