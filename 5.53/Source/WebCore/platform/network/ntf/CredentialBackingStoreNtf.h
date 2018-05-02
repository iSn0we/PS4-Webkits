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
#ifndef CredentialBackingStoreNtf_h
#define CredentialBackingStoreNtf_h

#include "Credential.h"
#include "ProtectionSpace.h"
#include "SQLiteDatabase.h"

namespace WebCore {

class URL;
class ProtectionSpace;

class CredentialBackingStore {
public:
    static CredentialBackingStore* instance();
    ~CredentialBackingStore();
    bool open(const String& dbPath);
    void close();
    bool addLogin(const URL&, const ProtectionSpace&, const Credential&);
    bool updateLogin(const URL&, const ProtectionSpace&, const Credential&);
    Credential getLogin(const ProtectionSpace&);
    Credential getLogin(const URL&);
    Credential getLogin(const URL&, ProtectionSpaceServerType&, ProtectionSpaceAuthenticationScheme&);
    bool removeLogin(const ProtectionSpace&);

private:
    CredentialBackingStore();
    String encryptedString(const String& plainText) const;
    String decryptedString(const String& cipherText) const;

    SQLiteDatabase m_database;
    SQLiteStatement* m_addLoginStatement;
    SQLiteStatement* m_updateLoginStatement;
    SQLiteStatement* m_getLoginStatement;
    SQLiteStatement* m_getLoginByURLStatement;
    SQLiteStatement* m_getTypeSchemeByURLStatement;
    SQLiteStatement* m_removeLoginStatement;
};

} // namespace WebCore

#endif // CredentialBackingStoreNtf_h
