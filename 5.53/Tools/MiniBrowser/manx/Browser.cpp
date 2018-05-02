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

#include "Browser.h"

#include "utils.h"
#include <cassert>

static const char* HomePageURL = "http://www.yahoo.co.jp/";

Browser::Browser(SW::WindowManager* windowManager)
    : SW::DialogWindow(windowManager)
    , m_activeWebViewIndex(-1)
    , m_homeURL(HomePageURL)
    , m_injectedBundlePath("")
    , m_useWebAppMode(false)
{
}

Browser::~Browser()
{
    for (auto view : m_webViewList)
        delete view;
    m_webViewList.clear();
    for (auto context : m_webContextMap)
        delete context.first;
    m_webContextMap.clear();
}

WebView* Browser::webView() const
{
    return m_webViewList[m_activeWebViewIndex];
}

WebView* Browser::createWebView(WebContext* context)
{
    m_webContextMap.at(context)++;
    WebView* webView = new WebView(this, context);
    m_webViewList.insert(m_webViewList.end(), webView);
    setActive(m_webViewList.size() - 1);
    return webView;
}

void Browser::openNewTab()
{
    createWebView(webView()->webContext());
    webView()->load(m_homeURL);
}

void Browser::openNewTabWithNewContext()
{
    int userID = windowManager()->userID();
    bool useWebSecurity = !m_useWebAppMode;
    WebContext* context = new WebContext(userID, m_injectedBundlePath.c_str(), useWebSecurity);
    m_webContextMap[context] = 0;
    createWebView(context);
    webView()->load(m_homeURL);
}

void Browser::closeTab(WebView* webView)
{
    WebContext* context = webView->webContext();
    std::vector<WebView*>::iterator i;
    for (i = m_webViewList.begin(); i != m_webViewList.end(); ++i) {
        if (*i == webView) {
            delete webView;
            m_webViewList.erase(i);
            switchTab(0);
            break;
        }
    }
    std::map<WebContext*, int>::iterator it = m_webContextMap.find(context);
    it->second--;
    if (!it->second) {
        delete context;
        m_webContextMap.erase(it);
    }
}

void Browser::switchTab(int delta)
{
    unsigned size = m_webViewList.size();
    setActive((m_activeWebViewIndex + size + delta) % size);
}

void Browser::closeThisTab()
{
    webView()->tryClose();
}

