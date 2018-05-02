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
#include "FontPlatformData.h"

#include "FontDescription.h"
#include "GraphicsUtilManx.h"
#include <manx/FontData.h>
#include <wtf/HashMap.h>
#include <wtf/text/StringHash.h>

using namespace WTF;

namespace WebCore {

struct FontHandleCacheKey {
    WTF_MAKE_FAST_ALLOCATED;
public:
    FontHandleCacheKey(const AtomicString& family = AtomicString(), FontWeight weight = FontWeight100, bool bold = false, bool italic = false)
        : m_family(family)
        , m_weight(weight)
        , m_bold(bold)
        , m_italic(italic)
        , m_deleted(false)
    {
    }

    FontHandleCacheKey(HashTableDeletedValueType)
        : m_deleted(true) { }
    bool isHashTableDeletedValue() const { return m_deleted; }

    bool operator==(const FontHandleCacheKey& other) const
    {
        return equalIgnoringCase(m_family, other.m_family)
            && m_weight == other.m_weight
            && m_bold == other.m_bold
            && m_italic == other.m_italic
            && m_deleted == other.m_deleted;
    }

    AtomicString m_family;
    FontWeight m_weight;
    bool m_bold;
    bool m_italic;
    bool m_deleted;
};

inline unsigned computeHash(const FontHandleCacheKey& font)
{
    unsigned hashCodes[3] = {
        CaseFoldingHash::hash(font.m_family),
        (unsigned)font.m_weight,
        (unsigned)font.m_italic
    };
    return StringHasher::hashMemory<sizeof(hashCodes)>(hashCodes);
}

struct FontHandleCacheKeyHash {
    static unsigned hash(const FontHandleCacheKey& font)
    {
        return computeHash(font);
    }
    
    static bool equal(const FontHandleCacheKey& a, const FontHandleCacheKey& b)
    {
        return a == b;
    }

