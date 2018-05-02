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

#ifndef RenderThemeManx_h
#define RenderThemeManx_h

#include "RenderTheme.h"

namespace WebCore {

class RenderThemeManx : public RenderTheme {
private:
    RenderThemeManx()
        : RenderTheme() { }
    virtual ~RenderThemeManx();

public:
    static PassRefPtr<RenderTheme> create();

#if ENABLE(VIDEO)
    virtual String mediaControlsStyleSheet() override;
    virtual String mediaControlsScript() override;
#endif

    static PassRefPtr<Image> loadThemeImage(const char*);
    static void paintThemeImage(GraphicsContext*, Image*, const IntRect&,
        ColorSpace styleColorSpace = ColorSpaceDeviceRGB);
    static void paintThemeImage3x3(GraphicsContext*, Image*, const FloatRect&,
        int left = 0, int right = 0, int top = 0, int bottom = 0,
        ColorSpace = ColorSpaceDeviceRGB);

    // A method asking if the theme is able to draw the focus ring.
    virtual bool supportsFocusRing(const RenderStyle&) const override;

    // A method asking if the theme's controls actually care about redrawing when hovered.
    virtual bool supportsHover(const RenderStyle&) const override;

    // System fonts and colors for CSS.
    virtual void updateCachedSystemFontDescription(CSSValueID, FontDescription&) const override;
    
    // Methods for each appearance value.
    virtual bool paintCheckbox(const RenderObject&, const PaintInfo&, const IntRect&) override;
    virtual void setCheckboxSize(RenderStyle&) const override;

    virtual bool paintRadio(const RenderObject&, const PaintInfo&, const IntRect&) override;
    virtual void setRadioSize(RenderStyle&) const override;

    virtual void adjustButtonStyle(StyleResolver&, RenderStyle&, Element*) const override;
    virtual bool paintButton(const RenderObject&, const PaintInfo&, const IntRect&) override;

    virtual void adjustMenuListStyle(StyleResolver&, RenderStyle&, Element*) const override;
    virtual bool paintMenuList(const RenderObject&, const PaintInfo&, const FloatRect&) override;

    virtual void adjustMenuListButtonStyle(StyleResolver&, RenderStyle&, Element*) const override;
    virtual bool paintMenuListButtonDecorations(const RenderObject&, const PaintInfo&, const FloatRect&) override;

    virtual bool paintTextArea(const RenderObject&, const PaintInfo&, const FloatRect&) override;

    virtual void adjustSliderThumbSize(RenderStyle&, Element*) const override;

    virtual int popupInternalPaddingLeft(RenderStyle&) const override;
    virtual int popupInternalPaddingRight(RenderStyle&) const override;
    virtual int popupInternalPaddingTop(RenderStyle&) const override;
    virtual int popupInternalPaddingBottom(RenderStyle&) const override;

    virtual Color platformFocusRingColor() const override;
};

}

#endif
