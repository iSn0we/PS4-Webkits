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

#include "config.h"

#if WK_HAVE_C_SPI

#include "InjectedBundleTest.h"
#include "PlatformUtilities.h"

#include <WebKit/WKRetainPtr.h>
#include <WebKit/WKBundleFrame.h>
#include <WebKit/WKBundleFrameManx.h>
#include <WebKit/WKBundlePage.h>

namespace TestWebKitAPI {

static void didFinishLoadForFrameCallback(WKBundlePageRef, WKBundleFrameRef, WKTypeRef*, const void* clientInfo);

class WKBundleFrameConvertMonotonicTimeToDocumentTimeTest : public InjectedBundleTest {
public:
    WKBundleFrameConvertMonotonicTimeToDocumentTimeTest(const std::string& identifier);

    virtual void didCreatePage(WKBundleRef, WKBundlePageRef) override;

    void frameLoadFinished(WKBundleFrameRef);

private:
    WKBundleRef m_bundle;
};

static InjectedBundleTest::Register<WKBundleFrameConvertMonotonicTimeToDocumentTimeTest> registrar("WKBundleFrameConvertMonotonicTimeToDocumentTimeTest");

WKBundleFrameConvertMonotonicTimeToDocumentTimeTest::WKBundleFrameConvertMonotonicTimeToDocumentTimeTest(const std::string& identifier)
    : InjectedBundleTest(identifier)
{
}

void WKBundleFrameConvertMonotonicTimeToDocumentTimeTest::didCreatePage(WKBundleRef bundle, WKBundlePageRef page)
{
    m_bundle = bundle;

    WKBundlePageLoaderClientV0 pageLoaderClient;
    memset(&pageLoaderClient, 0, sizeof(pageLoaderClient));

    pageLoaderClient.base.version = 0;
    pageLoaderClient.base.clientInfo = this;
    pageLoaderClient.didFinishLoadForFrame = didFinishLoadForFrameCallback;      

    WKBundlePageSetPageLoaderClient(page, &pageLoaderClient.base);
}

void WKBundleFrameConvertMonotonicTimeToDocumentTimeTest::frameLoadFinished(WKBundleFrameRef frame)
{
    double time = WKBundleFrameConvertMonotonicTimeToDocumentTime(frame, 0.0);
    if (fabs(time) < 0.000001)
        WKBundlePostMessage(m_bundle, Util::toWK("SUCCEED").get(), nullptr);
    else
        WKBundlePostMessage(m_bundle, Util::toWK("FAIL").get(), nullptr);
}

static void didFinishLoadForFrameCallback(WKBundlePageRef, WKBundleFrameRef frame, WKTypeRef*, const void *clientInfo)
{
    ((WKBundleFrameConvertMonotonicTimeToDocumentTimeTest*)clientInfo)->frameLoadFinished(frame);
}

} // namespace TestWebKitAPI

#endif
