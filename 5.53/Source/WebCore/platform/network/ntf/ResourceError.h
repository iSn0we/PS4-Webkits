/*
 * Copyright (C) 2006 Apple Computer, Inc.  All rights reserved.
 * Copyright (C) 2012 Sony Interactive Entertainment Inc.
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

#ifndef ResourceError_h
#define ResourceError_h

#include "ResourceErrorBase.h"
#include <wtf/text/CString.h>

namespace WebCore {

class ResourceError : public ResourceErrorBase {
public:
    ResourceError()
        : ResourceErrorBase()
        , m_sslVerificationResult(0)
        , m_confirm(false)
    {
    }

    ResourceError(const String& domain, int errorCode, const String& failingURL, const String& localizedDescription)
        : ResourceErrorBase(domain, errorCode, failingURL, localizedDescription)
        , m_sslVerificationResult(0)
        , m_confirm(false)
    {
    }

    ResourceError(const String& domain, int errorCode, long sslVerificationResult, const String& failingURL, const String& localizedDescription, const String& sslVerificationResultString)
        : ResourceErrorBase(domain, errorCode, failingURL, localizedDescription)
        , m_sslVerificationResult(sslVerificationResult)
        , m_sslVerificationResultString(sslVerificationResultString)
        , m_confirm(false)
    {
    }

    ResourceError(long sslVerificationResult, const String& failingURL)
        : ResourceErrorBase(String(), 0, failingURL, String())
        , m_sslVerificationResult(sslVerificationResult)
        , m_confirm(false)
    {
    }

    void platformCopy(ResourceError& errorCopy) const
    {
        errorCopy.m_sslVerificationResult = m_sslVerificationResult;
        errorCopy.m_confirm = m_confirm;
        errorCopy.m_certs = m_certs;
    }

    void setCerts(Vector<CString>& certs) { m_certs = certs; }
    Vector<CString>& certs() const { return m_certs; }

    void setConfirmCert(bool confirm) const { m_confirm = confirm; }
    bool confirmCert() { return m_confirm; }

    void setSslVerificationResult(long sslVerificationResult) { m_sslVerificationResult = sslVerificationResult; }
    long sslVerificationResult() const { return m_sslVerificationResult; }
    
    void setSslVerificationResultString(const String& sslVerificationResultString) { m_sslVerificationResultString = sslVerificationResultString; }
    const String& sslVerificationResultString() const { return m_sslVerificationResultString; }

private:
    long m_sslVerificationResult;
    String m_sslVerificationResultString;
    mutable bool m_confirm;
    mutable Vector<CString> m_certs;
};

}

#endif // ResourceError_h_