    static const bool safeToCompareToEmptyOrDeleted = true;
};

struct FontHandleCacheKeyTraits : SimpleClassHashTraits<FontHandleCacheKey> { };

typedef HashMap<FontHandleCacheKey, FT_Face, FontHandleCacheKeyHash, FontHandleCacheKeyTraits> FtFaceCache;
static FtFaceCache* gFtFaceCache = 0;

static bool createFtFace(const UCharFromString& fontName, FT_Library ftLibrary, FT_Face *ftFace)
{
    const char* fontPath = Manx::getFontPath(fontName.characters16(), fontName.length());
    if (!fontPath)
        fontPath = Manx::getDefaultFontPath();

    ASSERT(fontPath);
    if (!fontPath)
        return false;

    // Try to get preloaded font data.
    void* addr = 0;
    unsigned size = 0;
    if (!Manx::getFontData(fontPath, &addr, &size) && size > 0) {
        FT_Error error = FT_New_Memory_Face(ftLibrary, (FT_Byte*)addr, size, 0, ftFace);
        if (!error)
            return true;
    }

    FT_Error error = FT_New_Face(ftLibrary, fontPath, 0, ftFace);
    if (error)
        return false;

    return true;
}

static void finalizeFtFace(void *arg)
{
    FT_Face ftFace = reinterpret_cast<FT_Face>(arg);
    FtFaceCache::iterator it;
    for (it = gFtFaceCache->begin(); it != gFtFaceCache->end(); ++it) {
        if (it->value == ftFace) {
            gFtFaceCache->remove(it);
            break;
        }
    }
}

static void doneFtFace(void *arg)
{
    // This function is called on destroying cairo_font_face_t.
    // FT_Done_Face decrements the reference count of FT_Face and finalizeFtFace()
    // is called after the count gets to zero.
    FT_Done_Face(reinterpret_cast<FT_Face>(arg));
}

FT_Library FontPlatformData::s_ftLibrary = 0;
cairo_user_data_key_t FontPlatformData::s_fontFaceUserDataKey;

bool FontPlatformData::init()
{
    if (gFtFaceCache)
        delete gFtFaceCache;
    gFtFaceCache = new FtFaceCache;
    
    if (!s_ftLibrary) {
        FT_Error error = FT_Init_FreeType(&s_ftLibrary);
        if (error)
            return false;
    }

    return true;
}

FontPlatformData::FontPlatformData(const WTF::AtomicString& fontNameAS, const FontDescription& fontDescription, bool syntheticWeight, bool bold, bool oblique)
    : m_size(fontDescription.computedPixelSize())
    , m_weight(fontDescription.weight())
    , m_syntheticWeight(syntheticWeight)
    , m_syntheticBold(bold)
    , m_syntheticOblique(oblique)
    , m_fixedWidth(false)
    , m_scaledFont(0)
{
    UCharFromString fontName(fontNameAS);
    
    FT_Face ftFace = 0;
    FontHandleCacheKey key(fontName, m_weight, m_syntheticBold, m_syntheticOblique);
    FtFaceCache::iterator it = gFtFaceCache->find(key);
    if (it == gFtFaceCache->end()) {
        // Font face is not found in cache. Create new one.
        createFtFace(fontName, s_ftLibrary, &ftFace);
        if (!ftFace)
            return;

        ftFace->generic.data = gFtFaceCache;
        ftFace->generic.finalizer = finalizeFtFace;

        gFtFaceCache->set(key, ftFace);
    } else {
        ftFace = it->value;
        FT_Reference_Face(ftFace);
    }

    m_size *= Manx::getFontScale(fontName.characters16(), fontName.length());
    int fontWeightOffset = 0;
    fontWeightOffset = Manx::getFontWeightOffset(fontName.characters16(), fontName.length());

    double baselineOffset = Manx::getBaselineOffset(fontName.characters16(), fontName.length());

    cairo_font_face_t* fontFace = cairo_ft_font_face_create_for_ft_face(ftFace, 0);
    cairo_font_face_set_user_data(fontFace, &s_fontFaceUserDataKey, (void*)ftFace, doneFtFace);

    initializeWithFontFace(fontFace, fontDescription, fontWeightOffset, baselineOffset);

    // Decrement the reference counter of fontFace. If a new scaled font is successfully created
    // the counter is incremented and therefore fontFace is not destroyed here.
    cairo_font_face_destroy(fontFace);
}

FontPlatformData::FontPlatformData(double size, bool bold, bool italic)
    : m_size(size)
    , m_syntheticBold(bold)
    , m_syntheticOblique(italic)
    , m_fixedWidth(false)
    , m_scaledFont(0)
{
    // We cannot create a scaled font here.
}

FontPlatformData::FontPlatformData(cairo_font_face_t* fontFace, double size, bool bold, bool oblique)
    : m_size(size)
    , m_syntheticBold(bold)
    , m_syntheticOblique(oblique)
    , m_scaledFont(0)
{
    initializeWithFontFace(fontFace);

    FT_Face fontConfigFace = cairo_ft_scaled_font_lock_face(m_scaledFont);
    if (fontConfigFace) {
        m_fixedWidth = fontConfigFace->face_flags & FT_FACE_FLAG_FIXED_WIDTH;
        cairo_ft_scaled_font_unlock_face(m_scaledFont);
    }
}

FontPlatformData& FontPlatformData::operator=(const FontPlatformData& other)
{
    // Check for self-assignment.
    if (this == &other)
        return *this;

    m_size = other.m_size;
    m_weight = other.m_weight;
    m_syntheticWeight = other.m_syntheticWeight;
    m_syntheticBold = other.m_syntheticBold;
    m_syntheticOblique = other.m_syntheticOblique;
    m_fixedWidth = other.m_fixedWidth;

    if (m_scaledFont && m_scaledFont != hashTableDeletedFontValue())
        cairo_scaled_font_destroy(m_scaledFont);
    m_scaledFont = cairo_scaled_font_reference(other.m_scaledFont);

#if USE(HARFBUZZ)
    m_harfBuzzFace = other.m_harfBuzzFace;
#endif

    return *this;
}

FontPlatformData::FontPlatformData(const FontPlatformData& other)
    : m_scaledFont(0)
#if USE(HARFBUZZ)
    , m_harfBuzzFace(other.m_harfBuzzFace)
#endif
{
    *this = other;
}

FontPlatformData::~FontPlatformData()
{
    if (m_scaledFont && m_scaledFont != hashTableDeletedFontValue())
        cairo_scaled_font_destroy(m_scaledFont);
}

bool FontPlatformData::isFixedPitch()
{
    return m_fixedWidth;
}

bool FontPlatformData::operator==(const FontPlatformData& other) const
{
    return m_size == other.m_size
        && m_weight == other.m_weight
        && m_syntheticWeight == other.m_syntheticWeight
        && m_syntheticBold == other.m_syntheticBold
        && m_syntheticOblique == other.m_syntheticOblique
        && m_scaledFont == other.m_scaledFont;
}

#ifndef NDEBUG
String FontPlatformData::description() const
{
    return String();
}
#endif

void FontPlatformData::initializeWithFontFace(cairo_font_face_t* fontFace, const FontDescription& fontDescription, int fontWeightOffset, double baselineOffset)
{
    cairo_matrix_t fontMatrix;
    cairo_matrix_init_scale(&fontMatrix, m_size, m_size);
    cairo_matrix_translate(&fontMatrix, 0, -baselineOffset);
    cairo_matrix_t ctm;
    cairo_matrix_init_identity(&ctm);
    cairo_font_options_t* options = cairo_font_options_create();
    cairo_font_options_set_hint_style(options, CAIRO_HINT_STYLE_NONE);
    cairo_font_options_set_hint_metrics(options, CAIRO_HINT_METRICS_OFF);
    cairo_font_options_set_antialias(options, CAIRO_ANTIALIAS_GRAY);

    if (syntheticBold())
        cairo_ft_font_face_set_synthesize(fontFace, CAIRO_FT_SYNTHESIZE_BOLD);
    else
        cairo_ft_font_face_unset_synthesize(fontFace, CAIRO_FT_SYNTHESIZE_BOLD);

    int strength = 0;
    if (m_syntheticWeight) {
        switch (m_weight) {
        case FontWeight100:
        case FontWeight200:
        case FontWeight300:
            strength = -3;
            break;
        case FontWeight400:
            strength = 0;
            break;
        case FontWeight500:
            strength = 2;
            break;
        case FontWeight600:
            strength = 3;
            break;
        case FontWeight700:
        case FontWeight800:
        case FontWeight900:
            strength = 6;
            break;
        default:
            strength = 0;
            break;
        }
    }
    cairo_ft_font_face_reset_synthesize_weight(fontFace);
    cairo_ft_font_face_set_synthesize_weight(fontFace, strength + fontWeightOffset);
    
    // In case of synthetic oblique, we apply a transformation using cairo matrix to apply
    // slant to the text. The below code makes sure that the scaled font (m_scaledFont) object
    // is created with information about the oblique slant and hence the font-metrics correspond
    // to a slanted font and not a regular one.
    if (syntheticOblique()) {
        static const double syntheticObliqueSkew = -((double)0x03a / (double)0x100);
        cairo_matrix_t skew = { 1, 0, syntheticObliqueSkew, 1, 0, 0 };
        cairo_matrix_multiply(&fontMatrix, &skew, &fontMatrix);
    }

    m_scaledFont = cairo_scaled_font_create(fontFace, &fontMatrix, &ctm, options);
    cairo_font_options_destroy(options);    
    ASSERT(m_scaledFont);
}

bool FontPlatformData::hasCompatibleCharmap()
{
    if (!m_scaledFont)
        return false;

    FT_Face freeTypeFace = cairo_ft_scaled_font_lock_face(m_scaledFont);
    bool hasCompatibleCharmap = !(FT_Select_Charmap(freeTypeFace, ft_encoding_unicode)
        && FT_Select_Charmap(freeTypeFace, ft_encoding_symbol)
        && FT_Select_Charmap(freeTypeFace, ft_encoding_apple_roman));
    cairo_ft_scaled_font_unlock_face(m_scaledFont);
    return hasCompatibleCharmap;
}

#if USE(HARFBUZZ)
HarfBuzzFace* FontPlatformData::harfBuzzFace() const
{
    if (!m_harfBuzzFace)
        m_harfBuzzFace = HarfBuzzFace::create(const_cast<FontPlatformData*>(this), hash());

    return m_harfBuzzFace.get();
}
#endif

}
