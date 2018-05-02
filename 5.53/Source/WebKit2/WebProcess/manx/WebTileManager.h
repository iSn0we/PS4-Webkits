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

#ifndef WebTileManager_h
#define WebTileManager_h

#if ENABLE(MANX_HTMLTILE)

#include "MessageReceiver.h"
#include <WebCore/Modules/htmltile/HTMLTileClient.h>
#include <wtf/HashMap.h>
#include <wtf/Noncopyable.h>
#include <wtf/text/WTFString.h>

namespace WebCore {
class DOMWrapperWorld;
class HTMLTile;
}

namespace WebKit {

class UpdateInfo;
class WebFrame;
class WebPage;
class WebTile;

class WebTileManager : public WebCore::HTMLTileClient, private IPC::MessageReceiver {
    WTF_MAKE_NONCOPYABLE(WebTileManager);
public:
    WebTileManager(WebPage*);
    virtual ~WebTileManager();

    bool enableHTMLTileForFrame(WebFrame*, WebCore::DOMWrapperWorld&);

    // HTMLTileClient interface
    uint64_t createTile(int width, int height, const unsigned char*& tileData, size_t& tileDataSize, WebCore::HTMLTile*) override;
    void destroyTile(uint64_t tileID) override;
    void tileValid(uint64_t tileID);
    void loadURL(uint64_t tileID, const String& url) override;
    void sendEvent(uint64_t tileID, int eventType) override;
    void pause(uint64_t tileID) override;
    void resume(uint64_t tileID) override;
    void eval(uint64_t tileID, const String& jsScript, uint64_t callbackID) override;

private:
    void tileUpdated(uint64_t tileID, const UpdateInfo&);
    void tileUnresponsive(uint64_t tileID);
    void tileCrashed(uint64_t tileID);
    void loadCompleted(uint64_t tileID);
    void loadFailed(uint64_t tileID, uint32_t errorCode);
    void locationChangeRequested(uint64_t tileID, const String& url);
    void runJavaScriptResultCallback(uint64_t tileID, uint64_t callbackID, const String& result);

    // IPC::MessageReceiver Interface.
    virtual void didReceiveMessage(IPC::Connection&, IPC::MessageDecoder&) override;

    WebPage* m_webPage;

    struct TileData {
        WebTile* webTile;
        // This should ideally be a WeakPtr<WebCore::HTMLTile> but WTF does provide WeakPtr :-(
        WebCore::HTMLTile* htmlTile;
    };

    HashMap<uint64_t, std::unique_ptr<TileData> > m_tiles;
};

} // namespace WebKit

#endif // ENABLE(MANX_HTMLTILE)

#endif // WebTileManager_h
