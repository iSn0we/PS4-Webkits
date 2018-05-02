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
#include <cstdlib>
#include <getopt.h>
#include <manx/RunLoop.h>
#include <manx/util.h>

#define WEBKIT_THEME_PATH "/hostapp/common/lib/theme"
#define WEBKIT_DATABASE_DIR "/hostapp/db"

static void setEnv()
{
    setenv("WEBKIT_THEME_PATH", WEBKIT_THEME_PATH, 1);
    setenv("WEBKIT_DATABASE_DIR", WEBKIT_DATABASE_DIR, 1);
}

static void showHelpInfo(char* executableName)
{
    printf("Usage: %s [-option] [argument]\n", executableName);
    printf("option: -h show help information\n");
    printf("        -w sets web app mode on\n");
    printf("        -b [path] sets injected bundle path\n");
    printf("        -u [url] sets home page URL\n");
    printf("example: %s -b /hostapp/common/lib/libInjectedBundle.sprx -m off -s off -u http://www.google.com/\n", executableName);
}

class Application : public SW::WindowManagerClient {
public:
    Application(int argc, char* argv[])
    {
        // Default config values
        bool useWebAppMode = false;
        std::string injectedBundlePath = "";
        std::string homeURL = "";

        int opt = 0;
        while ((opt = getopt(argc, argv, "hwb:u:")) != EOF) {
            switch (opt) {
            case 'b':
                injectedBundlePath = optarg;
                printf("Injected Bundle Path set to %s\n", optarg);
                break;
            case 'u':
                homeURL = optarg;
                printf("Home URL set to %s\n", optarg);
                break;
            case 'w':
                useWebAppMode = true;
                printf("Web App Mode Enabled.");
                break;
            case 'h':
            default:
                showHelpInfo(argv[0]);
                break;
            }
        }

        m_windowManager.reset(SW::WindowManager::create(this, DISPLAY_WIDTH, DISPLAY_HEIGHT));
        m_windowManager->setMouseMode(useWebAppMode ? false : true);

        m_browser.reset(new Browser(m_windowManager.get()));
        m_browser->setUseWebAppMode(useWebAppMode);

        if (!injectedBundlePath.empty())
            m_browser->setInjectedBundlePath(injectedBundlePath);

        if (!homeURL.empty())
            m_browser->setHomeURL(homeURL);
    }

    virtual void updateClient(void)
    {
        Manx::RunLoop::poll();
    }

    void run()
    {
        m_browser->show();
    }

private:
    std::auto_ptr<Browser> m_browser;
    std::auto_ptr<SW::WindowManager> m_windowManager;
};

int main(int argc, char* argv[])
{
    setEnv();
    Application app(argc, argv);
    app.run();
    return 0;
}
