/*
 * Copyright (C) 2017 Sony Interactive Entertainment Inc.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "WebTile.h"

#if ENABLE(MANX_HTMLTILE)

#include "ShareableBitmap.h"
#include "UpdateInfo.h"
#include "WebTileManager.h"
#include <cairo/cairo.h>
#include <wtf/Vector.h>
#include <wtf/text/CString.h>

// #define WEBTILE_UPDATE_DEBUG
// #define WEBTILE_UPDATE_DUMP
// #define WEBTILE_INIT_WITH_PATTERN

using namespace WebCore;
using namespace WTF;

namespace {
const size_t ARGB32ByteSize = 4;
}

namespace WebKit {

class WebTileBackingStore {
    WTF_MAKE_NONCOPYABLE(WebTileBackingStore);
public:
    static std::unique_ptr<WebTileBackingStore> create(const IntSize& size)
    {
        return std::unique_ptr<WebTileBackingStore>(new WebTileBackingStore(size));
    }

    const unsigned char* surfaceData() const;
    size_t surfaceDataSize() const;

    void incorporateUpdate(const UpdateInfo&);

private:
    WebTileBackingStore(const IntSize&);

    Vector<unsigned char> m_surface;
    IntRect m_bounds;

#if defined(WEBTILE_UPDATE_DEBUG) || defined(WEBTILE_UPDATE_DUMP)
    size_t m_updateCounter;
#endif
};

WebTileBackingStore::WebTileBackingStore(const IntSize& size)
    : m_surface(ARGB32ByteSize * size.width() * size.height(), 0)
    , m_bounds(IntPoint(), size)
#if defined(WEBTILE_UPDATE_DEBUG) || defined(WEBTILE_UPDATE_DUMP)
    , m_updateCounter(0)
#endif
{
}

const unsigned char* WebTileBackingStore::surfaceData() const
{
    return m_surface.data();
}

size_t WebTileBackingStore::surfaceDataSize() const
{
    return m_surface.size();
}

void WebTileBackingStore::incorporateUpdate(const UpdateInfo& updateInfo)
{
    RefPtr<ShareableBitmap> bitmap = ShareableBitmap::create(updateInfo.bitmapHandle);
    if (!bitmap)
        return;

    RefPtr<cairo_surface_t> image = bitmap->createCairoSurface();

    if (cairo_image_surface_get_format(image.get()) != CAIRO_FORMAT_ARGB32)
        return;

#if defined(WEBTILE_UPDATE_DEBUG) || defined(WEBTILE_UPDATE_DUMP)
    ++m_updateCounter;
#endif

#ifdef WEBTILE_UPDATE_DEBUG
    printf("UpdateInfo %p %lu: [%d,%d,%d,%d] (%lu rect) - Bitmap size: %dx%d\n"
        , this
        , m_updateCounter
        , updateInfo.updateRectBounds.location().x()
        , updateInfo.updateRectBounds.location().y()
        , updateInfo.updateRectBounds.width()
        , updateInfo.updateRectBounds.height()
        , updateInfo.updateRects.size()
        , bitmap->size().width()
        , bitmap->size().height());
#endif
#ifdef WEBTILE_UPDATE_DUMP
    const String filename = String::format("htmltile-capture-%p-%lu.png", this, m_updateCounter);
    cairo_surface_write_to_png(image.get(), filename.latin1().data());
#endif

    const unsigned char* srcBase = cairo_image_surface_get_data(image.get());
    unsigned char* dstBase = m_surface.data();
    const int srcStride = cairo_image_surface_get_stride(image.get());

    // Paint all update rects.
    const IntPoint updateRectLocation = updateInfo.updateRectBounds.location();
    for (size_t i = 0; i < updateInfo.updateRects.size(); ++i) {
        const IntRect updateRect = updateInfo.updateRects[i];
        IntRect srcRect = updateRect;
        srcRect.move(-updateRectLocation.x(), -updateRectLocation.y());

        // Sanity checks
        if (!(bitmap->bounds().contains(srcRect) && m_bounds.contains(updateRect)))
            continue;

        // TODO benchmark various methods of performing an RGBA -> BGRA transfer
        for (int i = 0; i < srcRect.height(); ++i) {
            const unsigned char* src = srcBase + ((srcRect.y() + i) * srcStride) + (srcRect.x() * ARGB32ByteSize);
            const unsigned char* srcEnd = src + srcRect.width() * ARGB32ByteSize;
            unsigned char* dst = dstBase + ((updateRect.y() + i) * m_bounds.width() + updateRect.x()) * ARGB32ByteSize;
            unsigned char* dstEnd = dst + srcRect.width() * ARGB32ByteSize;
            while (src != srcEnd && dst != dstEnd) {
                // Romain: it's crude, but likely to be optimized well by the compiler (vectorization, SSE)

                dst[0] = src[2]; // B -> R
                dst[1] = src[1];
                dst[2] = src[0]; // R -> B
                dst[3] = src[3];

                src += ARGB32ByteSize;
                dst += ARGB32ByteSize;
            }
        }
    }
}

std::unique_ptr<WebTile> WebTile::create(int width, int height, WebTileManager* tileManager)
{
    return std::unique_ptr<WebTile>(new WebTile(width, height, tileManager));
}

WebTile::WebTile(int width, int height, WebTileManager* tileManager)
    : m_backingStore(WebTileBackingStore::create(IntSize(width, height)))
{
#ifdef WEBTILE_INIT_WITH_PATTERN
    // simple pattern to show something on screen
    unsigned char* imgData = const_cast<unsigned char*>(m_backingStore->surfaceData());
    for (int j = 0; j < height; ++j)
    for (int i = 0; i < width*4; i += 4) {
        const int p = j * width * 4 + i;
        imgData[p] = (i < width * 2 && j < height / 2) ? 128 : 0;
        imgData[p+1] = (i > width * 2 && j < height / 2) ? 128 : 0;
        imgData[p+2] = (i < width * 2 && j > height / 2) ? 128 : 0;
        imgData[p+3] = (i > width * 2 && j > height / 2) ? 0 : 128;
    }
#endif
}

WebTile::~WebTile()
{
}

const unsigned char* WebTile::tileData() const
{
    return m_backingStore->surfaceData();
}

size_t WebTile::tileDataSize() const
{
    return m_backingStore->surfaceDataSize();
}

void WebTile::tileUpdated(const UpdateInfo& updateInfo)
{
    ASSERT(m_backingStore);
    m_backingStore->incorporateUpdate(updateInfo);
}

} // namespace WebKit

#endif // ENABLE(MANX_HTMLTILE)
