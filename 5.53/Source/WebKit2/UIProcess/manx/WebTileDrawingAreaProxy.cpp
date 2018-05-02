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
#include "WebTileDrawingAreaProxy.h"

#if ENABLE(MANX_HTMLTILE)

#include "DrawingAreaMessages.h"
#include "UpdateInfo.h"
#include "WebPageProxy.h"
#include "WebProcessProxy.h"
#include "notImplemented.h"
#include <WebCore/IntRect.h>

using namespace WebCore;

namespace WebKit {

std::unique_ptr<WebTileDrawingAreaProxy> WebTileDrawingAreaProxy::create(WebPageProxy& webPageProxy)
{
    return std::unique_ptr<WebTileDrawingAreaProxy>(new WebTileDrawingAreaProxy(webPageProxy));
}

WebTileDrawingAreaProxy::WebTileDrawingAreaProxy(WebPageProxy& webPageProxy)
    : DrawingAreaProxy(DrawingAreaTypeCoordinated, webPageProxy)
    , m_pendingUpdateInfo(0)
{
}

WebTileDrawingAreaProxy::~WebTileDrawingAreaProxy()
{
}

const UpdateInfo* WebTileDrawingAreaProxy::pendingUpdateInfo() const
{
    return m_pendingUpdateInfo;
}

void WebTileDrawingAreaProxy::didUpdate()
{
    m_webPageProxy.process().send(Messages::DrawingArea::DidUpdate(), m_webPageProxy.pageID());
}

void WebTileDrawingAreaProxy::sizeDidChange()
{
    // We don't support changing sizes after the initial one is set
    notImplemented();
}

void WebTileDrawingAreaProxy::deviceScaleFactorDidChange()
{
    notImplemented();
}

void WebTileDrawingAreaProxy::update(uint64_t backingStoreStateID, const UpdateInfo& updateInfo)
{
    ASSERT_ARG(backingStoreStateID, !backingStoreStateID);

    if (updateInfo.updateRectBounds.isEmpty())
        return;

    m_pendingUpdateInfo = &updateInfo;
    // Don't worry about the updateRects, we'll handle them on the other side
    m_webPageProxy.setViewNeedsDisplay(IntRect());
    m_pendingUpdateInfo = 0;
}

void WebTileDrawingAreaProxy::didUpdateBackingStoreState(uint64_t backingStoreStateID, const UpdateInfo &, const LayerTreeContext &)
{
    ASSERT_NOT_REACHED();
    notImplemented();
}

} // namespace WebKit

#endif // ENABLE(MANX_HTMLTILE)
