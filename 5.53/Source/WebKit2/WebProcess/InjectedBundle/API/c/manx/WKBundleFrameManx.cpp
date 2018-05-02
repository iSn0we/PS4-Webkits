/*
 * Copyright (C) 2015 Sony Interactive Entertainment Inc.
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
#include "WKBundleFrameManx.h"

#include "DrawingArea.h"
#include "InjectedBundleScriptWorld.h"
#include "LayerTreeHost.h"
#include "WKAPICast.h"
#include "WKBundleAPICast.h"
#include "WKSharedAPICast.h"
#include "WebFrame.h"
#include "WebPage.h"
#include "WebTileManager.h"
#include <WebCore/Document.h>
#include <WebCore/DocumentLoader.h>
#include <WebCore/Frame.h>
#include <WebCore/TileUpdateThrottle.h>

#if ENABLE(MANX_ASYNC_IMAGE_DECODER)
#include <WebCore/AsyncImageDecoder.h>
#endif

using namespace WebCore;
using namespace WebKit;


bool WKBundleFrameRegisterAsyncImageDecoder(WKStringRef propertyName, WKBundleFrameRef frame, WKBundleScriptWorldRef world)
{
#if ENABLE(MANX_ASYNC_IMAGE_DECODER)
    return AsyncImageDecoder::registerOnFrame(toImpl(propertyName)->string(), toImpl(frame)->coreFrame(), toImpl(world)->coreWorld());
#else
    return false;
#endif
}

double WKBundleFrameConvertMonotonicTimeToDocumentTime(WKBundleFrameRef frame, double timestamp)
{
    // Convert to the same format and base as webkitNow()
    return 1000.0 * toImpl(frame)->coreFrame()->document()->loader()->timing().monotonicTimeToZeroBasedDocumentTime(timestamp);
}

bool WKBundleFrameEnableHTMLTile(WKBundleFrameRef frame, WKBundleScriptWorldRef world)
{
#if ENABLE(MANX_HTMLTILE)
    return toImpl(frame)->page()->webTileManager().enableHTMLTileForFrame(toImpl(frame), toImpl(world)->coreWorld());
#else
    return false;
#endif
}

void WKBundleFrameGetACMemoryInfo(WKBundleFrameRef frame, WKGLMemoryInfo* wkACMemoryInfo)
{
    if (!wkACMemoryInfo)
        return;

    WebCore::GLMemoryInfo acMemoroyInfo = toImpl(frame)->page()->corePage()->acMemoryInfo();
    wkACMemoryInfo->heap = acMemoroyInfo.heap;
    wkACMemoryInfo->texture = acMemoroyInfo.texture;
    wkACMemoryInfo->surfaces = acMemoroyInfo.surfaces;
    wkACMemoryInfo->programs = acMemoroyInfo.programs;
    wkACMemoryInfo->buffers = acMemoroyInfo.buffers;
    wkACMemoryInfo->commandBuffers = acMemoroyInfo.commandBuffers;
    wkACMemoryInfo->total = acMemoroyInfo.total;
    wkACMemoryInfo->maxAllowed = acMemoroyInfo.maxAllowed;
    wkACMemoryInfo->fmemMappedSizeTotal = acMemoroyInfo.fmemMappedSizeTotal;
    wkACMemoryInfo->fmemMappedSizeLimit = acMemoroyInfo.fmemMappedSizeLimit;
}

void WKBundleFrameEnableThrottlingByTileCount(WKBundleFrameRef frame, int count)
{
    LayerTreeHost* host = toImpl(frame)->page()->drawingArea()->layerTreeHost();
    if (!host)
        return;
    TileUpdateThrottle* throttle = host->tileUpdateThrottle();
    if (!throttle)
        return;
    throttle->enableThrottlingByTileCount(count);
}

void WKBundleFrameEnableThrottlingByTime(WKBundleFrameRef frame, double timeInMs)
{
    LayerTreeHost* host = toImpl(frame)->page()->drawingArea()->layerTreeHost();
    if (!host)
        return;
    TileUpdateThrottle* throttle = host->tileUpdateThrottle();
    if (!throttle)
        return;
    throttle->enableThrottlingByTime(timeInMs);
}

void WKBundleFrameDisableThrottlingByTileCount(WKBundleFrameRef frame)
{
    LayerTreeHost* host = toImpl(frame)->page()->drawingArea()->layerTreeHost();
    if (!host)
        return;
    TileUpdateThrottle* throttle = host->tileUpdateThrottle();
    if (!throttle)
        return;
    throttle->disableThrottlingByTileCount();
}

void WKBundleFrameDisableThrottlingByTime(WKBundleFrameRef frame)
{
    LayerTreeHost* host = toImpl(frame)->page()->drawingArea()->layerTreeHost();
    if (!host)
        return;
    TileUpdateThrottle* throttle = host->tileUpdateThrottle();
    if (!throttle)
        return;
    throttle->disbleThrottlingByTime();
}
