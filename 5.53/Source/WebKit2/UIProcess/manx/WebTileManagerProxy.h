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

#ifndef WebTileManagerProxy_h
#define WebTileManagerProxy_h

#if ENABLE(MANX_HTMLTILE)

#include "MessageReceiver.h"
#include "WebContextSupplement.h"
#include "WebProcessPool.h"
#include <wtf/HashMap.h>
#include <wtf/RefCounted.h>
#include <wtf/RefPtr.h>
#include <wtf/text/WTFString.h>

namespace WebKit {

class WebProcessPool;
class WebPageGroup;
class WebProcessProxy;
class WebTileProxy;
class UpdateInfo;

class WebTileManagerProxy : public RefCounted<WebTileManagerProxy>, public WebContextSupplement, private IPC::MessageReceiver {
public:
    static const char* supplementName();

    static PassRefPtr<WebTileManagerProxy> create(WebProcessPool*);
    ~WebTileManagerProxy();

    WebProcessPool* webTilesProcessPool();

    void dispatchTileUpdated(uint64_t tileID, const UpdateInfo&) const;
    void dispatchTileValid(uint64_t tileID) const;
    void dispatchTileUnresponsive(uint64_t tileID) const;
    void dispatchTileCrashed(uint64_t tileID) const;
    void dispatchLoadCompleted(uint64_t tileID) const;
    void dispatchLoadFailed(uint64_t tileID, uint32_t errorCode) const;
    void dispatchLocationChangeRequested(uint64_t tileID, const String& url) const;
    void dispatchRunJavaScriptResultCallback(uint64_t tileID, uint64_t callbackID, const String& result) const;

    void pauseWebTiles();
    void resumeWebTiles();
    bool isPaused();

    using RefCounted::ref;
    using RefCounted::deref;

private:
    WebTileManagerProxy(WebProcessPool*);

    void createWebTile(uint64_t tileID, uint32_t width, uint32_t height, uint64_t pageID);
    void destroyWebTile(uint64_t tileID);
    void validWebTile(uint64_t tileID);
    void pauseWebTile(uint64_t tileID);
    void resumeWebTile(uint64_t tileID);
    void sendWebTileEvent(uint64_t tileID, uint32_t eventType);
    void runJavaScriptInWebTile(uint64_t tileID, const String& jsScript, uint64_t callbackID);
    void didChangeWebTileURL(uint64_t tileID, const String& url);
    void didUpdateWebTile(uint64_t tileID);

    // WebContextSupplement Interface
    virtual void processPoolDestroyed() override;
    virtual void processDidClose(WebProcessProxy*) override;
    virtual bool shouldTerminate(WebProcessProxy*) const override;
    virtual void refWebContextSupplement() override;
    virtual void derefWebContextSupplement() override;

    // IPC::MessageReceiver Interface
    virtual void didReceiveMessage(IPC::Connection&, IPC::MessageDecoder&) override;

    RefPtr<WebProcessPool> m_webTilesProcessPool;
    RefPtr<WebPageGroup> m_webTilesPageGroup;

    HashMap<uint64_t, RefPtr<WebTileProxy> > m_webTiles;

    bool m_paused = false;
};

} // namespace WebKit

#endif // ENABLE(MANX_HTMLTILE)

#endif // WebTileManagerProxy_h
