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
#include "config.h"
#include "CredentialBackingStoreNtf.h"

#include "CredentialStorage.h"
#include "FileSystem.h"
#include "NotImplemented.h"
#include "ProtectionSpaceHash.h"
#include "SQLiteStatement.h"
#include "URL.h"

#define HANDLE_SQL_EXEC_FAILURE(statement, returnValue, ...) \
    if (statement) \
    { \
        LOG_ERROR(__VA_ARGS__); \
        return returnValue; \
    }

namespace WebCore {

CredentialBackingStore* CredentialBackingStore::instance()
{
    static CredentialBackingStore* backingStore = 0;
    if (!backingStore) {
        backingStore = new CredentialBackingStore;
        // TODO:
        // Currently use on memory authentication cache.
        //
        backingStore->open(String(":memory:"));
    }
    return backingStore;
}

CredentialBackingStore::CredentialBackingStore()
    : m_addLoginStatement(0)
    , m_updateLoginStatement(0)
    , m_getLoginStatement(0)
    , m_getLoginByURLStatement(0)
    , m_getTypeSchemeByURLStatement(0)
    , m_removeLoginStatement(0)
{
}

CredentialBackingStore::~CredentialBackingStore()
{
    if (m_database.isOpen())
        m_database.close();
}

bool CredentialBackingStore::open(const String& dbPath)
{
    ASSERT(!m_database.isOpen());

    HANDLE_SQL_EXEC_FAILURE(!m_database.open(dbPath), false,
        "Failed to open database file %s for login database", dbPath.utf8().data());

    m_database.disableThreadingChecks();

    if (!m_database.tableExists("logins")) {
        HANDLE_SQL_EXEC_FAILURE(!m_database.executeCommand("CREATE TABLE logins (origin_url VARCHAR NOT NULL, host VARCHAR NOT NULL, port INTEGER, service_type INTEGER NOT NULL, realm VARCHAR, auth_scheme INTEGER NOT NULL, username VARCHAR, password BLOB) "),
            false, "Failed to create table logins for login database");

        // Create index for table logins.
        HANDLE_SQL_EXEC_FAILURE(!m_database.executeCommand("CREATE INDEX logins_index ON logins (host)"),
            false, "Failed to create index for table logins");
    }

    if (!m_database.tableExists("never_remember")) {
        HANDLE_SQL_EXEC_FAILURE(!m_database.executeCommand("CREATE TABLE never_remember (origin_url VARCHAR NOT NULL, host VARCHAR NOT NULL, port INTEGER, service_type INTEGER NOT NULL, realm VARCHAR, auth_scheme INTEGER NOT NULL) "),
            false, "Failed to create table never_remember for login database");

        // Create index for table never_remember.
        HANDLE_SQL_EXEC_FAILURE(!m_database.executeCommand("CREATE INDEX never_remember_index ON never_remember (host)"),
            false, "Failed to create index for table never_remember");
    }

    // Prepare the statements.
    m_addLoginStatement = new SQLiteStatement(m_database, "INSERT OR REPLACE INTO logins (origin_url, host, port, service_type, realm, auth_scheme, username, password) VALUES (?, ?, ?, ?, ?, ?, ?, ?)");
    HANDLE_SQL_EXEC_FAILURE(m_addLoginStatement->prepare() != SQLITE_OK,
        false, "Failed to prepare addLogin statement");

    m_updateLoginStatement = new SQLiteStatement(m_database, "UPDATE logins SET username = ?, password = ? WHERE origin_url = ? AND host = ? AND port = ? AND service_type = ? AND realm = ? AND auth_scheme = ?");
    HANDLE_SQL_EXEC_FAILURE(m_updateLoginStatement->prepare() != SQLITE_OK,
        false, "Failed to prepare updateLogin statement");

    m_getLoginStatement = new SQLiteStatement(m_database, "SELECT username, password FROM logins WHERE host = ? AND port = ? AND service_type = ? AND realm = ? AND auth_scheme = ?");
    HANDLE_SQL_EXEC_FAILURE(m_getLoginStatement->prepare() != SQLITE_OK,
        false, "Failed to prepare getLogin statement");

    m_getLoginByURLStatement = new SQLiteStatement(m_database, "SELECT username, password FROM logins WHERE origin_url = ?");
    HANDLE_SQL_EXEC_FAILURE(m_getLoginByURLStatement->prepare() != SQLITE_OK,
        false, "Failed to prepare getLoginByURL statement");

    m_getTypeSchemeByURLStatement = new SQLiteStatement(m_database, "SELECT service_type, auth_scheme FROM logins WHERE origin_url = ?");
    HANDLE_SQL_EXEC_FAILURE(m_getTypeSchemeByURLStatement->prepare() != SQLITE_OK,
        false, "Failed to prepare getTypeSchemeByURL statement");

    m_removeLoginStatement = new SQLiteStatement(m_database, "DELETE FROM logins WHERE host = ? AND port = ? AND service_type = ? AND realm = ? AND auth_scheme = ?");
    HANDLE_SQL_EXEC_FAILURE(m_removeLoginStatement->prepare() != SQLITE_OK,
        false, "Failed to prepare removeLogin statement");

    return true;
}

void CredentialBackingStore::close()
{
    delete m_addLoginStatement;
    m_addLoginStatement = 0;
    delete m_updateLoginStatement;
    m_updateLoginStatement = 0;
    delete m_getLoginStatement;
    m_getLoginStatement = 0;
    delete m_getLoginByURLStatement;
    m_getLoginByURLStatement = 0;
    delete m_getTypeSchemeByURLStatement;
    m_getTypeSchemeByURLStatement = 0;
    delete m_removeLoginStatement;
    m_removeLoginStatement = 0;

    if (m_database.isOpen())
        m_database.close();
}

bool CredentialBackingStore::addLogin(const URL& url, const ProtectionSpace& protectionSpace, const Credential& credential)
{
    ASSERT(m_database.isOpen());
    ASSERT(m_database.tableExists("logins"));

    if (!m_addLoginStatement)
        return false;

    m_addLoginStatement->bindText(1, url.string());
    m_addLoginStatement->bindText(2, protectionSpace.host());
    m_addLoginStatement->bindInt(3, protectionSpace.port());
    m_addLoginStatement->bindInt(4, static_cast<int>(protectionSpace.serverType()));
    m_addLoginStatement->bindText(5, protectionSpace.realm());
    m_addLoginStatement->bindInt(6, static_cast<int>(protectionSpace.authenticationScheme()));
    m_addLoginStatement->bindText(7, credential.user());
    m_addLoginStatement->bindBlob(8, encryptedString(credential.password()));

    int result = m_addLoginStatement->step();
    m_addLoginStatement->reset();
    HANDLE_SQL_EXEC_FAILURE(result != SQLITE_DONE, false,
        "Failed to add login info into table logins - %i", result);

    return true;
}

bool CredentialBackingStore::updateLogin(const URL& url, const ProtectionSpace& protectionSpace, const Credential& credential)
{
    ASSERT(m_database.isOpen());
    ASSERT(m_database.tableExists("logins"));

    if (!m_updateLoginStatement)
        return false;

    m_updateLoginStatement->bindText(1, credential.user());
    m_updateLoginStatement->bindBlob(2, encryptedString(credential.password()));
    m_updateLoginStatement->bindText(3, url.string());
    m_updateLoginStatement->bindText(4, protectionSpace.host());
    m_updateLoginStatement->bindInt(5, protectionSpace.port());
    m_updateLoginStatement->bindInt(6, static_cast<int>(protectionSpace.serverType()));
    m_updateLoginStatement->bindText(7, protectionSpace.realm());
    m_updateLoginStatement->bindInt(8, static_cast<int>(protectionSpace.authenticationScheme()));

    int result = m_updateLoginStatement->step();
    m_updateLoginStatement->reset();
    HANDLE_SQL_EXEC_FAILURE(result != SQLITE_DONE, false,
        "Failed to update login info in table logins - %i", result);

    return true;
}

Credential CredentialBackingStore::getLogin(const ProtectionSpace& protectionSpace)
{
    ASSERT(m_database.isOpen());
    ASSERT(m_database.tableExists("logins"));

    if (!m_getLoginStatement)
        return Credential();

    m_getLoginStatement->bindText(1, protectionSpace.host());
    m_getLoginStatement->bindInt(2, protectionSpace.port());
    m_getLoginStatement->bindInt(3, static_cast<int>(protectionSpace.serverType()));
    m_getLoginStatement->bindText(4, protectionSpace.realm());
    m_getLoginStatement->bindInt(5, static_cast<int>(protectionSpace.authenticationScheme()));

    String username;
    String password;
    int result = m_getLoginStatement->step();
    if (result == SQLITE_ROW) {
        username = m_getLoginStatement->getColumnText(0);
        password = m_getLoginStatement->getColumnBlobAsString(1);
    }

    m_getLoginStatement->reset();

    return Credential(username, decryptedString(password), CredentialPersistencePermanent);
}

Credential CredentialBackingStore::getLogin(const URL& url)
{
    ASSERT(m_database.isOpen());
    ASSERT(m_database.tableExists("logins"));

    if (!m_getLoginByURLStatement)
        return Credential();

    m_getLoginByURLStatement->bindText(1, url.string());

    String username;
    String password;
    int result = m_getLoginByURLStatement->step();
    if (result == SQLITE_ROW) {
        username = m_getLoginByURLStatement->getColumnText(0);
        password = m_getLoginByURLStatement->getColumnBlobAsString(1);
    }
    m_getLoginByURLStatement->reset();

    return Credential(username, decryptedString(password), CredentialPersistencePermanent);
}

Credential CredentialBackingStore::getLogin(const URL& url, ProtectionSpaceServerType& type, ProtectionSpaceAuthenticationScheme& scheme)
{
    ASSERT(m_database.isOpen());
    ASSERT(m_database.tableExists("logins"));

    if (!m_getLoginByURLStatement || !m_getTypeSchemeByURLStatement)
        return Credential();

    m_getLoginByURLStatement->bindText(1, url.string());

    String username;
    String password;
    int result = m_getLoginByURLStatement->step();
    if (result == SQLITE_ROW) {
        username = m_getLoginByURLStatement->getColumnText(0);
        password = m_getLoginByURLStatement->getColumnBlobAsString(1);
    }
    m_getLoginByURLStatement->reset();

    m_getTypeSchemeByURLStatement->bindText(1, url.string());

    result = m_getTypeSchemeByURLStatement->step();
    if (result == SQLITE_ROW) {
        type = static_cast<ProtectionSpaceServerType>(m_getTypeSchemeByURLStatement->getColumnInt(0));
        scheme = static_cast<ProtectionSpaceAuthenticationScheme>(m_getTypeSchemeByURLStatement->getColumnInt(1));
    } else {
        type = ProtectionSpaceServerHTTP;
        scheme = ProtectionSpaceAuthenticationSchemeDefault;
    }
    m_getTypeSchemeByURLStatement->reset();

    return Credential(username, decryptedString(password), CredentialPersistencePermanent);
}

bool CredentialBackingStore::removeLogin(const ProtectionSpace& protectionSpace)
{
    ASSERT(m_database.isOpen());
    ASSERT(m_database.tableExists("logins"));

    if (!m_removeLoginStatement)
        return false;

    m_removeLoginStatement->bindText(1, protectionSpace.host());
    m_removeLoginStatement->bindInt(2, protectionSpace.port());
    m_removeLoginStatement->bindInt(3, static_cast<int>(protectionSpace.serverType()));
    m_removeLoginStatement->bindText(4, protectionSpace.realm());
    m_removeLoginStatement->bindInt(5, static_cast<int>(protectionSpace.authenticationScheme()));

    int result = m_removeLoginStatement->step();
    HANDLE_SQL_EXEC_FAILURE(result != SQLITE_DONE, false,
        "Failed to remove login info in table logins - %i", result);
    m_removeLoginStatement->reset();

    return true;
}

String CredentialBackingStore::encryptedString(const String& plainText) const
{
    // TODO:
    // replace with encrypted String.
    return plainText;
}

String CredentialBackingStore::decryptedString(const String& cipherText) const
{
    // TODO:
    // replace decrypted String.
    return cipherText;
}

} // namespace WebCore
