/*
 * Copyright (C) 2012 Sony Network Entertainment Intl. All Rights Reserved.
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
 * THIS SOFTWARE IS PROVIDED BY SONY NETWORK ENTERTAINMENT INTL. ``AS IS''
 * ANY ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SONY NETWORK ENTERTAINMENT INTL.
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include "config.h"
#include "WebInspectorServer.h"

#include "FileSystem.h"
#include "MIMETypeRegistry.h"
#include "NotImplemented.h"
#include "WebInspectorProxy.h"
#include "WebKitVersion.h"
#include "WebProcessPool.h"
#include "WebProcessProxy.h"
#include <errno.h>
#include <inspector/InspectorValues.h>
#if USE(NTF)
#include <ntf/NetworkControl.h>
#endif
#include <wtf/Assertions.h>
#include <wtf/NeverDestroyed.h>
#include <wtf/text/CString.h>

using namespace WebCore;
using namespace Inspector;

namespace {
    // Use the last revision before the Blink/WebKit fork as our reference revision
    // (cf. https://groups.google.com/a/chromium.org/d/msg/blink-dev/J41PSKuMan0/gD5xcqicqP8J)
    // Revision numbering was maintained in Blink, but since history has started diverging after
    // that commit the WebKit revisions aren't mappable anymore.
    // ChromeDriver adjusts some event handling depending on that revision number.
    // (grep 'blink_revision' in chromedriver/chrome/navigation_tracker.cc)
    const int webkitRevision = 147503; // FIXME:

    const char* getServerPathFromEnv()
    {
        const char* name = getenv("MANX_INSPECTOR_SERVER_PATH");
        if (name && strlen(name))
            return name;
        return "";
    }

    const String& serverPath()
    {
        static NeverDestroyed<String> serverPath(getServerPathFromEnv());
        return serverPath;
    }

    const String inspectorBaseURL()
    {
        static String baseURL = String("file://") + serverPath();
        if (!baseURL.endsWith('/'))
            baseURL.append('/');

        return baseURL;
    }
}

namespace WebKit {

bool WebInspectorServer::platformResourceForPath(const String& path, Vector<char>& data, String& contentType)
{
    // The page list contains an unformated list of pages that can be inspected with a link to open a session.
    if (path == "/pagelist.json") {
        buildPageList(data, contentType);
        return true;
    }

    if (path.startsWith("/json"))
        return buildPageJson(path.substring(5), data, contentType);

    // Point the default path to display the inspector landing page.
    // All other paths are mapped directly to a resource, if possible.
    const URL baseURL(URL(), inspectorBaseURL());
    const URL resourceURL(baseURL, (path == "/") ? "inspectorPageIndex.html" : path.substring(1));
    if (resourceURL.isNull() || !resourceURL.isLocalFile())
        return false;

    // read resource data
    Vector<char> resourceData;
    long long resourceDataSize = 0;

    if (!loadResourceFile(resourceURL.fileSystemPath(), resourceData, resourceDataSize)) {
        StringBuilder builder;
        builder.appendLiteral("<!DOCTYPE html><html><head></head><body>Error: ");
        builder.appendLiteral(" Couldn't load file: ");
        builder.append(resourceURL);
        builder.appendLiteral(" occurred during fetching inspector resource files.</body></html>");

        CString errorHTML = builder.toString().utf8();
        data.append(errorHTML.data(), errorHTML.length());
        contentType = "text/html; charset=utf-8";

        WTFLogAlways("Error fetching webinspector resource files: %d, %s", -1, resourceURL.fileSystemPath().utf8().data());
        return false;
    }

    data.append(static_cast<const char*>(resourceData.data()), resourceData.size());

    const String localPath = resourceURL.fileSystemPath();
    contentType = MIMETypeRegistry::getMIMETypeForPath(localPath);

    return true;
}

bool WebInspectorServer::loadResourceFile(const String& localPath, Vector<char>& data, long long& dataSize)
{
    PlatformFileHandle handle = openFile(localPath, OpenForRead);
    if (!isHandleValid(handle)) {
        LOG_ERROR("WebInspectorServer: couldn't access platform resource '%s' for reading! (%d)", localPath.utf8().data(), errno);
        return false;
    }

    long long fileSize = 0;
    if (!getFileSize(localPath, fileSize)) {
        LOG_ERROR("WebInspectorServer: couldn't get file size for '%s'! (%d)", localPath.utf8().data(), errno);
        closeFile(handle);
        return false;
    }
    // NB: grow takes a size_t arg, so fileSize will be truncated to size_t which is an unsigned int.
    // This is safe because if we ever have fileSize > UINT_MAX the check below will simply fail.
    // This would only happen if we had to handle files >4GB, which we don't have in the inspector anyway.
    data.grow(fileSize);
    if (readFromFile(handle, data.data(), data.size()) < fileSize) {
        LOG_ERROR("WebInspectorServer: didn't read all contents of file '%s'! (%d)", localPath.utf8().data(), errno);
        closeFile(handle);
        return false;
    }

    closeFile(handle);

    dataSize = fileSize;
    return true;
}

void WebInspectorServer::buildPageList(Vector<char>& data, String& contentType)
{
    // chromedevtools (http://code.google.com/p/chromedevtools) 0.3.8 expected JSON format:
    // {
    //  "title": "Foo",
    //  "url": "http://foo",
    //  "devtoolsFrontendUrl": "/Main.html?ws=localhost:9222/devtools/page/1",
    //  "webSocketDebuggerUrl": "ws://localhost:9222/devtools/page/1"
    // },

    StringBuilder builder;
    builder.appendLiteral("[ ");
    ClientMap::iterator end = m_clientMap.end();
    for (ClientMap::iterator it = m_clientMap.begin(); it != end; ++it) {
        WebPageProxy* webPage = it->value->inspectedPage();
        if (it != m_clientMap.begin())
            builder.appendLiteral(", ");
        builder.appendLiteral("{ \"id\": ");
        builder.appendNumber(it->key);
        builder.appendLiteral(", \"title\": \"");
        builder.append(webPage->pageLoadState().title());
        builder.appendLiteral("\", \"url\": \"");
        builder.append(webPage->pageLoadState().activeURL());
        builder.appendLiteral("\", \"inspectorUrl\": \"");
        builder.appendLiteral("/Main.html?page=");
        builder.appendNumber(it->key);
        builder.appendLiteral("\", \"devtoolsFrontendUrl\": \"");
        builder.appendLiteral("/Main.html?ws=");
        builder.append(bindAddress());
        builder.appendLiteral(":");
        builder.appendNumber(port());
        builder.appendLiteral("/devtools/page/");
        builder.appendNumber(it->key);
        builder.appendLiteral("\", \"webSocketDebuggerUrl\": \"");
        builder.appendLiteral("ws://");
        builder.append(bindAddress());
        builder.appendLiteral(":");
        builder.appendNumber(port());
        builder.appendLiteral("/devtools/page/");
        builder.appendNumber(it->key);
        builder.appendLiteral("\" }");
    }
    builder.appendLiteral(" ]");
    CString cstr = builder.toString().utf8();
    data.append(cstr.data(), cstr.length());
    contentType = "application/json; charset=utf-8";
}

bool WebInspectorServer::buildPageJson(const String& jsonSubpath, Vector<char>& data, String& contentType)
{
    // Since "query" part is not available, we use the following path format for JSONP:
    //   /json/jsonp/CALLBACKNAME
    CString jsonpCallbackName;
    if (jsonSubpath.startsWith("/jsonp/"))
        jsonpCallbackName = jsonSubpath.substring(7).ascii();

    if (jsonSubpath.isEmpty() || jsonpCallbackName.length()) {
        RefPtr<InspectorArray> message = InspectorArray::create();

        for (HashMap<unsigned, WebInspectorProxy*>::const_iterator it = m_clientMap.begin(); it != m_clientMap.end(); ++it) {
            WebInspectorProxy* client = it->value;
            WebPageProxy* page = client->inspectedPage();
            WebProcessProxy& process = page->process();
            RefPtr<InspectorObject> tabInfo = InspectorObject::create();

            tabInfo->setString("id", String::number(it->key));
            tabInfo->setInteger("processID", process.processIdentifier());
            tabInfo->setInteger("processPageID", page->pageID());
            tabInfo->setString("title", page->pageLoadState().title());
            tabInfo->setString("type", "page");
            tabInfo->setString("url", page->pageLoadState().activeURL());
            tabInfo->setString("webSocketDebuggerUrl", String::format("ws://%s:%u/devtools/page/%u", bindAddress().latin1().data(), port(), it->key));

            WebProcessPool& processPool = process.processPool();
            if (processPool.webProcessPath().endsWith("SecureWebProcess.self"))
                tabInfo->setString("processDisplayName", "Live Area / Store / RegCAM");
            else if (processPool.webProcessPath().endsWith("WebProcessHTMLTile.self"))
                tabInfo->setString("processDisplayName", "HTML Live Tiles");
            else
                tabInfo->setString("processDisplayName", processPool.userStorageDirectory().endsWith("webbrowser") ? "Web Browser" : "Miscellaneous");

            message->pushObject(tabInfo);
        }

        if (jsonpCallbackName.length()) {
            data.append(jsonpCallbackName.data(), jsonpCallbackName.length());
            data.append('(');
        }

        CString tabInfo = message->toJSONString().utf8();
        data.append(tabInfo.data(), tabInfo.length());

        if (jsonpCallbackName.length())
            data.append(");", 2);

    } else if (jsonSubpath == "/version") {
        // Basic version info needed by the WebDriver server
        RefPtr<InspectorObject> versionInfo = InspectorObject::create();
        versionInfo->setString("Browser", ""); // Intentionally kept blank, indicates custom browser to ChromeDriver
        versionInfo->setString("Protocol-Version", "1.0"); // FIXME:
        versionInfo->setString("User-Agent", WebPageProxy::standardUserAgent());
        versionInfo->setString("WebKit-Version", String::format("%d.%d (@%d)", WEBKIT_MAJOR_VERSION, WEBKIT_MINOR_VERSION, webkitRevision));

        CString versionInfoString = versionInfo->toJSONString().utf8();
        data.append(versionInfoString.data(), versionInfoString.length());
    } else {
        // Unsupported command
        return false;
    }

    contentType = jsonpCallbackName.length() ? "application/javascript; charset=utf-8" : "application/json; charset=utf-8";
    return true;
}

void WebInspectorServer::updateServerState()
{
    char* envValue = getenv("MANX_INSPECTOR_SERVER_PORT");
    if (!envValue)
        return;

    if (!m_clientMap.isEmpty() && serverState() == Closed) {
        const int inspectorServerPort = atoi(envValue);
        if (inspectorServerPort <= 0) {
            LOG_ERROR("Invalid Inspector Server port!");
            return;
        }

#if USE(NTF)
        if (!listen(NTF::NetworkControl::getIPAddress(), inspectorServerPort))
            LOG_ERROR("Couldn't start the Inspector Server!");
#endif
    } else if (m_clientMap.isEmpty() && serverState() == Listening) {
        close();
        ASSERT(serverState() == Closed);
    }
}

}
