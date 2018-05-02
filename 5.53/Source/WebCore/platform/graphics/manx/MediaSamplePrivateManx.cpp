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
#include "MediaSamplePrivateManx.h"

namespace WebCore {

MediaSamplePrivateManx::MediaSamplePrivateManx(Manx::SourceBufferClient::MediaSampleInfo& info)
    : m_info(info)
    , m_offset(0.0)
{
}

MediaSamplePrivateManx::~MediaSamplePrivateManx()
{
}

MediaTime MediaSamplePrivateManx::presentationTime() const
{
    return MediaTime::createWithDouble(m_info.presentationTimestamp + m_offset);
}

MediaTime MediaSamplePrivateManx::decodeTime() const
{
    return MediaTime::createWithDouble(m_info.decodeTimestamp + m_offset);
}

MediaTime MediaSamplePrivateManx::duration() const
{
    return MediaTime::createWithDouble(m_info.duration);
}

AtomicString MediaSamplePrivateManx::trackID() const
{
    return AtomicString(m_info.trackId.c_str());
}

size_t MediaSamplePrivateManx::sizeInBytes() const
{
    return sizeof(MediaSamplePrivateManx);
}

FloatSize MediaSamplePrivateManx::presentationSize() const
{
    return FloatSize();
}

void MediaSamplePrivateManx::offsetTimestampsBy(const MediaTime& offset)
{
    m_offset = offset.toDouble();
}

MediaSample::SampleFlags MediaSamplePrivateManx::flags() const
{
    return m_info.isSync ? MediaSample::IsSync : MediaSample::None;
}

PlatformSample MediaSamplePrivateManx::platformSample()
{
    PlatformSample sample = { PlatformSample::None, { nullptr } };
    return sample;
}

void MediaSamplePrivateManx::dump(PrintStream&) const
{
}

} // namespace WebCore

#endif // ENABLE(MEDIA_SOURCE)
