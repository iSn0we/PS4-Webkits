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

#include "utils.h"

#include "WKRetainPtr.h"

std::string
createString(WKStringRef wkString)
{
    size_t maxSize = WKStringGetMaximumUTF8CStringSize(wkString);
    char* buffer = new char[maxSize];
    size_t actualSize = WKStringGetUTF8CString(wkString, buffer, maxSize);
    std::string ret(buffer, actualSize);
    delete [] buffer;
    return ret;
}

std::string createString(WKURLRef wkURL)
{
    WKRetainPtr<WKStringRef> url = adoptWK(WKURLCopyString(wkURL));
    return createString(url.get());
}

std::string toUtf8(const std::wstring& src)
{
    size_t len = src.length();
    char* mbs = new char[len * MB_CUR_MAX + 1];
    wcstombs(mbs, src.c_str(), len * MB_CUR_MAX + 1);
    std::string dest(mbs);
    delete[] mbs;
    return dest;
}

std::wstring fromUtf8(const std::string& src)
{
    size_t len = src.length();
    wchar_t* wcs = new wchar_t[len + 1];
    size_t wlen;
    mbstowcs_s(&wlen, wcs, len+1, src.c_str(), len);
    std::wstring dest(wcs);
    delete[] wcs;
    return dest;
}
