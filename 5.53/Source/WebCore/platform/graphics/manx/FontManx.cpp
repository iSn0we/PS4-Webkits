/*
 * Copyright (C) 2008 Apple Computer, Inc.  All rights reserved.
 * Copyright (C) 2012 Sony Interactive Entertainment Inc.
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
#include "Font.h"

#include "CairoUtilities.h"
#include "FloatConversion.h"
#include "FontCascade.h"
#include "GlyphBuffer.h"
#include "GraphicsContext.h"
#include "IntRect.h"
#include "NotImplemented.h"
#include "PlatformContextCairo.h"
#include "ShadowBlur.h"
#include "UCharUChar32Iterator.h"
#include <cairo-ft.h>
#include <cairo.h>
#include <ft2build.h>
#include <unicode/normlzr.h>

#if USE(HARFBUZZ)
#include "HarfBuzzShaper.h"
#include "Logging.h"
#endif

#include FT_TRUETYPE_TABLES_H

namespace WebCore {

#if USE(HARFBUZZ)
float FontCascade::getGlyphsAndAdvancesForComplexText(const TextRun& run, int from, int to, GlyphBuffer& glyphBuffer, ForTextEmphasisOrNot /* forTextEmphasis */) const
{
    HarfBuzzShaper shaper(this, run);
    if (!shaper.shape(&glyphBuffer, from, to)) {
        LOG_ERROR("Shaper couldn't shape glyphBuffer.");
        return 0;
    }

    if (glyphBuffer.isEmpty())
        return 0;

    return shaper.selectionRect({ }, 0, from, to).x();
}

float FontCascade::drawComplexText(GraphicsContext* context, const TextRun& run, const FloatPoint& point, int from, int to) const
{
    // This glyph buffer holds our glyphs + advances + font data for each glyph.
    GlyphBuffer glyphBuffer;

    float startX = point.x() + getGlyphsAndAdvancesForComplexText(run, from, to, glyphBuffer);

    // We couldn't generate any glyphs for the run. Give up.
    if (glyphBuffer.isEmpty())
        return 0;

    // Draw the glyph buffer now at the starting point returned in startX.
    FloatPoint startPoint(startX, point.y());
    drawGlyphBuffer(context, run, glyphBuffer, startPoint);

    return startPoint.x() - startX;
}

void FontCascade::drawEmphasisMarksForComplexText(GraphicsContext* /* context */, const TextRun& /* run */, const AtomicString& /* mark */, const FloatPoint& /* point */, int /* from */, int /* to */) const
{
    notImplemented();
}

bool FontCascade::canReturnFallbackFontsForComplexText()
{
    return false;
}

bool FontCascade::canExpandAroundIdeographsInComplexText()
{
    return false;
}

float FontCascade::floatWidthForComplexText(const TextRun& run, HashSet<const Font*>*, GlyphOverflow*) const
{
    HarfBuzzShaper shaper(this, run);
    if (shaper.shape())
        return shaper.totalWidth();
    LOG_ERROR("Shaper couldn't shape text run.");
    return 0;
}

int FontCascade::offsetForPositionForComplexText(const TextRun& run, float x, bool) const
{
    HarfBuzzShaper shaper(this, run);
    if (shaper.shape())
        return shaper.offsetForPosition(x);
    LOG_ERROR("Shaper couldn't shape text run.");
    return 0;
}

void FontCascade::adjustSelectionRectForComplexText(const TextRun& run, LayoutRect& selectionRect, int from, int to) const
{
    HarfBuzzShaper shaper(this, run);
    if (shaper.shape()) {
        // FIXME: This should mimic Mac port.
        FloatRect rect = shaper.selectionRect(FloatPoint(selectionRect.location()), selectionRect.height().toInt(), from, to);
        selectionRect = LayoutRect(rect);
        return;
    }
    LOG_ERROR("Shaper couldn't shape text run.");
}

#else // USE(HARFBUZZ)

bool FontCascade::canReturnFallbackFontsForComplexText()
{
    return false;
}

bool FontCascade::canExpandAroundIdeographsInComplexText()
{
    return false;
}

void FontCascade::adjustSelectionRectForComplexText(const TextRun& run, const FloatPoint& pt, int h, int from, int to) const
{
    notImplemented();
}

float FontCascade::drawComplexText(GraphicsContext* context, const TextRun& run, const FloatPoint& point, int from, int to) const
{
    notImplemented();
    return 0;
}

void FontCascade::drawEmphasisMarksForComplexText(GraphicsContext* context, const TextRun& run, const AtomicString& mark, const FloatPoint& point, int from, int to) const
{
    notImplemented();
}

float FontCascade::floatWidthForComplexText(const TextRun& run, HashSet<const Font*>* fallbackFonts, GlyphOverflow* glyphOverflow) const
{
    notImplemented();
    return 0;
}

