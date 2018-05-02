/*
 * Copyright (C) 2015 Sony Computer Entertainment Inc.
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

#ifndef WKErrorManx_h
#define WKErrorManx_h

#include <WebKit/WKBase.h>

#include <ntf/Error.h>

#ifdef __cplusplus
extern "C" {
#endif

    WK_EXPORT int WKErrorGetSslVerificationResult(WKErrorRef error);
    WK_EXPORT WKStringRef WKErrorCopySslVerificationResultString(WKErrorRef error);

#ifdef __cplusplus
}
#endif

// Additional Return Value Definitions
// NOTE: Following values are also defined in the Manx port's network library.
//       Please be careful about inconsistency between WKErrorManx.h and the library's errors.

// WKErrorGetSslVerificationResult()
// X509_V_ERR_* and the following errors wil be returned. See also: https ://github.com/openssl/openssl/blob/master/include/openssl/x509_vfy.h
#define SCE_WKERROR_SSLVERIFY_NOT_MATCH_COMMONNAME      NTF_SSLVERIFY_ERROR_NO_MATCH_COMMONNAME

// WKErrorGetErrorCode()
// CURLE_* and followig erros will be returned. See also: https://github.com/bagder/curl/blob/master/include/curl/curl.h
#define SCE_WKERROR_NET_CANCELED_IN_BADCERT             NTF_HTTP_ERROR_CANCELED_IN_BADCERT

#endif /* WKErrorManx_h */
