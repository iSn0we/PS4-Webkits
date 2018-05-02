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

#if ENABLE(VIDEO)
#include "MediaPlayerPrivateManx.h"

#include "CachedResourceLoader.h"
#include "ContentType.h"
#include "FocusController.h"
#include "FrameView.h"
#include "GraphicsContext.h"
#include "HTMLMediaElement.h"
#include "HTMLNames.h"
#include "HTMLSourceElement.h"
#include "HTMLTrackElement.h"
#include "MainFrame.h"
#include "NotImplemented.h"
#include "Page.h"
#include "PageVisibilityState.h"
#include "PlatformContextCairo.h"
#include "RenderElement.h"
#include "Settings.h"

#if ENABLE(MEDIA_SOURCE)
#include "MediaSource.h"
#endif

#include <wtf/StdLibExtras.h>
#include <wtf/text/CString.h>

// #define TRACE_FUNCTION printf("[WebCore::MediaPlayerPrivateManx] %s\n", __FUNCTION__)
#define TRACE_FUNCTION

static const double timerInterval = 1.0 / 60.0;

namespace WebCore {

void MediaPlayerPrivateManx::registerMediaEngine(MediaEngineRegistrar registrar)
{
    TRACE_FUNCTION;
    registrar([](MediaPlayer* player) { return std::make_unique<MediaPlayerPrivateManx>(player); },
        getSupportedTypes, supportsType, 0, 0, 0, 0);
}

MediaPlayerPrivateManx::MediaPlayerPrivateManx(MediaPlayer* player)
    : m_player(player)
    , m_platform(0)
    , m_timer(*this, &MediaPlayerPrivateManx::timerFired)
{
    TRACE_FUNCTION;
    m_platform = Manx::MediaPlayer::create(this);

    m_timer.startRepeating(timerInterval);
}

MediaPlayerPrivateManx::~MediaPlayerPrivateManx()
{
    TRACE_FUNCTION;
    delete m_platform;
    m_timer.stop();
}

void MediaPlayerPrivateManx::load(const String& url)
{
    TRACE_FUNCTION;

    m_player->characteristicChanged();

    m_platform->setMuted(m_player->muted());
    m_platform->setVolume(m_player->volume());
    m_platform->load(url.utf8().data(), contentMIMEType(), contentTypeCodecs(), userAgent());
}

#if ENABLE(MEDIA_SOURCE)
void MediaPlayerPrivateManx::load(const String& url, MediaSourcePrivateClient* client)
{
    TRACE_FUNCTION;

    if (!client)
        return;

    m_player->characteristicChanged();

    MediaSourcePrivateManx* mediaSourcePrivate;
    mediaSourcePrivate = MediaSourcePrivateManx::create();
    mediaSourcePrivate->setClient(client);

    Manx::MediaSource* platformMediaSource = mediaSourcePrivate->platform();

    m_platform->setMuted(m_player->muted());
    m_platform->setVolume(m_player->volume());
    m_platform->load(url.utf8().data(), platformMediaSource);
}
#endif

void MediaPlayerPrivateManx::cancelLoad()
{
    TRACE_FUNCTION;
    m_platform->cancelLoad();
}

void MediaPlayerPrivateManx::prepareToPlay()
{
    TRACE_FUNCTION;
    m_platform->prepareToPlay();
}

PlatformLayer* MediaPlayerPrivateManx::platformLayer() const
{
    return (PlatformLayer*)&m_platformLayer;
}

void MediaPlayerPrivateManx::play()
{
    TRACE_FUNCTION;
    m_platform->play();
}

void MediaPlayerPrivateManx::pause()
{
    TRACE_FUNCTION;
    m_platform->pause();
}

bool MediaPlayerPrivateManx::supportsFullscreen() const
{
    return true;
}

FloatSize MediaPlayerPrivateManx::naturalSize() const
{
    TRACE_FUNCTION;
    int width = 0;
    int height = 0;
    m_platform->naturalSize(width, height);
    return FloatSize(width, height);
}

bool MediaPlayerPrivateManx::hasVideo() const
{
    TRACE_FUNCTION;
    return m_platform->hasVideo();
}

bool MediaPlayerPrivateManx::hasAudio() const
{
    TRACE_FUNCTION;
    return m_platform->hasAudio();
}

void MediaPlayerPrivateManx::setVisible(bool visible)
{
    TRACE_FUNCTION;
    notImplemented(); 
}

float MediaPlayerPrivateManx::duration() const
{
    TRACE_FUNCTION;
    return m_platform->duration();
}

float MediaPlayerPrivateManx::currentTime() const
{
    TRACE_FUNCTION;
    return m_platform->currentTime();
}

void MediaPlayerPrivateManx::seek(float time)
{
    TRACE_FUNCTION;
    m_platform->seek(time);
}

bool MediaPlayerPrivateManx::seeking() const
{
    TRACE_FUNCTION;
    return m_platform->seeking();
}

void MediaPlayerPrivateManx::setRate(float rate)
{
    TRACE_FUNCTION;
    m_platform->setRate(rate);
}

double MediaPlayerPrivateManx::rate() const
{
    TRACE_FUNCTION;
    return m_platform->rate();
}

bool MediaPlayerPrivateManx::paused() const
{
    TRACE_FUNCTION;
    return m_platform->paused();
}

void MediaPlayerPrivateManx::setVolume(float volume)
{
    TRACE_FUNCTION;
    m_platform->setVolume(volume);
}

bool MediaPlayerPrivateManx::supportsMuting() const
{
    TRACE_FUNCTION;
    return m_platform->supportsMuting();
}

void MediaPlayerPrivateManx::setMuted(bool muted)
{
    TRACE_FUNCTION;
    m_platform->setMuted(muted);
}

MediaPlayer::NetworkState MediaPlayerPrivateManx::networkState() const
{
    TRACE_FUNCTION;
    MediaPlayer::NetworkState networkState = MediaPlayer::Empty;
    switch (m_platform->networkState()) {
    case Manx::MediaPlayer::Empty:
        networkState = MediaPlayer::Empty;
        break;
    case Manx::MediaPlayer::Idle:
        networkState = MediaPlayer::Idle;
        break;
    case Manx::MediaPlayer::Loading:
        networkState = MediaPlayer::Loading;
        break;
    case Manx::MediaPlayer::Loaded:
        networkState = MediaPlayer::Loaded;
        break;
    case Manx::MediaPlayer::FormatError:
        networkState = MediaPlayer::FormatError;
        break;
    case Manx::MediaPlayer::NetworkError:
        networkState = MediaPlayer::NetworkError;
        break;
    case Manx::MediaPlayer::DecodeError:
        networkState = MediaPlayer::DecodeError;
        break;
    }
    return networkState;
}

MediaPlayer::ReadyState MediaPlayerPrivateManx::readyState() const
{
    TRACE_FUNCTION;
    MediaPlayer::ReadyState readyState = MediaPlayer::HaveNothing;
    switch (m_platform->readyState()) {
    case Manx::MediaPlayer::HaveNothing:
        readyState = MediaPlayer::HaveNothing;
        break;
    case Manx::MediaPlayer::HaveMetadata:
        readyState = MediaPlayer::HaveMetadata;
        break;
    case Manx::MediaPlayer::HaveCurrentData:
        readyState = MediaPlayer::HaveCurrentData;
        break;
    case Manx::MediaPlayer::HaveFutureData:
        readyState = MediaPlayer::HaveFutureData;
        break;
    case Manx::MediaPlayer::HaveEnoughData:
        readyState = MediaPlayer::HaveEnoughData;
        break;
    }
    return readyState;
}

float MediaPlayerPrivateManx::maxTimeSeekable() const
{
    TRACE_FUNCTION;
    return m_platform->maxTimeSeekable();
}

double MediaPlayerPrivateManx::minTimeSeekable() const
{
    TRACE_FUNCTION;
    return m_platform->minTimeSeekable();
}

std::unique_ptr<PlatformTimeRanges> MediaPlayerPrivateManx::buffered() const
{
    TRACE_FUNCTION;
    float start = 0.0;
    float end = 0.0;
    auto timeRanges = std::make_unique<PlatformTimeRanges>();

    m_platform->buffered(start, end);
    if (start >= 0.0 && end >= 0.0)
        timeRanges->add(MediaTime::createWithFloat(start), MediaTime::createWithFloat(end));
    return timeRanges;
}

bool MediaPlayerPrivateManx::didLoadingProgress() const
{
    TRACE_FUNCTION;
    return m_platform->didLoadingProgress();
}

void MediaPlayerPrivateManx::setSize(const IntSize& size)
{
    TRACE_FUNCTION;
    m_platform->setSize(size.width(), size.height());
}


