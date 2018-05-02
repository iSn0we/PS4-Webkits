/*
 * Copyright (C) 2017 Sony Computer Entertainment Inc.
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

#if USE(TEXTURE_MAPPER) && PLATFORM(MANX)
#include "TextureMapperInspector.h"

#include "BitmapTextureGL.h"
#include "CoordinatedBackingStore.h"
#include "CoordinatedGraphicsScene.h"
#include "DebugFontData.h"
#include <WebCore/TextureMapperGL.h>
#include <wtf/CurrentTime.h>
#include <wtf/NeverDestroyed.h>

// #include "GamepadList.h"
// #include "Gamepads.h"

using namespace WebCore;

namespace WebKit {

static void fit(FloatRect& rect, const FloatSize& size)
{
    if (size.width() * rect.height() > size.height() * rect.width()) {
        float newHeight = size.height() * rect.width() / size.width();
        rect.move(0, (rect.height() - newHeight) * 0.5);
        rect.setHeight(newHeight);
    } else if (size.width() * rect.height() < size.height() * rect.width()) {
        float newWidth = size.width() * rect.height() / size.height();
        rect.move((rect.width() - newWidth) * 0.5, 0);
        rect.setWidth(newWidth);
    }
}

class DebugFont {
public:
    DebugFont(TextureMapperGL* texmap)
    {
        // Initialize font data
        const uint32_t color = 0xFF00FF00;
        const uint32_t bgColor = 0x80000000;
        const IntSize size(DebugFontWidth, DebugFontHeight);
        m_fontSize = size;
        const int bytesPerLine = sizeof(uint32_t)* DebugFontWidth;
        for (unsigned char c = FirstDebugFontChar; c <= LastDebugFontChars; c++) {
            uint32_t data[DebugFontWidth * DebugFontHeight];
            for (int y = 0; y < DebugFontHeight; y++) {
                uint32_t* line = data + DebugFontWidth * y;
                unsigned char bits = DebugFontData[c - FirstDebugFontChar][y];
                for (int x = 0; x < DebugFontWidth; x++) {
                    line[x] = (bits & 0x80) ? color : bgColor;
                    bits <<= 1;
                }

            }

            m_asciiGlyphs[c] = texmap->createTexture();
            BitmapTextureGL* tex = reinterpret_cast<BitmapTextureGL*>(m_asciiGlyphs[c].get());
            if (!tex)
                return;
            tex->reset(size);
            tex->updateContents(data, IntRect(IntPoint(), size), IntPoint(0, 0), bytesPerLine, BitmapTexture::UpdateCannotModifyOriginalImageData);
        }
    }

    void drawString(TextureMapperGL* texmap, const char* str, const FloatPoint& targetPoint, float fontScale, const TransformationMatrix& modelViewMatrix)
    {
        FloatPoint point = targetPoint;
        FloatSize charSize = m_fontSize;
        charSize.scale(fontScale, fontScale);
        int len = strlen(str);
        for (int i = 0; i < len; i++) {
            unsigned char c = static_cast<unsigned char>(str[i]);
            BitmapTexture* tex = m_asciiGlyphs[c].get();
            if (tex)
                texmap->drawTexture(*tex, FloatRect(point, charSize), modelViewMatrix, 1, 0);

            point.moveBy(FloatPoint(charSize.width(), 0));
        }
    }

private:
    RefPtr<BitmapTexture> m_asciiGlyphs[256];
    FloatSize m_fontSize;
};

static DebugFont& debugFont(TextureMapperGL* texmap)
{
    static NeverDestroyed<DebugFont> debugFont(texmap);
    return debugFont;
}

TextureMapperInspector::TextureMapperInspector()
    : m_showLayerTable(false)
{
}

void TextureMapperInspector::paint(CoordinatedGraphicsScene* scene, TextureMapperGL* texmap, const TransformationMatrix& matrix, const FloatRect& clipRect)
{
    auto layer = scene->rootLayer();
    if (!layer)
        return;

    if (m_showLayerTable) {
        // Layer Table
        paintLayerTable(scene, texmap, matrix, clipRect);
    } else
        layer->paint();
}

void TextureMapperInspector::paintLayerTable(CoordinatedGraphicsScene* scene, TextureMapperGL* texmap, const TransformationMatrix& matrix, const FloatRect& clipRect)
{
    // Show the layers in a grid.
    int numLayers = scene->m_backingStores.size() + scene->m_imageBackings.size() + scene->m_surfaceBackingStores.size();
    int gridSize = (int)ceilf(sqrtf((float)numLayers));
    int i = 0;
    TransformationMatrix transform;
    Color layerBorderColor(255, 0, 0);
    Color imageBorderColor(0, 0, 255);
    Color surfaceBorderColor(0, 255, 255);
    Color backgroundColor(0, 64, 0);
    texmap->drawSolidColor(clipRect, transform, backgroundColor);
    for (auto it = scene->m_backingStores.begin(); it != scene->m_backingStores.end(); ++it) {
        FloatRect rect(i / gridSize, i % gridSize, 1, 1);
        rect.scale(1.0 / gridSize);
        rect.scale(clipRect.width(), clipRect.height());
        auto backingStore = it->value.get();
        FloatSize size = backingStore->size();
        fit(rect, size);
        backingStore->paintToTextureMapper(texmap, rect, transform, 1);
        backingStore->drawBorder(texmap, Color::lightGray, 1, rect, transform);
        rect.inflate(-1);
        texmap->drawBorder(layerBorderColor, 1, rect, transform);
        char buf[32];
        snprintf(buf, sizeof buf, "(%.1f, %.1f)", size.width(), size.height());
        debugFont(texmap).drawString(texmap, buf, rect.location(), 1, transform);
        i++;
    }
    for (auto it = scene->m_imageBackings.begin(); it != scene->m_imageBackings.end(); ++it) {
        FloatRect rect(i / gridSize, i % gridSize, 1, 1);
        rect.scale(1.0 / gridSize);
        rect.scale(clipRect.width(), clipRect.height());
        auto backingStore = it->value.get();
        FloatSize size = backingStore->size();
        fit(rect, size);
        backingStore->paintToTextureMapper(texmap, rect, transform, 1);
        rect.inflate(-1);
        texmap->drawBorder(imageBorderColor, 1, rect, transform);
        char buf[32];
        snprintf(buf, sizeof buf, "(%.1f, %.1f)", size.width(), size.height());
        debugFont(texmap).drawString(texmap, buf, rect.location(), 1, transform);
        i++;
    }
    for (auto it = scene->m_surfaceBackingStores.begin(); it != scene->m_surfaceBackingStores.end(); ++it) {
        FloatRect rect(i / gridSize, i % gridSize, 1, 1);
        rect.scale(1.0 / gridSize);
        rect.scale(clipRect.width(), clipRect.height());
        auto backingStore = it->value.get();
        backingStore->paintToTextureMapper(texmap, rect, transform, 1);
        rect.inflate(-1);
        texmap->drawBorder(surfaceBorderColor, 1, rect, transform);
        i++;
    }
}

} // namespace WebKit

#endif // USE(TEXTURE_MAPPER) && PLATFORM(MANX)
