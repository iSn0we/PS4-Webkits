/*
 * Copyright (C) 2006 Apple Computer, Inc.  All rights reserved.
 * Copyright (C) 2013 Sony Interactive Entertainment Inc.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef ResourceResponse_h
#define ResourceResponse_h

#include "HTTPHeaderNames.h"
#include "HTTPParsers.h"

#include "ResourceResponseBase.h"

typedef struct _CFURLResponse* CFURLResponseRef;

namespace WebCore {

class ResourceResponse : public ResourceResponseBase {
public:
    ResourceResponse()
        : m_responseFired(false)
        , m_isMainResource(false)
    {
    }

    ResourceResponse(const URL& url, const String& mimeType, long long expectedLength, const String& textEncodingName)
        : ResourceResponseBase(url, mimeType, expectedLength, textEncodingName)
        , m_responseFired(false)
        , m_isMainResource(false)
    {
    }

    void setResponseFired(bool fired) { m_responseFired = fired; }
    bool responseFired() { return m_responseFired; }

    void setReplaceUrl(const URL& url) { m_replaceUrl = url; }
    URL& replaceUrl() const { return m_replaceUrl; }

    void setIsMainResource(bool isMain) const { m_isMainResource = isMain; }
    bool isMainResource() const { return m_isMainResource; }

    void clearHttpHeaderFields() { m_httpHeaderFields.clear(); }

    // Needed for compatibility.
    CFURLResponseRef cfURLResponse() const { return 0; }

    bool platformResponseIsUpToDate() const { return false; }

private:
    friend class ResourceResponseBase;

    std::unique_ptr<CrossThreadResourceResponseData> doPlatformCopyData(std::unique_ptr<CrossThreadResourceResponseData> data) const { return data; }
    void doPlatformAdopt(std::unique_ptr<CrossThreadResourceResponseData>) { }
    String platformSuggestedFilename() const
    {
        return filenameFromHTTPContentDisposition(httpHeaderField(HTTPHeaderName::ContentDisposition));
    }

    bool m_responseFired;
    mutable URL m_replaceUrl;
    mutable bool m_isMainResource;
};

struct CrossThreadResourceResponseData : public CrossThreadResourceResponseDataBase {
};

} // namespace WebCore

#endif // ResourceResponse_h
