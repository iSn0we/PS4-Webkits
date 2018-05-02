/*
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
#ifndef CredentialTransformDataNtf_h
#define CredentialTransformDataNtf_h

#include "Credential.h"
#include "HTMLFormElement.h"
#include "ProtectionSpace.h"
#include "URL.h"

namespace WebCore {

class HTMLFormElement;

struct CredentialTransformData {
    // If the provided form is suitable for password completion, isValid() will
    // return true;
    CredentialTransformData(const URL&, const ProtectionSpace&, const Credential&);

    // If creation failed, return false.
    bool isValid() const { return m_isValid; }

    URL url() const;
    ProtectionSpace protectionSpace() const { return m_protectionSpace; }
    Credential credential() const;

private:

    URL m_url;
    ProtectionSpace m_protectionSpace;
    mutable Credential m_credential;
    bool m_isValid;
};

} // namespace WebCore

#endif