int FontCascade::offsetForPositionForComplexText(const TextRun& run, float position, bool) const
{
    notImplemented();
    return 0;
}
#endif // USE(HARFBUZZ)

static void drawGlyphsToContext(cairo_t* context, const Font* font, GlyphBufferGlyph* glyphs, int numGlyphs)
{
    cairo_set_scaled_font(context, font->platformData().scaledFont());
    cairo_show_glyphs(context, glyphs, numGlyphs);
}

static void drawGlyphsShadow(GraphicsContext* graphicsContext, const FloatPoint& point, const Font* font, GlyphBufferGlyph* glyphs, int numGlyphs)
{
    ShadowBlur& shadow = graphicsContext->platformContext()->shadowBlur();

    if (!(graphicsContext->textDrawingMode() & TextModeFill) || shadow.type() == ShadowBlur::NoShadow)
        return;

    if (!graphicsContext->mustUseShadowBlur()) {
        // Optimize non-blurry shadows, by just drawing text without the ShadowBlur.
        cairo_t* context = graphicsContext->platformContext()->cr();
        cairo_save(context);

        FloatSize shadowOffset(graphicsContext->state().shadowOffset);
        cairo_translate(context, shadowOffset.width(), shadowOffset.height());
        setSourceRGBAFromColor(context, graphicsContext->state().shadowColor);
        drawGlyphsToContext(context, font, glyphs, numGlyphs);

        cairo_restore(context);
        return;
    }

    cairo_text_extents_t extents;
    cairo_scaled_font_glyph_extents(font->platformData().scaledFont(), glyphs, numGlyphs, &extents);
    FloatRect fontExtentsRect(point.x() + extents.x_bearing, point.y() + extents.y_bearing, extents.width, extents.height);

    if (GraphicsContext* shadowContext = shadow.beginShadowLayer(graphicsContext, fontExtentsRect)) {
        drawGlyphsToContext(shadowContext->platformContext()->cr(), font, glyphs, numGlyphs);
        shadow.endShadowLayer(graphicsContext);
    }
}

void FontCascade::drawGlyphs(GraphicsContext* context, const Font* font, const GlyphBuffer& glyphBuffer,
    int from, int numGlyphs, const FloatPoint& point) const
{
    if (!font->platformData().size())
        return;

    GlyphBufferGlyph* glyphs = const_cast<GlyphBufferGlyph*>(glyphBuffer.glyphs(from));

    FloatPoint offset = point;
    for (int i = 0; i < numGlyphs; i++) {
        glyphs[i].x = offset.x();
        glyphs[i].y = offset.y();
        offset += glyphBuffer.advanceAt(from + i);
    }

    PlatformContextCairo* platformContext = context->platformContext();
    drawGlyphsShadow(context, point, font, glyphs, numGlyphs);

    cairo_t* cr = platformContext->cr();
    cairo_save(cr);

    if (context->textDrawingMode() & TextModeFill) {
        platformContext->prepareForFilling(context->state(), PlatformContextCairo::AdjustPatternForGlobalAlpha);
        drawGlyphsToContext(cr, font, glyphs, numGlyphs);
    }

    // Prevent running into a long computation within cairo. If the stroke width is
    // twice the size of the width of the text we will not ask cairo to stroke
    // the text as even one single stroke would cover the full wdth of the text.
    //  See https://bugs.webkit.org/show_bug.cgi?id=33759.
    if (context->textDrawingMode() & TextModeStroke && context->strokeThickness() < 2 * offset.x()) {
        platformContext->prepareForStroking(context->state());
        cairo_set_line_width(cr, context->strokeThickness());

        // This may disturb the CTM, but we are going to call cairo_restore soon after.
        cairo_set_scaled_font(cr, font->platformData().scaledFont());
        cairo_glyph_path(cr, glyphs, numGlyphs);
        cairo_stroke(cr);
    }

    cairo_restore(cr);
}

