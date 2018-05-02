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
#include <WebKit/WKBundleManx.h>

namespace TestWebKitAPI {

class WKBundleGetMemoryCacheStatisticsTest : public InjectedBundleTest {
public:
    WKBundleGetMemoryCacheStatisticsTest(const std::string& identifier)
        : InjectedBundleTest(identifier)
    {
    }

    virtual void didReceiveMessage(WKBundleRef bundle, WKStringRef messageName, WKTypeRef messageBody) override
    {
        WKBundleMemoryCacheStatistics memoryCacheStatistics;
        WKBundleGetMemoryCacheStatistics(bundle, &memoryCacheStatistics);

        bool ok = true;

        // image
        ok = ok && (memoryCacheStatistics.images.count == 1);
        ok = ok && memoryCacheStatistics.images.size;
        ok = ok && memoryCacheStatistics.images.liveSize;
        ok = ok && !memoryCacheStatistics.images.decodedSize; // FIXME: images.decodedSize value is 0.
        ok = ok && !memoryCacheStatistics.images.purgeableSize;
        ok = ok && !memoryCacheStatistics.images.purgedSize;

        // cssStyleSheets
        ok = ok && (memoryCacheStatistics.cssStyleSheets.count == 1);
        ok = ok && memoryCacheStatistics.cssStyleSheets.size;
        ok = ok && memoryCacheStatistics.cssStyleSheets.liveSize;
        ok = ok && memoryCacheStatistics.cssStyleSheets.decodedSize;
        ok = ok && !memoryCacheStatistics.cssStyleSheets.purgeableSize;
        ok = ok && !memoryCacheStatistics.cssStyleSheets.purgedSize;

        // scripts
        ok = ok && (memoryCacheStatistics.scripts.count == 1);
        ok = ok && memoryCacheStatistics.scripts.size;
        ok = ok && memoryCacheStatistics.scripts.liveSize;
        ok = ok && !memoryCacheStatistics.scripts.decodedSize;
        ok = ok && !memoryCacheStatistics.scripts.purgeableSize;
        ok = ok && !memoryCacheStatistics.scripts.purgedSize;

        // xslStyleSheets
        //  NOTE : XSLT is disable.
        //  It doesn't run xslStyleSheets test.

        // fonts
        ok = ok && (memoryCacheStatistics.fonts.count == 1);
        ok = ok && memoryCacheStatistics.fonts.size;
        ok = ok && memoryCacheStatistics.fonts.liveSize;
        ok = ok && !memoryCacheStatistics.fonts.decodedSize;
        ok = ok && !memoryCacheStatistics.fonts.purgeableSize;
        ok = ok && !memoryCacheStatistics.fonts.purgedSize;

        WKBundlePostMessage(bundle, Util::toWK(ok ? "SUCCEED" : "FAIL").get(), nullptr);
    }
};

static InjectedBundleTest::Register<WKBundleGetMemoryCacheStatisticsTest> registrar("WKBundleGetMemoryCacheStatisticsTest");

} // namespace TestWebKitAPI

#endif