    //
    // TODO:
    //
    // GraphicsContext contains not only the video element but also other elements that should be repaited.
    // However, the another rect parameter always points the position of video element in View.
    // We should move paintRect with some other offset value to get the actual rect of video element in Manx::MediaPlayer.
    //
    // paintRect.moveBy(...);
    //

void MediaPlayerPrivateManx::paint(GraphicsContext* context, const FloatRect& rect)
{
    TRACE_FUNCTION;

    if (context->paintingDisabled())
        return;

#if ENABLE(MANX_INDIRECT_COMPOSITING)
    WebCore::MediaPlayerClient& client = m_player->client();
    if (client.mediaPlayerRenderingCanBeAccelerated(m_player))
        return;

    Document* document = m_player->cachedResourceLoader()->document();
    if (!document)
        return;

    document->view()->invalidateRect(enclosingIntRect(rect));
    bool hasFullscreen = document->page()->mainFrame().document()->documentElement()->containsFullScreenElement();
    if (hasFullscreen) {
        // Search for the HTMLMediaElement which is bound to "m_player";
        HTMLMediaElement* element = nullptr;
        auto allMediaElements = HTMLMediaElement::allMediaElements();
        for (auto i : allMediaElements) {
            if (i->player() == m_player) {
                element = i;
                break;
            }
        }

        if (element) {
            bool fullscreen = false;

            if (element->isFullscreen())
                fullscreen = true;
            else {
                RenderObject* r = element->renderer();
                while (r) {
                    if (r->isRenderFullScreen()) {
                        fullscreen = true;
                        break;
                    }
                    r = r->parent();
                }
            }

            if (!fullscreen)
                return;
        }
    }

    uint32_t canvasHandle = m_platformLayer.canvasHandle();
    if (canvasHandle != sce::Canvas::kInvalidHandle)
        context->punchCanvasHole(canvasHandle, enclosingIntRect(rect));
#endif
}

void MediaPlayerPrivateManx::setPreload(MediaPlayer::Preload preload)
{
    TRACE_FUNCTION;

    Manx::MediaPlayer::Preload manxPreload = Manx::MediaPlayer::Auto;
    switch (preload) {
    case MediaPlayer::None:
        manxPreload = Manx::MediaPlayer::None;
        break;
    case MediaPlayer::MetaData:
        manxPreload = Manx::MediaPlayer::MetaData;
        break;
    case MediaPlayer::Auto:
        manxPreload = Manx::MediaPlayer::Auto;
        break;
    }
    m_platform->setPreload(manxPreload);
}

bool MediaPlayerPrivateManx::supportsAcceleratedRendering() const
{
    Document* document = m_player->cachedResourceLoader()->document();
    if (document && document->settings())
        return document->settings()->acceleratedCompositingEnabled();
    return false;
}

void MediaPlayerPrivateManx::acceleratedRenderingStateChanged()
{
    m_player->client().mediaPlayerRenderingModeChanged(m_player);
}

MediaPlayer::MovieLoadType MediaPlayerPrivateManx::movieLoadType() const
{
    TRACE_FUNCTION;

    MediaPlayer::MovieLoadType movieLoadType = MediaPlayer::Unknown;

    switch (m_platform->movieLoadType()) {
    case Manx::MediaPlayer::Unknown:
        movieLoadType = MediaPlayer::Unknown;
        break;
    case Manx::MediaPlayer::Download:
        movieLoadType = MediaPlayer::Download;
        break;
    case Manx::MediaPlayer::StoredStream:
        movieLoadType = MediaPlayer::StoredStream;
        break;
    case Manx::MediaPlayer::LiveStream:
        movieLoadType = MediaPlayer::LiveStream;
        break;
    }

    return movieLoadType;
}

bool MediaPlayerPrivateManx::hasSingleSecurityOrigin() const
{
    TRACE_FUNCTION;
    return true;
}

void MediaPlayerPrivateManx::timerFired()
{
    TRACE_FUNCTION;
    m_platform->update(timerInterval);
}

uint32_t MediaPlayerPrivateManx::canvasHandle() const
{
    return m_platformLayer.canvasHandle();
}

void MediaPlayerPrivateManx::restoreReleasedResource()
{
    m_platform->restoreReleasedResource();
}

void MediaPlayerPrivateManx::mediaPlayerNetworkStateChanged()
{
    TRACE_FUNCTION;
    m_player->networkStateChanged();
}

void MediaPlayerPrivateManx::mediaPlayerReadyStateChanged()
{
    TRACE_FUNCTION;
    m_player->readyStateChanged();
}

void MediaPlayerPrivateManx::mediaPlayerTimeChanged()
{
    TRACE_FUNCTION;
    m_player->timeChanged();
}

void MediaPlayerPrivateManx::mediaPlayerDurationChanged()
{
    TRACE_FUNCTION;
    m_player->durationChanged();
}

void MediaPlayerPrivateManx::mediaPlayerRateChanged()
{
    TRACE_FUNCTION;
    m_player->rateChanged();
}

void MediaPlayerPrivateManx::mediaPlayerPlaybackStateChanged()
{
    TRACE_FUNCTION;
    m_player->playbackStateChanged();
}

void MediaPlayerPrivateManx::mediaPlayerRepaint()
{
    TRACE_FUNCTION;
    m_player->repaint();
}

void MediaPlayerPrivateManx::mediaPlayerSizeChanged()
{
    TRACE_FUNCTION;
    m_player->sizeChanged();
}

void MediaPlayerPrivateManx::mediaPlayerEngineUpdated()
{
    TRACE_FUNCTION;
    m_player->client().mediaPlayerEngineUpdated(m_player);
}

void MediaPlayerPrivateManx::mediaPlayerFirstVideoFrameAvailable()
{
    TRACE_FUNCTION;
    m_player->firstVideoFrameAvailable();
}

void MediaPlayerPrivateManx::mediaPlayerCharacteristicChanged()
{
    TRACE_FUNCTION;
    m_player->characteristicChanged();
}

void MediaPlayerPrivateManx::mediaPlayerUpdateCanvasHandle(bool isCanvasMode, uint32_t canvasHandle)
{
#if USE(TEXTURE_MAPPER) && USE(TEXTURE_MAPPER_GL)
    m_platformLayer.updateCanvasHandle(isCanvasMode, canvasHandle);
#endif
}

bool MediaPlayerPrivateManx::mediaPlayerIsPaused() const
{
    TRACE_FUNCTION;
    return m_player->client().mediaPlayerIsPaused();
}

bool MediaPlayerPrivateManx::mediaPlayerAcceleratedCompositingEnabled()
{
    return supportsAcceleratedRendering();
}

bool MediaPlayerPrivateManx::mediaPlayerIndirectCompositingEnabled()
{
#if ENABLE(MANX_INDIRECT_COMPOSITING)
    Document* document = m_player->cachedResourceLoader()->document();
    if (document && document->settings())
        return document->settings()->isVideoAlwaysEnabled();
#endif
    return false;
}

bool MediaPlayerPrivateManx::mediaPlayerIsActive() const
{
    Document* document = m_player->cachedResourceLoader()->document();
    if (document && document->page())
        return document->page()->focusController().isActive();
    return false;
}

bool MediaPlayerPrivateManx::mediaPlayerIsVisible() const
{
    Document* document = m_player->cachedResourceLoader()->document();
    if (document && document->page())
        return (document->page()->visibilityState() == PageVisibilityStateVisible);
    return false;
}

static HashSet<String> mimeTypeCache()
{
    DEPRECATED_DEFINE_STATIC_LOCAL(HashSet<String>, cache, ());
    static bool typeListInitialized = false;

    if (typeListInitialized)
        return cache;

    std::list<const char *> supportsTypes;

    Manx::MediaPlayer::getSupportsTypes(&supportsTypes);

    for (std::list<const char*>::iterator it = supportsTypes.begin(); it != supportsTypes.end(); ++it)
        cache.add(String(*it));

    typeListInitialized = true;
    return cache;
}

void MediaPlayerPrivateManx::getSupportedTypes(HashSet<String>& types)
{
    TRACE_FUNCTION;
    types = mimeTypeCache();
}

MediaPlayer::SupportsType MediaPlayerPrivateManx::supportsType(const MediaEngineSupportParameters& parameters)
{
    TRACE_FUNCTION;

    String type = parameters.type;
    String codecs = parameters.codecs;
#if ENABLE(MEDIA_SOURCE)
    bool isMediaSource = parameters.isMediaSource;
#else
    bool isMediaSource = false;
#endif

    if (type.isNull() || type.isEmpty())
        return MediaPlayer::IsNotSupported;

    if (!type.startsWith("audio/") && !type.startsWith("video/") && !type.startsWith("application/"))
        return MediaPlayer::IsNotSupported;

    int codecCheck = Manx::MediaPlayer::supportsType(type.utf8().data(), codecs.ascii().data(), isMediaSource);
    if (codecCheck == Manx::MediaPlayer::PLAYABLE_STATUS_PROBABLY)
        return MediaPlayer::IsSupported;
    if (codecCheck == Manx::MediaPlayer::PLAYABLE_STATUS_MAYBE)
        return MediaPlayer::MayBeSupported;

    return MediaPlayer::IsNotSupported;
}

bool MediaPlayerPrivateManx::isAvailable()
{
    TRACE_FUNCTION;
    notImplemented();
    return true;
}

const char* MediaPlayerPrivateManx::contentMIMEType()
{
    return m_player->contentMIMEType().lower().utf8().data();
}

const char* MediaPlayerPrivateManx::contentTypeCodecs()
{
    return m_player->contentTypeCodecs().utf8().data();
}

const char* MediaPlayerPrivateManx::userAgent()
{
    return m_player->userAgent().utf8().data();
}

} // namespace WebCore

#endif // ENABLE(VIDEO)