void Font::platformInit()
{
    if (!m_platformData.m_size)
        return;

    ASSERT(m_platformData.scaledFont());

    double ascent;
    double descent;
    double height;

#if !USE(HARFBUZZ)
    Font::setCodePath(Font::Simple); // FIXME: Implement Complex CodePath APIs.
#endif

    cairo_scaled_font_t* font = m_platformData.scaledFont();
    FT_Face face = cairo_ft_scaled_font_lock_face(font);
    TT_OS2* os2 = (TT_OS2*)FT_Get_Sfnt_Table(face, ft_sfnt_os2);
    if (os2) {
        // Use OS/2 table to keep compatibility with Silk porting.
        FT_Long winAscent = FT_RoundFix(FT_MulFix((os2->usWinAscent << 10), face->size->metrics.y_scale));
        FT_Long winDescent = FT_RoundFix(FT_MulFix((os2->usWinDescent << 10), face->size->metrics.y_scale));
        height = (winAscent + winDescent) >> 16;
        descent = winDescent >> 16;
        ascent = winAscent >> 16;
    } else {
        cairo_font_extents_t fontExtents;
        cairo_scaled_font_extents(m_platformData.scaledFont(), &fontExtents);

        ascent = fontExtents.ascent;
        descent = fontExtents.descent;
        height = fontExtents.height;
    }
    float lineGap = narrowPrecisionToFloat(height - ascent - descent);
    cairo_ft_scaled_font_unlock_face(font);

    m_fontMetrics.setAscent(ascent);
    m_fontMetrics.setDescent(descent);

    // Match CoreGraphics metrics.
    m_fontMetrics.setLineSpacing(lroundf(ascent) + lroundf(descent) + lroundf(lineGap));
    m_fontMetrics.setLineGap(lineGap);

    cairo_text_extents_t textExtents;
    cairo_scaled_font_text_extents(m_platformData.scaledFont(), "x", &textExtents);
    m_fontMetrics.setXHeight(narrowPrecisionToFloat(textExtents.height));

    cairo_scaled_font_text_extents(m_platformData.scaledFont(), " ", &textExtents);
    m_spaceWidth = narrowPrecisionToFloat(textExtents.x_advance);
    
    m_syntheticBoldOffset = m_platformData.syntheticBold() ? 1.0f : 0.0f;
}

void Font::platformCharWidthInit()
{
    m_avgCharWidth = 0.f;
    m_maxCharWidth = 0.f;
    initCharWidths();
}

void Font::platformDestroy()
{
}

PassRefPtr<Font> Font::platformCreateScaledFont(const FontDescription& fontDescription, float scaleFactor) const
{
    return Font::create(FontPlatformData(cairo_scaled_font_get_font_face(m_platformData.scaledFont()), scaleFactor * fontDescription.computedSize(),
        fontDescription.weight(), m_platformData.syntheticOblique()),
        isCustomFont(), false);
}

void Font::determinePitch()
{
    m_treatAsFixedPitch = m_platformData.isFixedPitch();
}

FloatRect Font::platformBoundsForGlyph(Glyph glyph) const
{
    if (!m_platformData.size())
        return FloatRect();

    // Calculate the bounds of the glyph. The scaled font (saved in m_platformData.scaledFont())
    // has information about the
    // transformations needed on font, like scaled size or application of oblique slant.
    // Bounds for glyph helps in calculation of glyph overflow for fonts
    // where the max glyph width (extents.x_bearing + extents.width) overshoots the
    // advance length, given by extents.x_advance.
    cairo_glyph_t cglyph = { glyph, 0, 0 };
    cairo_text_extents_t extents;
    cairo_scaled_font_glyph_extents(m_platformData.scaledFont(), &cglyph, 1, &extents);

    if (cairo_scaled_font_status(m_platformData.scaledFont()) == CAIRO_STATUS_SUCCESS)
        return FloatRect(extents.x_bearing, extents.y_bearing, extents.width, extents.height);
    return FloatRect();
}

float Font::platformWidthForGlyph(Glyph glyph) const
{
    ASSERT(m_platformData.scaledFont());

    cairo_glyph_t cglyph = { glyph, 0, 0 };
    cairo_text_extents_t extents;
    cairo_scaled_font_glyph_extents(m_platformData.scaledFont(), &cglyph, 1, &extents);

    float w = (float)m_spaceWidth;
    if (cairo_scaled_font_status(m_platformData.scaledFont()) == CAIRO_STATUS_SUCCESS && extents.x_advance)
        w = (float)extents.x_advance;

    return w;
}

#if USE(HARFBUZZ)
bool Font::canRenderCombiningCharacterSequence(const UChar* characters, size_t length) const
{
    if (!m_combiningCharacterSequenceSupport)
        m_combiningCharacterSequenceSupport = std::make_unique<HashMap<String, bool>>();

    WTF::HashMap<String, bool>::AddResult addResult = m_combiningCharacterSequenceSupport->add(String(characters, length), false);
    if (!addResult.isNewEntry)
        return addResult.iterator->value;

    UErrorCode error = U_ZERO_ERROR;
    Vector<UChar, 4> normalizedCharacters(length);
    int32_t normalizedLength = unorm_normalize(characters, length, UNORM_NFC, UNORM_UNICODE_3_2, &normalizedCharacters[0], length, &error);
    // Can't render if we have an error or no composition occurred.
    if (U_FAILURE(error) || (static_cast<size_t>(normalizedLength) == length))
        return false;

    FT_Face face = cairo_ft_scaled_font_lock_face(m_platformData.scaledFont());
    if (!face)
        return false;

    UTF16UChar32Iterator iterator(normalizedCharacters.data(), normalizedLength);
    if (FT_Get_Char_Index(face, iterator.next()))
        addResult.iterator->value = true;

    cairo_ft_scaled_font_unlock_face(m_platformData.scaledFont());
    return addResult.iterator->value;
}
#endif

}