void Browser::showMenu()
{
    enum {
        CmdQuit,
        CmdStop,
        CmdReload,
        CmdOpenNewTab,
        CmdOpenNewTabWithNewContext,
        CmdCloseThisTab,
        CmdGoogle,
        CmdGoHome,
        CmdStartAutoCruise,
        CmdShowUserAgentMenu,
        CmdToggleIFilterProxy,
        CmdToggleDNT,
        CmdToggleDebugAccessibility,
        CmdToggleActiveState,
        CmdPauseHTMLTiles,
        CmdResumeHTMLTiles,
    };

    SW::MenuDialog menu(windowManager());
    menu.appendItem("Quit", CmdQuit);
    menu.appendItem("Stop", CmdStop);

    // In Web App mode, reloading should reload the app, so we go
    // home just to be safe.
    menu.appendItem("Reload", m_useWebAppMode ? CmdGoHome : CmdReload);

    if (!m_useWebAppMode) {
        menu.appendItem("Open New Tab", CmdOpenNewTab);
        menu.appendItem("Open New Tab with New WebContext", CmdOpenNewTabWithNewContext);
        if (m_webViewList.size() > 1)
            menu.appendItem("Close This Tab", CmdCloseThisTab);
        menu.appendItem("Auto Cruise", CmdStartAutoCruise);
        menu.appendItem("Switch User Agent", CmdShowUserAgentMenu);
    }

    // It is not currently possible to check the state of the tile manager
    // so Pause and Resume options will always be shown.
    menu.appendItem("Pause HTMLTiles", CmdPauseHTMLTiles);
    menu.appendItem("Resume HTMLTiles", CmdResumeHTMLTiles);

    if (webView()->isIFilterProxyMenuEnabled())
        menu.appendItem(webView()->isIFilterProxyEnabled() ? "Disable i-Filter Proxy" : "Enable i-Filter Proxy", CmdToggleIFilterProxy);
    menu.appendItem(webView()->isDNTEnabled() ? "Disable DNT" : "Enable DNT", CmdToggleDNT);
    menu.appendItem(webView()->isDebugAccessibilityEnabled() ? "Disable Debug Accessibility" : "Enable Debug Accessibility", CmdToggleDebugAccessibility);
    menu.appendItem(webView()->isActive() ? "Deactivate WebView" : "Activate WebView", CmdToggleActiveState);

    if (!m_useWebAppMode) {
        menu.appendItem("Google", CmdGoogle);
        menu.appendItem("Go Home", CmdGoHome);
    }

    webView()->setIsVisible(false);
    int cmd = menu.show();
    webView()->setIsVisible(true);
    switch (cmd) {
    case CmdQuit:
        close();
        break;
    case CmdStop:
        webView()->stop();
        break;
    case CmdReload:
        webView()->reload();
        break;
    case CmdOpenNewTab:
        openNewTab();
        break;
    case CmdOpenNewTabWithNewContext:
        openNewTabWithNewContext();
        break;
    case CmdCloseThisTab:
        closeThisTab();
        break;
    case CmdGoogle:
        webView()->load("http://google.com");
        break;
    case CmdGoHome:
        webView()->load(m_homeURL);
        break;
    case CmdStartAutoCruise:
        webView()->setAutoCruiseMode(true);
        break;
    case CmdShowUserAgentMenu:
        showUserAgentMenu();
        break;
    case CmdToggleIFilterProxy:
        webView()->toggleIFilterProxy();
        break;
    case CmdToggleDNT:
        webView()->toggleDNT();
        break;
    case CmdToggleDebugAccessibility:
        webView()->toggleDebugAccessibility();
        break;
    case CmdToggleActiveState:
        webView()->setIsActive(!webView()->isActive());
        break;
    case CmdPauseHTMLTiles:
        WKContextPauseHTMLTiles(webView()->webContext()->context());
        break;
    case CmdResumeHTMLTiles:
        WKContextResumeHTMLTiles(webView()->webContext()->context());
        break;
    }
}

void Browser::showUserAgentMenu()
{
    const struct {
        const char* label;
        const char* userAgent;
    } items[] = {
        { "Default", "" },
        { "Chrome 35", "Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/35.0.1916.114 Safari/537.36" },
        { "Safari 7", "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_9) AppleWebKit/537.71 (KHTML, like Gecko) Version/7.0 Safari/537.71" },
        { "Apple iPhone Mobile Safari iOS 7.0.6 ", "Mozilla/5.0 (iPhone; CPU iPhone OS 7_0_6 like Mac OS X) AppleWebKit/537.51.1 (KHTML, like Gecko) Version/7.0 Mobile/11B651 Safari/9537.53" },
        { "IE 11", "Mozilla/5.0 (Windows NT 6.3; WOW64; Trident/7.0; Touch; rv:11.0) like Gecko" },
        { "Firefox 29", "Mozilla/5.0 (Windows NT 6.1; WOW64; rv:29.0) Gecko/20100101 Firefox/29.0" },
        { "PS Vita", "Mozilla/5.0 (PlayStation Vita 3.00) AppleWebKit/536.26 (KHTML, like Gecko)" },
    };

    SW::MenuDialog menu(windowManager());

    for (int i = 0; i < sizeof(items) / sizeof(items[0]); i++)
        menu.appendItem(items[i].label, i);

    int index = menu.show();
    if (index < 0)
        return;
    assert(index < sizeof(items) / sizeof(items[0]));
    webView()->setUserAgent(items[index].userAgent);
    webView()->reload();
}

