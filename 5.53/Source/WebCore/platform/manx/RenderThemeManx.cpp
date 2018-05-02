/*
 * Copyright (C) 2008 Apple Computer, Inc.  All rights reserved.
 * Copyright (C) 2009 Google Inc.
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
#include "RenderThemeManx.h"

#include "BitmapImage.h"
#include "CSSValueKeywords.h"
#include "GraphicsContext.h"
#include "Image.h"
#include "NotImplemented.h"
#include "PaintInfo.h"
#include "SharedBuffer.h"
#include "UserAgentScripts.h"
#include "UserAgentStyleSheets.h"
#include <wtf/text/StringConcatenate.h>

#if ENABLE(VIDEO)
#include "HTMLMediaElement.h"
#include "RenderMediaControls.h"
#endif

namespace WebCore {

RenderThemeManx::~RenderThemeManx()
{
}

PassRefPtr<RenderTheme> RenderTheme::themeForPage(Page* page)
{
    static RenderTheme* rt = RenderThemeManx::create().leakRef();
    return rt;
}

PassRefPtr<RenderTheme> RenderThemeManx::create()
{
    return adoptRef(new RenderThemeManx());
}

#if ENABLE(VIDEO)
String RenderThemeManx::mediaControlsStyleSheet()
{
    return String(mediaControlsManxUserAgentStyleSheet, sizeof(mediaControlsManxUserAgentStyleSheet));
}

String RenderThemeManx::mediaControlsScript()
{
    StringBuilder scriptBuilder;
    scriptBuilder.append(mediaControlsLocalizedStringsJavaScript, sizeof(mediaControlsLocalizedStringsJavaScript));
    scriptBuilder.append(mediaControlsManxJavaScript, sizeof(mediaControlsManxJavaScript));
    return scriptBuilder.toString();
}
#endif

PassRefPtr<Image> RenderThemeManx::loadThemeImage(const char* name)
{
    static const char* path = getenv("WEBKIT_THEME_PATH");

    RefPtr<BitmapImage> img = BitmapImage::create();

    if (path) {
        RefPtr<SharedBuffer> buffer = SharedBuffer::createWithContentsOfFile(makeString(path, "/", name, ".png"));
        if (buffer)
            img->setData(buffer.release(), true);
    }

    return img.release();
}

void RenderThemeManx::paintThemeImage(GraphicsContext* ctx, Image* image, const IntRect& rect,
    ColorSpace styleColorSpace)
{
    ctx->drawImage(image, styleColorSpace, rect);
}

void RenderThemeManx::paintThemeImage3x3(GraphicsContext* ctx, Image* image, const FloatRect& rect,
    int left, int right, int top, int bottom,
    ColorSpace styleColorSpace)
{
    FloatRect dstRect = rect;
    FloatRect srcRect = image->rect();

    // top-left
    ctx->drawImage(image, styleColorSpace,
        IntRect(dstRect.x(), dstRect.y(), left, top),
        IntRect(srcRect.x(), srcRect.y(), left, top));
    // top-middle
    ctx->drawImage(image, styleColorSpace,
        IntRect(dstRect.x() + left, dstRect.y(), dstRect.width() - left - right, top),
        IntRect(srcRect.x() + left, srcRect.y(), srcRect.width() - left - right, top));
    // top-right
    ctx->drawImage(image, styleColorSpace,
        IntRect(dstRect.x() + dstRect.width() - right, dstRect.y(), right, top),
        IntRect(srcRect.x() + srcRect.width() - right, srcRect.y(), right, top));
    // middle-left
    ctx->drawImage(image, styleColorSpace,
        IntRect(dstRect.x(), dstRect.y() + top, left, dstRect.height() - top - bottom),
        IntRect(srcRect.x(), srcRect.y() + top, left, srcRect.height() - top - bottom));
    // middle-middle
    ctx->drawImage(image, styleColorSpace,
        IntRect(dstRect.x() + left, dstRect.y() + top, dstRect.width() - left - right, dstRect.height() - top - bottom),
        IntRect(srcRect.x() + left, srcRect.y() + top, srcRect.width() - left - right, srcRect.height() - top - bottom));
    // middle-right
    ctx->drawImage(image, styleColorSpace,
        IntRect(dstRect.x() + dstRect.width() - right, dstRect.y() + top, right, dstRect.height() - top - bottom),
        IntRect(srcRect.x() + srcRect.width() - right, srcRect.y() + top, right, srcRect.height() - top - bottom));
    // bottom-left
    ctx->drawImage(image, styleColorSpace,
        IntRect(dstRect.x(), dstRect.y() + dstRect.height() - bottom, left, bottom),
        IntRect(srcRect.x(), srcRect.y() + srcRect.height() - bottom, left, bottom));
    // bottom-middle
    ctx->drawImage(image, styleColorSpace,
        IntRect(dstRect.x() + left, dstRect.y() + dstRect.height() - bottom, dstRect.width() - left - right, bottom),
        IntRect(srcRect.x() + left, srcRect.y() + srcRect.height() - bottom, srcRect.width() - left - right, bottom));
    // bottom-right
    ctx->drawImage(image, styleColorSpace,
        IntRect(dstRect.x() + dstRect.width() - right, dstRect.y() + dstRect.height() - bottom, right, bottom),
        IntRect(srcRect.x() + srcRect.width() - right, srcRect.y() + srcRect.height() - bottom, right, bottom));
}

bool RenderThemeManx::supportsHover(const RenderStyle&) const
{
    return true;
}

bool RenderThemeManx::supportsFocusRing(const RenderStyle&) const
{
    return false;
}

void RenderThemeManx::updateCachedSystemFontDescription(CSSValueID propId, FontDescription& fontDescription) const
{
    float fontSize = 16;

    switch (propId) {
    case CSSValueWebkitMiniControl:
    case CSSValueWebkitSmallControl:
    case CSSValueWebkitControl:
        // Why 2 points smaller? Because that's what Gecko does. Note that we
        // are assuming a 96dpi screen, which is the default that we use on
        // Windows.
        static const float pointsPerInch = 72.0f;
        static const float pixelsPerInch = 96.0f;
        fontSize -= (2.0f / pointsPerInch) * pixelsPerInch;
        break;
    default:
        break;
    }

    fontDescription.setOneFamily("sans-serif");
    fontDescription.setSpecifiedSize(fontSize);
    fontDescription.setIsAbsoluteSize(true);
    fontDescription.setWeight(FontWeightNormal);
    fontDescription.setItalic(FontItalicOff);
}

bool RenderThemeManx::paintCheckbox(const RenderObject& obj, const PaintInfo& info, const IntRect& rect)
{
    static Image* const onNormal  = loadThemeImage("checkbox/checkbox_on_normal").leakRef();
    static Image* const onFocus   = loadThemeImage("checkbox/checkbox_on_focus").leakRef();
    static Image* const onHover   = loadThemeImage("checkbox/checkbox_on_hover").leakRef();
    static Image* const offNormal = loadThemeImage("checkbox/checkbox_off_normal").leakRef();
    static Image* const offFocus  = loadThemeImage("checkbox/checkbox_off_focus").leakRef();
    static Image* const offHover  = loadThemeImage("checkbox/checkbox_off_hover").leakRef();

    Image* image;

    if (this->isChecked(obj))
        image = isFocused(obj) ? onFocus : isHovered(obj) ? onHover : onNormal;
    else
        image = isFocused(obj) ? offFocus : isHovered(obj) ? offHover : offNormal;

    paintThemeImage(info.context, image, rect, obj.style().colorSpace());

    return false;
}

void RenderThemeManx::setCheckboxSize(RenderStyle& style) const
{
    // width and height both specified so do nothing
    if (!style.width().isIntrinsicOrAuto() && !style.height().isAuto())
        return;

    // hardcode size to 13 to match Firefox
    if (style.width().isIntrinsicOrAuto())
        style.setWidth(Length(13, Fixed));

    if (style.height().isAuto())
        style.setHeight(Length(13, Fixed));
}

bool RenderThemeManx::paintRadio(const RenderObject& obj, const PaintInfo& info, const IntRect& rect)
{
    static Image* const onNormal  = loadThemeImage("radio/radio_on_normal").leakRef();
    static Image* const onFocus   = loadThemeImage("radio/radio_on_focus").leakRef();
    static Image* const onHover   = loadThemeImage("radio/radio_on_hover").leakRef();
    static Image* const offNormal = loadThemeImage("radio/radio_off_normal").leakRef();
    static Image* const offFocus  = loadThemeImage("radio/radio_off_focus").leakRef();
    static Image* const offHover  = loadThemeImage("radio/radio_off_hover").leakRef();

    Image* image;

    if (isChecked(obj))
        image = isFocused(obj) ? onFocus : isHovered(obj) ? onHover : onNormal;
    else
        image = isFocused(obj) ? offFocus : isHovered(obj) ? offHover : offNormal;

    paintThemeImage(info.context, image, rect, obj.style().colorSpace());

    return false;
}

void RenderThemeManx::setRadioSize(RenderStyle& style) const
{
    // width and height both specified so do nothing
    if (!style.width().isIntrinsicOrAuto() && !style.height().isAuto())
        return;

    // hardcode size to 13 to match Firefox
    if (style.width().isIntrinsicOrAuto())
        style.setWidth(Length(13, Fixed));

    if (style.height().isAuto())
        style.setHeight(Length(13, Fixed));
}

void RenderThemeManx::adjustButtonStyle(StyleResolver&, RenderStyle& style, Element*) const
{
    // ignore line-height
    if (style.appearance() == PushButtonPart)
        style.setLineHeight(RenderStyle::initialLineHeight());
}

bool RenderThemeManx::paintButton(const RenderObject& obj, const PaintInfo& info, const IntRect& rect)
{
    static Image* const normal = loadThemeImage("button/button_normal").leakRef();
    static Image* const press  = loadThemeImage("button/button_press").leakRef();
    static Image* const focus  = loadThemeImage("button/button_focus").leakRef();
    static Image* const hover  = loadThemeImage("button/button_hover").leakRef();

    Image* image = isPressed(obj) ? press : isFocused(obj) ? focus : isHovered(obj) ? hover : normal;

    paintThemeImage3x3(info.context, image, rect, 6, 6, 6, 6, obj.style().colorSpace());

    return false;
}

void RenderThemeManx::adjustMenuListStyle(StyleResolver&, RenderStyle& style, Element*) const
{
    // The tests check explicitly that select menu buttons ignore line height.
    style.setLineHeight(RenderStyle::initialLineHeight());

    // We cannot give a proper rendering when border radius is active, unfortunately.
    style.resetBorderRadius();
}

bool RenderThemeManx::paintMenuList(const RenderObject& obj, const PaintInfo& info, const FloatRect& rect)
{
    static Image* const normal = loadThemeImage("menulist/menulist_normal").leakRef();
    static Image* const press  = loadThemeImage("menulist/menulist_press").leakRef();
    static Image* const focus  = loadThemeImage("menulist/menulist_focus").leakRef();
    static Image* const hover  = loadThemeImage("menulist/menulist_hover").leakRef();

    Image* image = isPressed(obj) ? press : isFocused(obj) ? focus : isHovered(obj) ? hover : normal;

    paintThemeImage3x3(info.context, image, rect, 6, 20, 6, 6, obj.style().colorSpace());

    return paintMenuListButtonDecorations(obj, info, rect);
}

void RenderThemeManx::adjustMenuListButtonStyle(StyleResolver& selector, RenderStyle& style, Element* element) const
{
    adjustMenuListStyle(selector, style, element);
}

bool RenderThemeManx::paintMenuListButtonDecorations(const RenderObject& o, const PaintInfo& paintInfo, const FloatRect& r)
{
    IntRect bounds = IntRect(r.x() + o.style().borderLeftWidth(),
        r.y() + o.style().borderTopWidth(),
        r.width() - o.style().borderLeftWidth() - o.style().borderRightWidth(),
        r.height() - o.style().borderTopWidth() - o.style().borderBottomWidth());
    const float arrowHeight = 5;
    const float arrowWidth = 7;
    const int arrowPaddingLeft = 5;
    const int arrowPaddingRight = 5;
    
    // Since we actually know the size of the control here, we restrict the font scale to make sure the arrow will fit vertically in the bounds
    float centerY = bounds.y() + bounds.height() / 2.0f;
    float leftEdge = bounds.maxX() - arrowPaddingRight - arrowWidth;
    float buttonWidth = arrowPaddingLeft + arrowWidth + arrowPaddingRight;

    if (bounds.width() < arrowWidth + arrowPaddingLeft)
        return false;

    GraphicsContext& context = *paintInfo.context;
    context.save();

    // Draw button background
    int fgGray = isPressed(o) ? 247 : isHovered(o) ? 84 : 94;
    int bgGray = isPressed(o) ? 84 : isHovered(o) ? 216 : 226;
    Color fgColor(fgGray, fgGray, fgGray);
    Color bgColor(bgGray, bgGray, bgGray);
    context.setStrokeStyle(SolidStroke);
    context.setStrokeColor(fgColor, ColorSpaceDeviceRGB);
    context.setFillColor(bgColor, ColorSpaceDeviceRGB);
    FloatRect button(bounds.maxX() - buttonWidth, bounds.y(), buttonWidth, bounds.height());
    button.inflate(-2);
    context.fillRect(button);
    context.strokeRect(button, 1);

    // Draw the arrow
    context.setFillColor(fgColor, ColorSpaceDeviceRGB);
    context.setStrokeColor(NoStroke, ColorSpaceDeviceRGB);

    FloatPoint arrow[3];
    arrow[0] = FloatPoint(leftEdge, centerY - arrowHeight / 2.0f);
    arrow[1] = FloatPoint(leftEdge + arrowWidth, centerY - arrowHeight / 2.0f);
    arrow[2] = FloatPoint(leftEdge + arrowWidth / 2.0f, centerY + arrowHeight / 2.0f);

    context.drawConvexPolygon(3, arrow, true);
    context.restore();
    return false;
}

bool RenderThemeManx::paintTextArea(const RenderObject& obj, const PaintInfo& info, const FloatRect& rect)
{
    return true;
}

int RenderThemeManx::popupInternalPaddingLeft(RenderStyle&) const
{
    return 5;
}

int RenderThemeManx::popupInternalPaddingRight(RenderStyle&) const
{
    return 5 + 20; // offset 20 for menulist button icon
}

int RenderThemeManx::popupInternalPaddingTop(RenderStyle&) const
{
    return 3;
}

int RenderThemeManx::popupInternalPaddingBottom(RenderStyle&) const
{
    return 3;
}

const int sliderThumbWidth = 7;
const int sliderThumbHeight = 15;

void RenderThemeManx::adjustSliderThumbSize(RenderStyle& style, Element*) const
{
    ControlPart part = style.appearance();
    switch (part) {
    case SliderThumbVerticalPart:
    case MediaVolumeSliderThumbPart:
        style.setWidth(Length(sliderThumbHeight, Fixed));
        style.setHeight(Length(sliderThumbWidth, Fixed));
        break;
    case SliderThumbHorizontalPart:
    case MediaSliderThumbPart:
        style.setWidth(Length(sliderThumbWidth, Fixed));
        style.setHeight(Length(sliderThumbHeight, Fixed));
        break;
    default:
        break;
    }
}

static const unsigned aquaFocusRingColor = 0xFF7DADD9;

Color RenderThemeManx::platformFocusRingColor() const
{
    return aquaFocusRingColor;
}

}
