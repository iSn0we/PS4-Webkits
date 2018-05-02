/*
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

#pragma once
#include "WebView.h"
#include <map>
#include <vector>

class Browser : public SW::DialogWindow {
public:
    Browser(SW::WindowManager*);
    ~Browser();

    WebView* createWebView(WebContext*);
    void setHomeURL(std::string url) { m_homeURL = url; }
    void setInjectedBundlePath(std::string path) { m_injectedBundlePath = path; }
    void setUseWebAppMode(bool enabled) { m_useWebAppMode = enabled; }
    void openNewTab();
    void openNewTabWithNewContext();
    void closeTab(WebView*);
    void switchTab(int delta = 1);
    void show();

private:
    virtual bool handleEvent(SW::Event&) override;
    virtual void paint(SW::GraphicsContext*) override;
    virtual void paintToCurrentGLContext(const SW::Rect&) override;
    virtual SW::Rect clientRect() override;
    virtual SW::CanvasHole canvasHole(uint32_t index) override;

    void drawTexture(SW::GraphicsContext*, SW::Texture*);
    void closeThisTab();
    void showMenu();
    void showUserAgentMenu();
    void setActive(unsigned);

    WebView* webView() const;
    std::map<WebContext*, int> m_webContextMap;
    unsigned m_activeWebViewIndex;
    std::vector<WebView*> m_webViewList;
    std::string m_homeURL;
    std::string m_injectedBundlePath;
    bool m_useWebAppMode;
};
