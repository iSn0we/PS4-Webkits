/*
 * Copyright (C) 2009, 2010, 2011 Apple Inc. All rights reserved.
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

#ifndef MediaPlayerPrivateManx_h
#define MediaPlayerPrivateManx_h

#if ENABLE(VIDEO)
#include "MediaPlayerPrivate.h"
#include "VideoLayerManx.h"
#if ENABLE(MEDIA_SOURCE)
#include "MediaSourcePrivateManx.h"
#endif

#include "Timer.h"
#include <manx/MediaPlayer.h>

namespace WebCore {

class MediaPlayerPrivateManx : public MediaPlayerPrivateInterface, public Manx::MediaPlayerClient {
public:
    static void registerMediaEngine(MediaEngineRegistrar);

    MediaPlayerPrivateManx(MediaPlayer*);
    ~MediaPlayerPrivateManx();

    void load(const String& url);
#if ENABLE(MEDIA_SOURCE)
    void load(const String&, MediaSourcePrivateClient*);
#endif
    void cancelLoad();

    void prepareToPlay();

    PlatformLayer* platformLayer() const;

    void play();
    void pause();

    bool supportsFullscreen() const;

    FloatSize naturalSize() const;

    bool hasVideo() const;
    bool hasAudio() const;

    void setVisible(bool);

    float duration() const;

    float currentTime() const;

    void seek(float time);

    bool seeking() const;

    void setRate(float);
    double rate() const;

    bool paused() const;

    void setVolume(float);

    bool supportsMuting() const;
    void setMuted(bool);

    MediaPlayer::NetworkState networkState() const;
    MediaPlayer::ReadyState readyState() const;

    float maxTimeSeekable() const;
    double minTimeSeekable() const;
    std::unique_ptr<PlatformTimeRanges> buffered() const;

    bool didLoadingProgress() const;

    void setSize(const IntSize&);

    void paint(GraphicsContext*, const FloatRect&);

    void setPreload(MediaPlayer::Preload);

    bool supportsAcceleratedRendering() const;
    void acceleratedRenderingStateChanged();

    MediaPlayer::MovieLoadType movieLoadType() const;

    bool hasSingleSecurityOrigin() const;

    void timerFired();
    uint32_t canvasHandle() const;
    void restoreReleasedResource();

    // platform player interface
    void mediaPlayerNetworkStateChanged();
    void mediaPlayerReadyStateChanged();
    void mediaPlayerVolumeChanged(float);
    void mediaPlayerMuteChanged(bool);
    void mediaPlayerTimeChanged();
    void mediaPlayerDurationChanged();
    void mediaPlayerRateChanged();
    void mediaPlayerPlaybackStateChanged();
    void mediaPlayerRepaint();
    void mediaPlayerSizeChanged();
    void mediaPlayerEngineUpdated();
    void mediaPlayerFirstVideoFrameAvailable();
    void mediaPlayerCharacteristicChanged();
    void mediaPlayerUpdateCanvasHandle(bool isCanvasMode, uint32_t canvasHandle);

    bool mediaPlayerIsPaused() const;
    bool mediaPlayerAcceleratedCompositingEnabled();
    bool mediaPlayerIndirectCompositingEnabled();
    bool mediaPlayerIsActive() const;
    bool mediaPlayerIsVisible() const;

private:
    // engine support
    static void getSupportedTypes(HashSet<String>& types);
    static MediaPlayer::SupportsType supportsType(const MediaEngineSupportParameters&);
    static bool isAvailable();

    const char* contentMIMEType();
    const char* contentTypeCodecs();
    const char* userAgent();

private:
    MediaPlayer* m_player;
    Manx::MediaPlayer* m_platform;
    Timer m_timer;

#if USE(TEXTURE_MAPPER) && USE(TEXTURE_MAPPER_GL)
    VideoLayerManx m_platformLayer;
#endif
};

}

#endif
#endif
