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

#ifndef MediaDescriptionManx_h
#define MediaDescriptionManx_h

#if ENABLE(MEDIA_SOURCE)

#include "MediaDescription.h"
#include <manx/SourceBuffer.h>
#include <wtf/Ref.h>

namespace WebCore {

class MediaDescriptionManx : public MediaDescription {
public:
    static Ref<MediaDescriptionManx> create(Manx::SourceBufferClient::MediaDescription& description)
    {
        return adoptRef(*new MediaDescriptionManx(description));
    }

    AtomicString codec() const;
    bool isVideo() const;
    bool isAudio() const;
    bool isText() const;

private:
    MediaDescriptionManx(Manx::SourceBufferClient::MediaDescription&);
    ~MediaDescriptionManx();

private:
    Manx::SourceBufferClient::MediaDescription m_description;
};
}

#endif // ENABLE(MEDIA_SOURCE)

#endif // MediaDescriptionManx_h
