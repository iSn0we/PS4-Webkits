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

#ifndef FontPlatformData_h
#define FontPlatformData_h

#include "FontDescription.h"
#include "GlyphBuffer.h"
#include "RefPtrCairo.h"
#include "TextFlags.h"
#include <cairo-ft.h>
#include <cairo.h>
#include <wtf/Forward.h>
#include <wtf/HashFunctions.h>

#if USE(HARFBUZZ)
#include "HarfBuzzFace.h"
#endif


namespace WebCore {

class FontPlatformData {
public:
    FontPlatformData(WTF::HashTableDeletedValueType)
        : m_size(0)
        , m_scaledFont(hashTableDeletedFontValue())
        { }

    FontPlatformData()
        : m_size(0)
        , m_scaledFont(0)
        { }

    FontPlatformData(const WTF::AtomicString& fontName, const FontDescription&, bool syntheticWeight, bool bold, bool oblique);
    FontPlatformData(cairo_font_face_t*, double size, bool bold, bool oblique);
    FontPlatformData(double size, bool bold, bool italic);
    FontPlatformData(const FontPlatformData&);

    ~FontPlatformData();

#if USE(HARFBUZZ)
    HarfBuzzFace* harfBuzzFace() const;
#endif

    bool isFixedPitch();
    double size() const { return m_size; }
    void setSize(double size) { m_size = size; }
    bool syntheticBold() const { return m_syntheticBold; }
    bool syntheticOblique() const { return m_syntheticOblique; }
    bool hasCompatibleCharmap();

    FontOrientation orientation() const { return Horizontal; } // FIXME: Implement.
    void setOrientation(FontOrientation) { } // FIXME: Implement.
    void setSyntheticOblique(bool syntheticOblique) { m_syntheticOblique = syntheticOblique; }
    
    cairo_scaled_font_t* scaledFont() const { return m_scaledFont; }

    unsigned hash() const
    {
        return PtrHash<cairo_scaled_font_t*>::hash(m_scaledFont);
    }

    bool operator==(const FontPlatformData&) const;
    FontPlatformData& operator=(const FontPlatformData&);
    bool isHashTableDeletedValue() const
    {
        return m_scaledFont == hashTableDeletedFontValue();
    }

    static bool init();

#ifndef NDEBUG
    String description() const;
#endif

    double m_size;
    // m_weight is stored to differentiate cache entries even if m_syntheticWeight is false
    FontWeight m_weight { FontWeight100 };
    bool m_syntheticWeight { false };
    bool m_syntheticBold { false };
    bool m_syntheticOblique { false };
    bool m_fixedWidth;
    cairo_scaled_font_t* m_scaledFont;
#if USE(HARFBUZZ)
    mutable RefPtr<HarfBuzzFace> m_harfBuzzFace;
#endif

private:
    void initializeWithFontFace(cairo_font_face_t*, const FontDescription& = FontDescription(), int fontWeightOffset = 0, double baselineOffset = 0);
    static cairo_scaled_font_t* hashTableDeletedFontValue() { return reinterpret_cast<cairo_scaled_font_t*>(-1); }

    static FT_Library s_ftLibrary;
    static cairo_user_data_key_t s_fontFaceUserDataKey;
};

}

#endif // FontPlatformData_h