bool Browser::handleEvent(SW::Event& event)
{
    bool bubbleToWebView = true;
    if (event.m_type == SW::Event::KeyDown) {
        SW::KeyboardEvent& keyboardEvent = static_cast<SW::KeyboardEvent&>(event);

        // In Web-app mode we only allow the Options menu to be accessed
        // because many button assignments overlap with Web App button assignments.
        if (m_useWebAppMode) {
            switch (keyboardEvent.m_vk) {
            case SW::VK_OPTIONS:
                // Some web apps still use this key, so we allow it to bubble,
                // even though the UX is inconvenient.
                showMenu();
                break;
            }
        } else {
            switch (keyboardEvent.m_vk) {
            case SW::VK_L1:
                webView()->goBack();
                bubbleToWebView = false;
                break;
            case SW::VK_R1:
                webView()->goForward();
                bubbleToWebView = false;
                break;
            case SW::VK_L2:
                switchTab(-1);
                bubbleToWebView = false;
                break;
            case SW::VK_R2:
                switchTab(1);
                bubbleToWebView = false;
                break;
            case SW::VK_OPTIONS:
                showMenu();
                bubbleToWebView = false;
                break;
            case SW::VK_R3:
                webView()->toggleZoomState();
                bubbleToWebView = false;
                break;
            }
        }
    }

    return bubbleToWebView ? webView()->handleEvent(event) : false;
}

SW::Rect Browser::clientRect()
{
    int posY = DISPLAY_HEIGHT - WEBVIEW_HEIGHT;
    return SW::Rect(SW::Point(0, posY), SW::Size(webView()->width(), webView()->height()));
}

SW::CanvasHole Browser::canvasHole(uint32_t index)
{
    return webView()->canvasHole(index);
}

void Browser::drawTexture(SW::GraphicsContext* graphicsContext, SW::Texture* texture)
{
    SW::Rect rect = clientRect();
    graphicsContext->drawTexture(rect.m_pos, rect.m_size, texture);
}

void Browser::paint(SW::GraphicsContext* graphicsContext)
{
    const char* renderModeStr;
    switch (SW::WindowManager::renderMode()) {
    case SW::RenderModeCPU:
        renderModeStr = "CPU";
        break;
    case SW::RenderModeAC:
        renderModeStr = "AC";
        break;
    case SW::RenderModeIndirect:
        renderModeStr = "Indirect";
        break;
    default:
        renderModeStr = "Unknown";
        break;
    }

    const char* shaderMode;
    switch (SW::WindowManager::shaderMode()) {
    case SW::ShaderModeBuiltin:
        shaderMode = "Builtin";
        break;
    case SW::ShaderModeRuntime:
        shaderMode = "Runtime";
        break;
    default:
        shaderMode = "Unknown";
        break;
    }

    const unsigned bufSize = 256;
    char buf[bufSize];

    if (m_useWebAppMode) {
        const unsigned clipLength = 64;
        snprintf(buf, bufSize, "[URL] %-*s  [WEBAPP]", clipLength, webView()->url().substr(0, clipLength).c_str());
    } else
        snprintf(buf, bufSize, "[URL] %s", webView()->url().c_str());

    graphicsContext->drawDebugString(SW::Point(0, 0), 40, SW::Color::white(), buf);

    snprintf(buf, bufSize, "[TITLE] %s", webView()->title().c_str());
    graphicsContext->drawDebugString(SW::Point(0, 40), 40, SW::Color::white(), buf);

    if (m_useWebAppMode)
        snprintf(buf, bufSize, "[PROGRESS] %3d [RENDER MODE] %s [SHADER] %s", webView()->progress(), renderModeStr, shaderMode);
    else
        snprintf(buf, bufSize, "[PROGRESS] %3d [TAB] %d/%d [RENDER MODE] %s [SHADER] %s", webView()->progress(), m_activeWebViewIndex + 1, m_webViewList.size(), renderModeStr, shaderMode);

    graphicsContext->drawDebugString(SW::Point(0, 80), 40, SW::Color::white(), buf);

    if (SW::WindowManager::renderMode() == SW::RenderModeCPU)
        webView()->paint(graphicsContext, clientRect());
}

void Browser::paintToCurrentGLContext(const SW::Rect& rect)
{
    webView()->paintToCurrentGLContext(rect);
}

void Browser::setActive(unsigned active)
{
    // Make current web view hidden
    if (m_activeWebViewIndex != -1 && m_activeWebViewIndex < m_webViewList.size())
        m_webViewList[m_activeWebViewIndex]->setIsVisible(false);

    m_activeWebViewIndex = active;
    m_webViewList[m_activeWebViewIndex]->setIsVisible(true);
}

void Browser::show()
{
    openNewTabWithNewContext();
    SW::DialogWindow::show();
}
