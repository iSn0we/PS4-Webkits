/*
 * Copyright (C) 2008 Apple Computer, Inc.  All rights reserved.
 * Copyright (c) 2006, 2007, 2008, 2009, 2012 Google Inc. All rights reserved.
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
#include "FontCache.h"

#include "Font.h"
#include "GraphicsUtilManx.h"
#include "UCharUChar32Iterator.h"
#include <manx/FontData.h>
#include <wtf/text/CString.h>

namespace WebCore {

static String calculateFontStyle(const FontDescription& fontDescription)
{
    String style = "";
    FontWeight weight = fontDescription.weight();
    if (weight < FontWeightNormal)
        style = "Light";
    else if (weight < FontWeight500)
        style = "";
    else if (weight < FontWeightBold)
        style = "Medium";
    else
        style = "Bold";

    if (fontDescription.italic() == FontItalicOn)
        style.append("Italic");

    return style;
}

static bool fontContainsCharacter(const FontPlatformData* fontData,
    UChar32 character)
{
    // FIXME: Cache charmap.
    cairo_scaled_font_t* scaledFont = fontData->scaledFont();
    ASSERT(scaledFont);
    FT_Face face = cairo_ft_scaled_font_lock_face(scaledFont);
    bool containsCharacter = FT_Get_Char_Index(face, character) > 0;
    cairo_ft_scaled_font_unlock_face(scaledFont);
    return containsCharacter;
}

void FontCache::platformInit()
{
    if (!FontPlatformData::init())
        ASSERT_NOT_REACHED();
}

RefPtr<Font> FontCache::systemFallbackForCharacters(const FontDescription& description, const Font* /* originalFontData */, bool, const UChar* characters, unsigned length)
{
    const FontDescription& fontDescription = description;
    const UCharFromString family(fontDescription.firstFamily());
    const wchar_t* genericFamily = Manx::getGenericFamily(family.characters16(), family.length());

    UTF16UChar32Iterator iterator(characters, length);
    UChar32 c = iterator.next();

    const wchar_t* const* platformFamilyList = Manx::getPlatformFamilyList(genericFamily, wcslen(genericFamily));

    FontPlatformData* data = 0;
    while (const wchar_t * platformFamily = *(platformFamilyList++)) {
        data = getCachedFontPlatformData(fontDescription, AtomicString(platformFamily));
        if (data && fontContainsCharacter(data, c))
            break;
    }

    if (data)
        return fontForPlatformData(*data);
    
    return 0;
}

Ref<Font> FontCache::lastResortFallbackFont(const FontDescription& fontDescription)
{
    // FIXME: Would be even better to somehow get the user's default font here.
    // For now we'll pick the default that the user would get without changing any prefs.
    static AtomicString timesStr("sans-serif");
    return *fontForFamily(fontDescription, timesStr);
}

void FontCache::getTraitsInFamily(const AtomicString& familyName, Vector<unsigned>& traitsMasks)
{
}

std::unique_ptr<FontPlatformData> FontCache::createFontPlatformData(const FontDescription& fontDescription, const AtomicString& familyAS)
{
    String fontStyle = calculateFontStyle(fontDescription);
    Manx::FontSynthesisHint hint = Manx::FontSynthesisNone;
    const UCharFromString family(familyAS);
    const wchar_t* fontName = Manx::getFontName(family.characters16(), family.length(), fontStyle.ascii().data(), hint);
    if (!fontName) {
        AtomicString genericFamily;

        if (!(equalIgnoringCase(family, "sans-serif") || equalIgnoringCase(family, "serif")
            || equalIgnoringCase(family, "monospace") || equalIgnoringCase(family, "fantasy")
            || equalIgnoringCase(family, "cursive") || equalIgnoringCase(family, "pictograph")))
            return 0;

        const wchar_t* const* platformFamilyList = Manx::getPlatformFamilyList(family.characters16(), family.length());
        const wchar_t* platformFamily = platformFamilyList[0];
        fontName = Manx::getFontName(platformFamily, wcslen(platformFamily), fontStyle.ascii().data(), hint);
    }

    bool syntheticWeight = hint & Manx::FontSynthesisBold;
    bool oblique = hint & Manx::FontSynthesisOblique;
    bool bold = hint & Manx::FontSynthesisBold && fontDescription.weight() >= FontWeightBold;

    auto platformData = std::make_unique<FontPlatformData>(AtomicString(fontName), fontDescription, syntheticWeight, bold, oblique);
    if (!platformData->hasCompatibleCharmap())
        return 0;

    return platformData;
}


}
