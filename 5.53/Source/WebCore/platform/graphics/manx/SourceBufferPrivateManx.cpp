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

#if ENABLE(MEDIA_SOURCE)
#include "SourceBufferPrivateManx.h"

#include "AudioTrackPrivateManx.h"
#include "InbandTextTrackPrivateManx.h"
#include "MediaDescriptionManx.h"
#include "MediaSamplePrivateManx.h"
#include "SourceBufferPrivateClient.h"
#include "VideoTrackPrivateManx.h"
#include <wtf/MediaTime.h>
#include <wtf/RefPtr.h>

// #define TRACE_FUNCTION printf("[WebCore::MediaPlayerPrivate] %s\n", __FUNCTION__)
#define TRACE_FUNCTION

static const double timerInterval = 1.0 / 60.0;

namespace WebCore {

SourceBufferPrivateManx* SourceBufferPrivateManx::create()
{
    return new SourceBufferPrivateManx();
}

SourceBufferPrivateManx::SourceBufferPrivateManx()
    : m_client(0)
    , m_platform(0)
    , m_readyState(MediaPlayer::HaveNothing)
    , m_timer(*this, &SourceBufferPrivateManx::timerFired)
{
    TRACE_FUNCTION;
    m_platform = Manx::SourceBuffer::create(this);
}

SourceBufferPrivateManx::~SourceBufferPrivateManx()
{
    TRACE_FUNCTION;
    delete m_platform;
    m_timer.stop();
}

void SourceBufferPrivateManx::setClient(SourceBufferPrivateClient* client)
{
    m_client = client;
}

void SourceBufferPrivateManx::append(const unsigned char* data, unsigned length)
{
    TRACE_FUNCTION;
    m_platform->append(data, length);

    if (m_platform->isUpdating())
        m_timer.startRepeating(timerInterval);
}

void SourceBufferPrivateManx::abort()
{
    TRACE_FUNCTION;
    m_platform->abort();
}

void SourceBufferPrivateManx::removedFromMediaSource()
{
    TRACE_FUNCTION;
    m_timer.stop();
    m_platform->removedFromMediaSource();
}

MediaPlayer::ReadyState SourceBufferPrivateManx::readyState() const
{
    TRACE_FUNCTION;
    return m_readyState;
}

void SourceBufferPrivateManx::setReadyState(MediaPlayer::ReadyState state)
{
    TRACE_FUNCTION;
    m_readyState = state;
}

void SourceBufferPrivateManx::timerFired()
{
    TRACE_FUNCTION;
    m_platform->updateAppendEvent();
}

void SourceBufferPrivateManx::sourceBufferDidReceiveInitializationSegment(InitializationSegmentInfo& info)
{
    TRACE_FUNCTION;
    SourceBufferPrivateClient::InitializationSegment initializationSegment;

    // duration
    initializationSegment.duration = MediaTime::createWithDouble(info.duration);

#if ENABLE(VIDEO_TRACK)
    // audioTracks
    for (std::list<AudioTrackInfo>::iterator it = info.audioTracks.begin(); it != info.audioTracks.end(); ++it) {
        SourceBufferPrivateClient::InitializationSegment::AudioTrackInformation trackInfo;

        trackInfo.description = MediaDescriptionManx::create(it->mediaDescription);
        trackInfo.track = AudioTrackPrivateManx::create(*it);
        initializationSegment.audioTracks.append(trackInfo);
    }

    // videoTracks
    for (std::list<VideoTrackInfo>::iterator it = info.videoTracks.begin(); it != info.videoTracks.end(); ++it) {
        SourceBufferPrivateClient::InitializationSegment::VideoTrackInformation trackInfo;

        trackInfo.description = MediaDescriptionManx::create(it->mediaDescription);
        trackInfo.track = VideoTrackPrivateManx::create(*it);
        initializationSegment.videoTracks.append(trackInfo);
    }

    // textTracks
    for (std::list<TextTrackInfo>::iterator it = info.textTracks.begin(); it != info.textTracks.end(); ++it) {
        SourceBufferPrivateClient::InitializationSegment::TextTrackInformation trackInfo;

        trackInfo.description = MediaDescriptionManx::create(it->mediaDescription);
        trackInfo.track = InbandTextTrackPrivateManx::create(InbandTextTrackPrivate::Data, *it);
    }
#endif

    if (m_client)
        m_client->sourceBufferPrivateDidReceiveInitializationSegment(this, initializationSegment);

}

void SourceBufferPrivateManx::sourceBufferDidReceiveSample(MediaSampleInfo& info)
{
    TRACE_FUNCTION;
    RefPtr<MediaSamplePrivateManx> mediaSample = MediaSamplePrivateManx::create(info);

    if (m_client)
        m_client->sourceBufferPrivateDidReceiveSample(this, mediaSample);
}

void SourceBufferPrivateManx::sourceBufferAppendComplete(bool isSucceeded)
{
    TRACE_FUNCTION;
    m_timer.stop();

    if (m_client) {
        SourceBufferPrivateClient::AppendResult result = isSucceeded ? SourceBufferPrivateClient::AppendSucceeded : SourceBufferPrivateClient::ParsingFailed;
        m_client->sourceBufferPrivateAppendComplete(this, result);
    }
}

void SourceBufferPrivateManx::sourceBufferDidReceiveRenderingError(int error)
{
    TRACE_FUNCTION;
    m_timer.stop();

    if (m_client)
        m_client->sourceBufferPrivateDidReceiveRenderingError(this, error);
}

} // namespace WebCore

#endif // ENABLE(MEDIA_SOURCE)
