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

#ifndef PageViewportControllerClientManx_h
#define PageViewportControllerClientManx_h

#include "PageViewportControllerClient.h"
#include <WebCore/FloatPoint.h>
#include <WebCore/TransformationMatrix.h>

namespace WebKit {

class WebView;

class PageViewportControllerClientManx : public PageViewportControllerClient {
public:
    explicit PageViewportControllerClientManx(WebView*);
    virtual ~PageViewportControllerClientManx() { }

    // PageViewportControllerClient API
    virtual void setViewportPosition(const WebCore::FloatPoint&) override;
    virtual void setPageScaleFactor(float) override;
    virtual void didChangeContentsSize(const WebCore::IntSize&) override;
    virtual void didChangeVisibleContents() override;
    virtual void didChangeViewportAttributes() override;

    void scrollPageBy(const WebCore::FloatPoint& deltaInUIUnits);
    void scalePage(float scale);

    //
    void applyTransformation(WebCore::TransformationMatrix&) const;

    WebCore::FloatPoint userViewportToContents(const WebCore::FloatPoint&) const;
    WebCore::IntPoint userViewportToContents(const WebCore::IntPoint&) const;

    WebCore::FloatPoint contentsToUserViewport(const WebCore::FloatPoint&) const;
    WebCore::IntPoint contentsToUserViewport(const WebCore::IntPoint&) const;

    WebCore::IntRect userViewportToContents(const WebCore::IntRect&) const;
    WebCore::FloatRect userViewportToContents(const WebCore::FloatRect&) const;

    WebCore::IntRect contentsToUserViewport(const WebCore::IntRect&) const;
    WebCore::FloatRect contentsToUserViewport(const WebCore::FloatRect&) const;

private:
    void scrollTo(const WebCore::FloatPoint& contentsPosition, const WebCore::FloatPoint& trajectoryVector = WebCore::FloatPoint::zero());

    WebView* m_webView;
    float m_pageScaleFactor;
    WebCore::FloatPoint m_contentsPosition; // Position in UI Units.
    WebCore::IntSize m_contentsSize;
};

} // namespace WebKit

#endif // PageViewportControllerClientManx_h
