/*
 * Copyright (C) 2012 Igalia S.L.
 * Copyright (C) 2012 Intel Corporation. All rights reserved.
 * Copyright (C) 2015 Sony Computer Entertainment Inc
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
#include "PlatformUtilities.h"

#include <manx/RunLoop.h>
#include <wtf/CurrentTime.h>

namespace TestWebKitAPI {
namespace Util {

void run(bool* done)
{
    while (!*done)
        Manx::RunLoop::poll();
}

void sleep(double seconds)
{
    double endTime = currentTime() + seconds;
    while (currentTime() < endTime)
        Manx::RunLoop::poll();
}

WKURLRef createURLForResource(const char* resource, const char* extension)
{
    char url[PATH_MAX];

    snprintf(url, sizeof(url), "file:///host/%s/%s.%s", TEST_WEBKIT2_RESOURCES_DIR, resource, extension);

    return WKURLCreateWithUTF8CString(url);
}

WKStringRef createInjectedBundlePath()
{
    return WKStringCreateWithUTF8CString("/hostapp/common/lib/libTestWebKitAPIInjectedBundle.sprx");
}

WKURLRef URLForNonExistentResource()
{
    return WKURLCreateWithUTF8CString("file:///does-not-exist.html");
}

}
}
