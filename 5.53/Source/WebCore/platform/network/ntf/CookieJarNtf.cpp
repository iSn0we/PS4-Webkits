/*
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "config.h"
#include "Cookie.h"

#include "NetworkStorageSession.h"
#include "PlatformCookieJar.h"
#include "URL.h"

#include <ntf/CookieJarDB.h>
#include <wtf/text/CString.h>

namespace WebCore {

String cookieRequestHeaderFieldValue(const NetworkStorageSession& session, const URL& /*firstParty*/, const URL& url)
{
    NTF::Cookie::CookieJarDB* cookieJarDB = NTF::Cookie::CookieJarDB::getInstance();
    if (!cookieJarDB || !cookieJarDB->isEnable())
        return "";

    String cookie;

    NTF::Cookie::CookieJarDB::Condition isHttpOnly = NTF::Cookie::CookieJarDB::ALL;
    NTF::Cookie::CookieJarDB::Condition isSecure = url.protocolIs("https") ? NTF::Cookie::CookieJarDB::ALL : NTF::Cookie::CookieJarDB::FALSE_ONLY;
    NTF::Cookie::CookieJarDB::Condition isSession = NTF::Cookie::CookieJarDB::ALL;
    NTF::Cookie::ResultSet* rs = cookieJarDB->searchCookies(url.string().utf8().data(), isHttpOnly, isSecure, isSession);
    if (rs) {
        rs->beforeFirst();
        while (rs->next()) {
            if (!cookie.isEmpty())
                cookie.append("; ");
            cookie.append(rs->get(0));
            cookie.append("=");
            cookie.append(rs->get(1));
        }
        cookieJarDB->closeResultSet(rs);
    }

    return cookie;
}

String cookiesForDOM(const NetworkStorageSession& session, const URL&, const URL& url)
{
    NTF::Cookie::CookieJarDB* cookieJarDB = NTF::Cookie::CookieJarDB::getInstance();
    if (!cookieJarDB || !cookieJarDB->isEnable())
        return "";

    String cookie;

    NTF::Cookie::CookieJarDB::Condition isHttpOnly = NTF::Cookie::CookieJarDB::FALSE_ONLY;
    NTF::Cookie::CookieJarDB::Condition isSecure = url.protocolIs("https") ? NTF::Cookie::CookieJarDB::ALL : NTF::Cookie::CookieJarDB::FALSE_ONLY;
    NTF::Cookie::CookieJarDB::Condition isSession = NTF::Cookie::CookieJarDB::ALL;
    NTF::Cookie::ResultSet* rs = cookieJarDB->searchCookies(url.string().utf8().data(), isHttpOnly, isSecure, isSession);
    if (rs) {
        rs->beforeFirst();
        while (rs->next()) {
            if (!cookie.isEmpty())
                cookie.append("; ");
            cookie.append(rs->get(0));
            cookie.append("=");
            cookie.append(rs->get(1));
        }
        cookieJarDB->closeResultSet(rs);
    }

    return cookie;
}

void setCookiesFromDOM(const NetworkStorageSession&, const URL&, const URL& url, const String& value)
{
    NTF::Cookie::CookieJarDB* cookieJarDB = NTF::Cookie::CookieJarDB::getInstance();
    if (!cookieJarDB || !cookieJarDB->isEnable())
        return;

    cookieJarDB->setCookie(url.string().utf8().data(), value.utf8().data(), true);
}

bool cookiesEnabled(const NetworkStorageSession& session, const URL& /*firstParty*/, const URL& /*url*/)
{
    return true;
}

bool getRawCookies(const NetworkStorageSession&, const URL& firstParty, const URL& url, Vector<Cookie>& rawCookies)
{
    rawCookies.clear();

    NTF::Cookie::CookieJarDB* cookieJarDB = NTF::Cookie::CookieJarDB::getInstance();
    if (!cookieJarDB || !cookieJarDB->isEnable())
        return false;

    NTF::Cookie::ResultSet* rs = cookieJarDB->rawCookies(url.string().utf8().data());
    if (rs) {
        rs->beforeFirst();
        while (rs->next()) {
            const char* name = rs->get(0);
            if (!name || !strlen(name))
                continue;

            const char* domain = rs->get(2);
            if (!domain || !strlen(domain))
                continue;

            String domainStr;
            if (*domain == '.')
                domainStr = String(domain);
            else
                domainStr = String(".") + String(domain);

            rawCookies.append(Cookie(
                String(name),
                String(rs->get(1)),
                domainStr,
                String(rs->get(3)),
                String(rs->get(4)).toDouble() * 1000,
                (*rs->get(5) == '1'),
                (*rs->get(6) == '1'),
                (*rs->get(7) == '1')));
        }
        cookieJarDB->closeResultSet(rs);
    }
    return true;
}

void deleteCookie(const NetworkStorageSession&, const URL& url, const String& deletedCookieName)
{
    NTF::Cookie::CookieJarDB* cookieJarDB = NTF::Cookie::CookieJarDB::getInstance();
    if (!cookieJarDB || !cookieJarDB->isEnable())
        return;

    cookieJarDB->deleteCookie(url.host().latin1().data(), deletedCookieName.latin1().data());
}

#if !PLATFORM(EFL)
void setCookieStoragePrivateBrowsingEnabled(bool enabled)
{
    // FIXME: Not yet implemented
}
#endif

void getHostnamesWithCookies(const NetworkStorageSession& session, HashSet<String>& hostnames)
{
    // FIXME: Not yet implemented
}

void deleteCookiesForHostnames(const NetworkStorageSession&, const Vector<String>& cookieHostNames)
{
    NTF::Cookie::CookieJarDB* cookieJarDB = NTF::Cookie::CookieJarDB::getInstance();
    if (!cookieJarDB || !cookieJarDB->isEnable())
        return;

    for (int i = 0; i < cookieHostNames.size(); i++)
        cookieJarDB->deleteCookies(cookieHostNames[i].latin1().data());
}

void deleteAllCookies(const NetworkStorageSession&)
{
    NTF::Cookie::CookieJarDB* cookieJarDB = NTF::Cookie::CookieJarDB::getInstance();
    if (!cookieJarDB || !cookieJarDB->isEnable())
        return;

    cookieJarDB->deleteAllCookies();
}

void deleteAllCookiesModifiedSince(const NetworkStorageSession&, std::chrono::system_clock::time_point)
{
    // FIXME: Not yet implemented
}

}
