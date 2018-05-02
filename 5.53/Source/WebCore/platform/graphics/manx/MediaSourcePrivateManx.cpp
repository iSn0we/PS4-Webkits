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
#include "MediaSourcePrivateManx.h"

#include "ContentType.h"
#include "MediaSourcePrivateClient.h"
#include "NotImplemented.h"
#include "SourceBufferPrivateManx.h"
#include <wtf/glib/GRefPtr.h>
#include <wtf/text/CString.h>
#include <wtf/text/WTFString.h>

// #define TRACE_FUNCTION printf("[WebCore::MediaPlayerPrivate] %s\n", __FUNCTION__)
#define TRACE_FUNCTION

namespace WebCore {

MediaSourcePrivateManx* MediaSourcePrivateManx::create()
{
    return new MediaSourcePrivateManx();
}

static const AtomicString& codecs()
{
    DEPRECATED_DEFINE_STATIC_LOCAL(const AtomicString, codecs, ("codecs", AtomicString::ConstructFromLiteral));
    return codecs;
}

MediaSourcePrivateManx::MediaSourcePrivateManx()
    : m_client(0)
    , m_platform(0)
    , m_readyState(MediaPlayer::HaveNothing)
{
    TRACE_FUNCTION;
    m_platform = Manx::MediaSource::create(this);
}

MediaSourcePrivateManx::~MediaSourcePrivateManx()
{
    TRACE_FUNCTION;
    delete m_platform;
}

void MediaSourcePrivateManx::setClient(MediaSourcePrivateClient* client)
{
    m_client = client;
}

MediaSourcePrivate::AddStatus MediaSourcePrivateManx::addSourceBuffer(const ContentType& contentType, RefPtr<SourceBufferPrivate>& sourceBufferPrivate)
{
    TRACE_FUNCTION;

    // Private pointer -> MediaSource module
    SourceBufferPrivateManx* sourceBufferPrivateManx = SourceBufferPrivateManx::create();
    sourceBufferPrivate = adoptRef<SourceBufferPrivateManx>(sourceBufferPrivateManx);

    Manx::SourceBuffer* platformSourceBuffer = sourceBufferPrivateManx->platform();

    // addSourceBuffer
    const char* contentMIMEType = contentType.type().lower().utf8().data();
    const char* contentTypeCodecs = contentType.parameter(codecs()).utf8().data();

    Manx::MediaSource::AddStatus addStatus = m_platform->addSourceBuffer(contentMIMEType, contentTypeCodecs, platformSourceBuffer);

    AddStatus status;
    switch (addStatus) {
    case Manx::MediaSource::Ok:
        status = Ok;
        break;
    case Manx::MediaSource::NotSupported:
        status = NotSupported;
        break;
    case Manx::MediaSource::ReachedIdLimit:
        status = ReachedIdLimit;
        break;
    }

    return Ok;
}

void MediaSourcePrivateManx::durationChanged()
{
    TRACE_FUNCTION;
    
    double duration = 0;
    if (m_client)
        duration = m_client->duration().toDouble();

    m_platform->setDuration(duration);
}

void MediaSourcePrivateManx::markEndOfStream(EndOfStreamStatus endOfStreamStatus)
{
    TRACE_FUNCTION;

    Manx::MediaSource::EndOfStreamStatus streamStatus;

    switch (endOfStreamStatus) {
    case EosNoError:
        streamStatus = Manx::MediaSource::EosNoError;
        break;
    case EosNetworkError:
        streamStatus = Manx::MediaSource::EosNetworkError;
        break;
    case EosDecodeError:
        streamStatus = Manx::MediaSource::EosDecodeError;
        break;
    }
    m_platform->endOfStream(streamStatus);
}

void MediaSourcePrivateManx::unmarkEndOfStream()
{
    TRACE_FUNCTION;
    notImplemented();
}

MediaPlayer::ReadyState MediaSourcePrivateManx::readyState() const
{
    TRACE_FUNCTION;
    return m_readyState;
}

void MediaSourcePrivateManx::setReadyState(MediaPlayer::ReadyState state)
{
    TRACE_FUNCTION;
    m_readyState = state;
}

void MediaSourcePrivateManx::waitForSeekCompleted()
{
    TRACE_FUNCTION;
    notImplemented();
}

void MediaSourcePrivateManx::seekCompleted()
{
    TRACE_FUNCTION;
    notImplemented();
}

void MediaSourcePrivateManx::mediaSourceOpen()
{
    TRACE_FUNCTION;
    if (m_client)
        m_client->setPrivateAndOpen(adoptRef(*this));
}

void MediaSourcePrivateManx::mediaSourceSeekToTime(float time)
{
    TRACE_FUNCTION;
    if (m_client)
        m_client->seekToTime(MediaTime::createWithFloat(time));
}
    
} // namespace WebCore

#endif // ENABLE(MEDIA_SOURCE)
