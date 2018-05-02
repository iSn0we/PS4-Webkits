/*
 * Copyright (C) 2012 Samsung Electronics. All rights reserved.
 * Copyright (C) 2013 Intel Corporation. All rights reserved.
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
#include "PageViewportControllerClientManx.h"

#include "ManxWebView.h"
#include "PageViewportController.h"

#define DBG_LOG(...)
// #define DBG_LOG(...) fprintf(stderr, __VA_ARGS__)

using namespace WebCore;

namespace WebKit {

PageViewportControllerClientManx::PageViewportControllerClientManx(WebView* webView)
    : m_webView(webView)
    , m_pageScaleFactor(1)
{
    ASSERT(m_webView);
}

void PageViewportControllerClientManx::didChangeContentsSize(const WebCore::IntSize& size)
{
    if (m_contentsSize != size) {
        m_contentsSize = size;
        FloatRect visibleContentsRect(m_webView->viewportController().boundContentsPosition(m_contentsPosition), m_webView->viewportController().visibleContentsSize());
        visibleContentsRect.intersect(FloatRect(FloatPoint::zero(), m_contentsSize));
        m_contentsPosition = visibleContentsRect.location();
        m_webView->didChangeContentsVisibility(m_contentsSize, m_contentsPosition, m_pageScaleFactor);
    }
}

void PageViewportControllerClientManx::setViewportPosition(const WebCore::FloatPoint& contentsPoint)
{
    DBG_LOG("PageViewportControllerClientManx::setViewportPosition (%.1f, %.1f)\n", contentsPoint.x(), contentsPoint.y());
    scrollTo(roundedIntPoint(contentsPoint));
}

void PageViewportControllerClientManx::setPageScaleFactor(float newScale)
{
    DBG_LOG("PageViewportControllerClientManx::setPageScaleFactor %f\n", newScale);
    if (m_pageScaleFactor != newScale) {
        m_pageScaleFactor = newScale;
        m_webView->postFakeMouseMove();
        m_webView->didChangeContentsVisibility(m_contentsSize, m_contentsPosition, m_pageScaleFactor);
    }
}

void PageViewportControllerClientManx::didChangeVisibleContents()
{
}

void PageViewportControllerClientManx::didChangeViewportAttributes()
{
}

void PageViewportControllerClientManx::scrollTo(const WebCore::FloatPoint& contentsPosition, const WebCore::FloatPoint& trajectoryVector)
{
    if (m_contentsPosition != contentsPosition) {
        m_contentsPosition = contentsPosition;
        m_webView->postFakeMouseMove();
        m_webView->didChangeContentsVisibility(m_contentsSize, m_contentsPosition, m_pageScaleFactor);
    }
    m_webView->viewportController().didChangeContentsVisibility(m_contentsPosition, m_pageScaleFactor, trajectoryVector);
}

void PageViewportControllerClientManx::scrollPageBy(const WebCore::FloatPoint& deltaInUIUnits)
{
    FloatPoint delta = deltaInUIUnits;
    delta.scale(1 / m_pageScaleFactor, 1 / m_pageScaleFactor);
    FloatPoint position = m_contentsPosition;
    position.moveBy(delta);
    FloatPoint newContentsPosition = roundedIntPoint(m_webView->viewportController().boundContentsPositionAtScale(position, m_pageScaleFactor));

    const float trajectoryVectorScale = 20.0;
    FloatPoint trajectoryVector = delta;
    trajectoryVector.scale(trajectoryVectorScale, trajectoryVectorScale);

    scrollTo(newContentsPosition, trajectoryVector);
}

void PageViewportControllerClientManx::scalePage(float scale)
{
    m_webView->viewportController().didChangeContentsVisibility(m_contentsPosition, scale);
}

void PageViewportControllerClientManx::applyTransformation(WebCore::TransformationMatrix& matrix) const
{
    matrix.scale(m_pageScaleFactor);
    matrix.translate(-m_contentsPosition.x(), -m_contentsPosition.y());
}

WebCore::FloatPoint PageViewportControllerClientManx::userViewportToContents(const WebCore::FloatPoint& point) const
{
    float scale = 1 / m_pageScaleFactor;
    FloatPoint contentsPoint = point;
    contentsPoint.scale(scale, scale);
    contentsPoint.moveBy(m_contentsPosition);
    return contentsPoint;
}

WebCore::IntPoint PageViewportControllerClientManx::userViewportToContents(const WebCore::IntPoint& point) const
{
    return roundedIntPoint(userViewportToContents(FloatPoint(point)));
}

WebCore::FloatPoint PageViewportControllerClientManx::contentsToUserViewport(const WebCore::FloatPoint& point) const
{
    float scale = m_pageScaleFactor;
    FloatPoint userViewportPoint = point;
    userViewportPoint.moveBy(-m_contentsPosition);
    userViewportPoint.scale(scale, scale);
    return userViewportPoint;
}

WebCore::IntPoint PageViewportControllerClientManx::contentsToUserViewport(const WebCore::IntPoint& point) const
{
    return roundedIntPoint(contentsToUserViewport(FloatPoint(point)));
}

WebCore::FloatRect PageViewportControllerClientManx::userViewportToContents(const WebCore::FloatRect& rect) const
{
    float scale = 1 / m_pageScaleFactor;
    FloatRect contentsRect = rect;
    contentsRect.scale(scale, scale);
    contentsRect.moveBy(m_contentsPosition);
    return contentsRect;
}

WebCore::IntRect PageViewportControllerClientManx::userViewportToContents(const WebCore::IntRect& rect) const
{
    return roundedIntRect(userViewportToContents(FloatRect(rect)));
}

WebCore::FloatRect PageViewportControllerClientManx::contentsToUserViewport(const WebCore::FloatRect& rect) const
{
    float scale = m_pageScaleFactor;
    FloatRect userViewportRect = rect;
    userViewportRect.moveBy(-m_contentsPosition);
    userViewportRect.scale(scale, scale);
    return userViewportRect;
}

WebCore::IntRect PageViewportControllerClientManx::contentsToUserViewport(const WebCore::IntRect& rect) const
{
    return roundedIntRect(contentsToUserViewport(FloatRect(rect)));
}

} // namespace WebKit
