/*
 * Copyright (C) 2010 Apple Inc. All rights reserved.
 * Copyright (C) 2010 Igalia S.L.
 * Copyright (C) 2013 Sony Computer Entertainment Inc.
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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "TestController.h"

#include "PlatformWebView.h"
#include "TestBed.h"
#include <WebKit/WKContextManx.h>
#include <wtf/CurrentTime.h>
#include <wtf/Platform.h>
#include <wtf/text/WTFString.h>

namespace WTR {

TestBed* g_testBed;

void TestController::notifyDone()
{
    g_testBed->notifyDone();
}

void TestController::platformInitialize()
{
    g_testBed = new TestBed;
}

void TestController::platformDestroy()
{
}

void TestController::platformWillRunTest(const TestInvocation&)
{
}

void TestController::platformRunUntil(bool& done, double timeout)
{
    g_testBed->runUntil(mainWebView()->platformView(), &done, timeout);
}

void TestController::initializeInjectedBundlePath()
{
    m_injectedBundlePath.adopt(WKStringCreateWithUTF8CString("/hostapp/common/lib/libTestRunnerInjectedBundle.sprx"));
}

void TestController::initializeTestPluginDirectory()
{
    m_testPluginDirectory.adopt(WKStringCreateWithUTF8CString(""));
}

void TestController::platformInitializeContext()
{
    WKContextSetWebProcessPath(m_context.get(), WKStringCreateWithUTF8CString("/hostapp/common/lib/WebProcess.self"));
    if (!m_hostPcIp.isEmpty())
        setenv("MANX_WTR_HOSTPC_IP", m_hostPcIp.utf8().data(), 1);
}

void TestController::runModal(PlatformWebView*)
{
}

const char* TestController::platformLibraryPathForTesting()
{
    return "/hostapp";
}

void TestController::setHidden(bool hidden)
{
}

} // namespace WTR
