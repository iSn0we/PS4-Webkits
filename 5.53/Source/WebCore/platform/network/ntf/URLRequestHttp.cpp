/*
 * Copyright (C) 2017 Sony Interactive Entertainment Inc.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "URLRequestHttp.h"

#include "CredentialBackingStoreNtf.h"
#include "CredentialTransformDataNtf.h"
#include "HTTPParsers.h"
#include "MIMETypeRegistry.h"
#include "ResourceHandle.h"
#include "ResourceHandleClient.h"
#include "ResourceHandleInternal.h"
#include "ResourceRequest.h"

// #define LISTENER_TRACE_ON

#if defined(LISTENER_TRACE_ON)
#define TRACE(...)   { printf("[Listener 0x%p] ", this); printf(__VA_ARGS__); }
#else
#define TRACE(...) ((void)0)
#endif

namespace WebCore {

// URLRequestHttpFactory ------------------------------------------------------

NTF::URLRequestMessage* URLRequestHttpFactory::createRequestMessage(ResourceRequest& resourceRequest, ResourceHandleInternal* d)
{
    // Prepare request message

    bool isMainResource = resourceRequest.isMainResource();

    URL requestUrl = resourceRequest.url();
    String httpMethod = resourceRequest.httpMethod();

    // Create RequestMessage

    NTF::URLRequestHttpMessage* message = NTF::URLRequestHttpMessage::create();
    ASSERT(message);

    message->setIsMainResource(isMainResource);
    message->setSchedulerId(1);
    // url is in ASCII so latin1() will only convert it to char* without character translation.
    if (!requestUrl.isEmpty())
        message->setUrl(requestUrl.string().latin1().data());
    message->setHttpMethod(httpMethod.latin1().data());

    // Header Field
    if (resourceRequest.httpHeaderFields().size() > 0) {
        HTTPHeaderMap customHeaders = resourceRequest.httpHeaderFields();
        HTTPHeaderMap::const_iterator end = customHeaders.end();
        for (HTTPHeaderMap::const_iterator it = customHeaders.begin(); it != end; ++it) {
            String key = it->key;
            String value = it->value;

            message->appendHeaderStrings(key.latin1().data(), value.latin1().data());
        }
    }

    // Body (Form) data
    RefPtr<FormData> formData = resourceRequest.httpBody();

    if (formData) {
        // Resolve the blob elements so the formData can correctly report it's size.
        formData = formData->resolveBlobReferences();
        resourceRequest.setHTTPBody(formData);

        // Setup form data.
        size_t numElements = formData->elements().size();

        for (size_t i = 0; i < numElements; i++) {
            const FormDataElement& element = formData->elements()[i];

            if (element.m_type == FormDataElement::Type::EncodedFile)
                message->formData()->appendFile(element.m_filename.utf8().data());
            else
                message->formData()->appendData(element.m_data.data(), element.m_data.size());
        }
    }

    // Timeout interval
    long timeoutMilliSeconds = 0;
    double timeoutSeconds = resourceRequest.timeoutInterval();
    if (timeoutSeconds > 0)
        timeoutMilliSeconds = static_cast<long>(timeoutSeconds * 1000.0);

    message->setTransferTimeoutIntervalMilliseconds(timeoutMilliSeconds);

    // HTTP Authentication
    ProtectionSpaceServerType protectServerType;
    ProtectionSpaceAuthenticationScheme protectAuthScheme;
    WebCore::Credential savedHttpCredential = CredentialBackingStore::instance()->getLogin(requestUrl, protectServerType, protectAuthScheme);

    if (!savedHttpCredential.isEmpty()) {
        NTF::AuthenticationScheme authScheme = NTF::AuthenticationSchemeHTTPDigest;

        if (protectAuthScheme == ProtectionSpaceAuthenticationSchemeHTTPBasic)
            authScheme = NTF::AuthenticationSchemeHTTPBasic;

        String user = savedHttpCredential.user();
        String password = savedHttpCredential.password();

        if (message->httpAuthCredential())
            message->httpAuthCredential()->setCredential(authScheme, user.latin1().data(), password.latin1().data());
    }

    // Web timing API
#if ENABLE(WEB_TIMING)
    message->setRequestStartTime(monotonicallyIncreasingTime());
#endif

    // Set response ------------------

    d->m_response.setIsMainResource(isMainResource);

    return static_cast<NTF::URLRequestMessage*>(message);
}



// URLRequestHttpListener -----------------------------------------------------

URLRequestHttpListener::URLRequestHttpListener(ResourceHandle* resourceHandle)
    : URLRequestListenerNtf(resourceHandle)
{

}

URLRequestHttpListener::~URLRequestHttpListener()
{

}

bool URLRequestHttpListener::didReceiveResponseMessage(NTF::URLResponseMessage* responseMessage)
{
    ASSERT(responseMessage);

    bool canReleaseHandle = false;

    switch (responseMessage->getMessageType()) {
    case NTF::URLResponseMessage::HttpResponseSendDataProgress :
        canReleaseHandle = didReceiveResponseSendDataProgressMessage(static_cast<NTF::URLRequestHttpSendDataProgressMessage*>(responseMessage));
        break;

    case NTF::URLResponseMessage::HttpResponseHeaderFields :
        canReleaseHandle = didReceiveResponseHeaderFieldsMessage(static_cast<NTF::URLResponseHttpHeaderFieldsMessage*>(responseMessage));
        break;

    case NTF::URLResponseMessage::HttpResponseHeader :
        canReleaseHandle = didReceiveResponseHeaderMessage(static_cast<NTF::URLResponseHttpHeaderMessage*>(responseMessage));
        break;

    case NTF::URLResponseMessage::HttpResponseBody :
        canReleaseHandle = didReceiveResponseBodyMessage(static_cast<NTF::URLResponseHttpBodyMessage*>(responseMessage));
        break;

    case NTF::URLResponseMessage::HttpResponseRedirect :
        canReleaseHandle = didReceiveResponseRedirectMessage(static_cast<NTF::URLResponseHttpRedirectMessage*>(responseMessage));
        break;

    case NTF::URLResponseMessage::HttpResponseFinish :
        canReleaseHandle = didReceiveResponseFinishMessage(static_cast<NTF::URLResponseHttpFinishMessage*>(responseMessage));
        break;

    case NTF::URLResponseMessage::HttpResponseFail :
        canReleaseHandle = didReceiveResponseFailMessage(static_cast<NTF::URLResponseHttpFailMessage*>(responseMessage));
        break;

    case NTF::URLResponseMessage::HttpAuthenticationChallenge :
        canReleaseHandle = didReceiveAuthenticationChallengeMessage(static_cast<NTF::URLResponseHttpAuthenticationChallengeMessage*>(responseMessage));
        break;

    case NTF::URLResponseMessage::HttpAuthenticationSuccess :
        canReleaseHandle = didReceiveAuthenticationSuccessMessage(static_cast<NTF::URLResponseHttpAuthenticationSuccessMessage*>(responseMessage));
        break;

    case NTF::URLResponseMessage::HttpSslCertError :
        canReleaseHandle = didReceiveSslCertErrorMessage(static_cast<NTF::URLResponseHttpSslCertErrorMessage*>(responseMessage));
        break;

    case NTF::URLResponseMessage::HttpTimingInformation :
        canReleaseHandle = didReceiveTimingInformationMessage(static_cast<NTF::URLResponseHttpTimingInformationMessage*>(responseMessage));
        break;

    default:
        ASSERT(false);
    }

    return canReleaseHandle;
}


// Request body written -----------------------------------

bool URLRequestHttpListener::didReceiveResponseSendDataProgressMessage(NTF::URLRequestHttpSendDataProgressMessage* message)
{
    TRACE("[IN] %s(%d)\n", __FUNCTION__, __LINE__);

    ResourceHandle* resourceHandle = getResourceHandle();
    ASSERT(resourceHandle);
    ResourceHandleInternal* d = resourceHandle->getInternal();
    ASSERT(d);
    ResourceHandleClient* client = d->client();

    if (d->m_cancelled)
        return false;

    if (client)
        client->didSendData(resourceHandle, message->bytesSent(), message->totalBytesToBeSent());

    return false;
}


// Response Header Field ----------------------------------

bool URLRequestHttpListener::didReceiveResponseHeaderFieldsMessage(NTF::URLResponseHttpHeaderFieldsMessage* message)
{
    TRACE("[IN] %s(%d)\n", __FUNCTION__, __LINE__);

    ResourceHandle* resourceHandle = getResourceHandle();
    ASSERT(resourceHandle);
    ResourceHandleInternal* d = resourceHandle->getInternal();
    ASSERT(d);

    if (d->m_cancelled)
        return false;

    String url(message->url());
    long responseCode = message->responseCode();
    long long contentLength = message->contentLength();
    int headerFieldSize = message->headerFieldSize();

    TRACE("url = %s\n", url.latin1().data());
    TRACE("responseCode = %ld\n", responseCode);
    TRACE("contentLength = %lld\n", contentLength);
    TRACE("headerFieldSize = %d\n", headerFieldSize);

    switch (message->source()) {
    case NTF::URLResponseHttpHeaderFieldsMessage::Source::Network:
        d->m_response.setSource(WebCore::ResourceResponseBase::Source::Network);
        break;
    case NTF::URLResponseHttpHeaderFieldsMessage::Source::DiskCache:
        d->m_response.setSource(WebCore::ResourceResponseBase::Source::DiskCache);
        break;
    case NTF::URLResponseHttpHeaderFieldsMessage::Source::DiskCacheAfterValidation:
        d->m_response.setSource(WebCore::ResourceResponseBase::Source::DiskCacheAfterValidation);
        break;
    default:
        d->m_response.setSource(WebCore::ResourceResponseBase::Source::Unknown);
        break;
    }

#if ENABLE(MANX_ADVANCED_TIMING_API)
    d->m_response.resourceLoadTiming().sceSourceType = d->m_response.sourceString();
#endif

    d->m_response.clearHttpHeaderFields();

    for (int headerFieldPos = 0; headerFieldPos < headerFieldSize; headerFieldPos++) {
        const void* data = 0;
        size_t dataSize = 0;

        message->headerField(headerFieldPos, &data, &dataSize);
        if (!data)
            continue;

        String header = String::fromUTF8WithLatin1Fallback(static_cast<const char*>(data), dataSize);
        TRACE("headerField = %s\n", header.stripWhiteSpace().latin1().data());

        int splitPos = header.find(':');
        if (splitPos != -1) {
            String key = header.left(splitPos).stripWhiteSpace();
            String value = header.substring(splitPos + 1).stripWhiteSpace();

            // TODO : See curl port.
            // if (isAppendableHeader(key))
            //     d->m_response.addHTTPHeaderField(key, value);

            d->m_response.setHTTPHeaderField(key, value);
        } else if (header.startsWith("HTTP", false)) {
            // This is the first line of the response.
            // Extract the http status text from this.

            String responseCodeString = String::number(responseCode);
            int responseCodePos = header.find(responseCodeString);

            if (responseCodePos != -1) {
                // The status text is after the status code.
                String status = header.substring(responseCodePos + responseCodeString.length());
                d->m_response.setHTTPStatusText(status.stripWhiteSpace());
            }
        }
    }

    d->m_response.setURL(URL(URL(), url));
    d->m_response.setHTTPStatusCode(responseCode);
    d->m_response.setExpectedContentLength(contentLength);

    d->m_response.setMimeType(extractMIMETypeFromMediaType(d->m_response.httpHeaderField(HTTPHeaderName::ContentType)).lower());
    d->m_response.setTextEncodingName(extractCharsetFromMediaType(d->m_response.httpHeaderField(HTTPHeaderName::ContentType)));

    return false;
}


// Response Header ----------------------------------------

bool URLRequestHttpListener::didReceiveResponseHeaderMessage(NTF::URLResponseHttpHeaderMessage* message)
{
    TRACE("[IN] %s(%d)\n", __FUNCTION__, __LINE__);

    ResourceHandle* resourceHandle = getResourceHandle();
    ASSERT(resourceHandle);
    ResourceHandleInternal* d = resourceHandle->getInternal();
    ASSERT(d);
    ResourceHandleClient* client = d->client();

    if (d->m_cancelled)
        return false;

    if (client)
        client->didReceiveResponse(resourceHandle, d->m_response);

    return false;
}


// Response Body ------------------------------------------

bool URLRequestHttpListener::didReceiveResponseBodyMessage(NTF::URLResponseHttpBodyMessage* message)
{
    TRACE("[IN] %s(%d)\n", __FUNCTION__, __LINE__);

    ResourceHandle* resourceHandle = getResourceHandle();
    ASSERT(resourceHandle);
    ResourceHandleInternal* d = resourceHandle->getInternal();
    ASSERT(d);
    ResourceHandleClient* client = d->client();

    if (d->m_cancelled)
        return false;

    const char* data = static_cast<const char*>(message->data());
    int dataSize = static_cast<int>(message->dataSize());

    if (client)
        client->didReceiveData(resourceHandle, data, dataSize, dataSize);

    return false;
}


// Response Redirect --------------------------------------

bool URLRequestHttpListener::didReceiveResponseRedirectMessage(NTF::URLResponseHttpRedirectMessage* message)
{
    TRACE("[IN] %s(%d)\n", __FUNCTION__, __LINE__);

    ResourceHandle* resourceHandle = getResourceHandle();
    ASSERT(resourceHandle);
    ResourceHandleInternal* d = resourceHandle->getInternal();
    ASSERT(d);
    ResourceHandleClient* client = d->client();

    NTF::RedirectHandler* redirectHandler = message->redirectHandler();

    if (d->m_cancelled) {
        if (redirectHandler)
            redirectHandler->cancelRedirectProcess();
        return false;
    }

    URL url(URL(), message->url());
    String location(message->location());
    String httpMethod(message->httpMethod());
    bool shouldRewriteUrl(message->shouldRewriteUrl());

    ResourceRequest newRequest = resourceHandle->firstRequest();
    URL newUrl(url, location);

    // Handling of fragment identifiers in redirected URLs
    // See also:
    //   http://www.w3.org/Protocols/HTTP/Fragment/draft-bos-http-redirect-00.txt
    //   https://bugs.webkit.org/show_bug.cgi?id=24175
    if (newUrl.protocolIsInHTTPFamily()
        && resourceHandle->firstRequest().url().hasFragmentIdentifier()
        && !newUrl.hasFragmentIdentifier())
        newUrl.setFragmentIdentifier(resourceHandle->firstRequest().url().fragmentIdentifier());

    newRequest.setURL(newUrl);

    if ((newRequest.httpMethod() != "GET") && (httpMethod == "GET")) {
        // Change newRequest method to GET if change was made during a previous redirection
        // or if current redirection says so
        newRequest.setHTTPMethod("GET");
        newRequest.setHTTPBody(0);
        newRequest.clearHTTPContentType();
    }

    // Should not set Referer after a redirect from a secure resource to non-secure one.
    if (!newUrl.protocolIs("https") && protocolIs(newRequest.httpReferrer(), "https") && resourceHandle->context()->shouldClearReferrerOnHTTPSToHTTPRedirect())
        newRequest.clearHTTPReferrer();

    d->m_user = newUrl.user();
    d->m_pass = newUrl.pass();
    newRequest.removeCredentials();

    if (!protocolHostAndPortAreEqual(resourceHandle->firstRequest().url(), newUrl)) {
        // When a redirected location is different from the original request's host and port,
        // the authorization and origin headers are cleared to avoid reusing those headers.
        newRequest.clearHTTPAuthorization();
        newRequest.clearHTTPOrigin();
    }

    // newRequest might be changed after invoking willSendRequest().
    // If newRequest is cancelled or newRequest's URL is null, the request is regarded as canceled.
    if (client)
        client->willSendRequest(resourceHandle, newRequest, d->m_response);

    if (shouldRewriteUrl)
        d->m_firstRequest.setURL(newRequest.url());

    if (redirectHandler) {
        if (!d->m_cancelled && !newRequest.url().isEmpty())
            redirectHandler->proceedRedirectProcess(URLRequestHttpFactory::createRequestMessage(newRequest, resourceHandle->getInternal()));
        else
            redirectHandler->cancelRedirectProcess();
    }

    return false;
}


// Response Finish ----------------------------------------

bool URLRequestHttpListener::didReceiveResponseFinishMessage(NTF::URLResponseHttpFinishMessage* message)
{
    TRACE("[IN] %s(%d)\n", __FUNCTION__, __LINE__);

    ResourceHandle* resourceHandle = getResourceHandle();
    ASSERT(resourceHandle);
    ResourceHandleInternal* d = resourceHandle->getInternal();
    ASSERT(d);
    ResourceHandleClient* client = d->client();

    if (!d->m_cancelled) {
        if (client)
#if !ENABLE(WEB_TIMING)
            client->didFinishLoading(resourceHandle, 0);
#else
            client->didFinishLoading(resourceHandle, monotonicallyIncreasingTime());
#endif
    }

    return true;
}


// Response Fail ------------------------------------------

bool URLRequestHttpListener::didReceiveResponseFailMessage(NTF::URLResponseHttpFailMessage* message)
{
    TRACE("[IN] %s(%d)\n", __FUNCTION__, __LINE__);

    ResourceHandle* resourceHandle = getResourceHandle();
    ASSERT(resourceHandle);
    ResourceHandleInternal* d = resourceHandle->getInternal();
    ASSERT(d);
    ResourceHandleClient* client = d->client();

    // Memo : In this case, we does not check the cancelled flag.
    //        We will notify the cancellation using the ResourceError.

    String errorDomain = ""; // Nothing is defined for 'errorDomain'.
    int errorCode = static_cast<int>(message->resultCode());
    long sslVerificationResult = message->sslVerificationResult();
    String failingURL(message->url());
    String localizedDescription(message->resultString());
    String sslVerificationResultString(message->sslVerificationResultString());

    ResourceError resourceError(errorDomain, errorCode, sslVerificationResult, failingURL, localizedDescription, sslVerificationResultString);

    if (message->isCancellation())
        resourceError.setIsCancellation(true);

    if (message->isTimeout())
        resourceError.setIsTimeout(true);

    if (!d->m_cancelled) {
        if (client)
            client->didFail(resourceHandle, resourceError);
    }

    return true;
}


// Authentication Challenge -------------------------------

bool URLRequestHttpListener::didReceiveAuthenticationChallengeMessage(NTF::URLResponseHttpAuthenticationChallengeMessage* message)
{
    TRACE("[IN] %s(%d)\n", __FUNCTION__, __LINE__);
    ResourceHandle* resourceHandle = getResourceHandle();
    ASSERT(resourceHandle);
    ResourceHandleInternal* d = resourceHandle->getInternal();
    ASSERT(d);

    URL url(URL(), String(message->url()));
    NTF::AuthenticationChallengeHandler* challengeHandler = message->authenticationChallengeHandler();
    NTF::AuthenticationTarget authTarget = message->authenticationTarget();
    NTF::AuthenticationScheme authScheme = message->authenticationScheme();
    const char* realm = message->realm();

    if (d->m_cancelled) {
        if (challengeHandler)
            challengeHandler->didReceiveAuthenticationCancellation();
        return false;
    }

    // Preapare a ProtectionSpace.
    ProtectionSpaceServerType serverType = ProtectionSpaceServerHTTP;
    if (authTarget == NTF::AuthenticationTargetHttp) {
        if (url.protocolIs("https"))
            serverType = ProtectionSpaceServerHTTPS;
    } else
        serverType = ProtectionSpaceProxyHTTP;

    ProtectionSpaceAuthenticationScheme protectAuthScheme = ProtectionSpaceAuthenticationSchemeHTTPDigest;
    if (authScheme == NTF::AuthenticationSchemeHTTPBasic)
        protectAuthScheme = ProtectionSpaceAuthenticationSchemeHTTPBasic;

    ProtectionSpace space = ProtectionSpace(url.host(), url.port(), serverType, realm, protectAuthScheme);

    // Remove credential if authentication failed
    if (message->challengeCount())
        CredentialBackingStore::instance()->removeLogin(space);

    // Get credential from cache or user.
    Credential savedCredential = CredentialBackingStore::instance()->getLogin(space);
    if (!savedCredential.isEmpty()) {
        // Get a credential from cache.
        if (challengeHandler)
            challengeHandler->didReceiveAuthenticationCredential(authTarget, authScheme, savedCredential.user().latin1().data(), savedCredential.password().latin1().data());
    } else {
        // Get a credential from user.
        AuthenticationChallenge challenge;
        Credential cred;
        ResourceResponse resp;
        ResourceError err;

        challenge = WebCore::AuthenticationChallenge(space, cred, 0, resp, err);
        challenge.m_authenticationChallengeHandler = challengeHandler;
        challenge.m_authTarget = authTarget;
        challenge.m_authScheme = authScheme;

        // Ask the user to enter authentication credential.
        resourceHandle->didReceiveAuthenticationChallenge(challenge);
    }

    return false;
}


// Authentication Success ---------------------------------

bool URLRequestHttpListener::didReceiveAuthenticationSuccessMessage(NTF::URLResponseHttpAuthenticationSuccessMessage* message)
{
    TRACE("[IN] %s(%d)\n", __FUNCTION__, __LINE__);

    ResourceHandle* resourceHandle = getResourceHandle();
    ASSERT(resourceHandle);
    ResourceHandleInternal* d = resourceHandle->getInternal();
    ASSERT(d);

    if (d->m_cancelled)
        return false;

    URL url(URL(), String(message->url()));
    const ProtectionSpace& space = d->m_currentWebChallenge.protectionSpace();

    Credential inputCredential(message->authenticationCredential().username(), message->authenticationCredential().password(), CredentialPersistencePermanent);
    CredentialTransformData data = CredentialTransformData(url, space, inputCredential);

    Credential newSavedCredential = CredentialBackingStore::instance()->getLogin(data.url());
    if (newSavedCredential != data.credential()) {
        if (newSavedCredential.isEmpty())
            CredentialBackingStore::instance()->addLogin(url, data.protectionSpace(), data.credential());
        else
            CredentialBackingStore::instance()->updateLogin(url, data.protectionSpace(), data.credential());
    }

    return false;
}


// Ssl Cert Error -----------------------------------------

bool URLRequestHttpListener::didReceiveSslCertErrorMessage(NTF::URLResponseHttpSslCertErrorMessage* message)
{
    TRACE("[IN] %s(%d)\n", __FUNCTION__, __LINE__);

    ResourceHandle* resourceHandle = getResourceHandle();
    ASSERT(resourceHandle);
    ResourceHandleInternal* d = resourceHandle->getInternal();
    ASSERT(d);
    NetworkingContext* context = resourceHandle->context();
    ASSERT(context);

    NTF::SslCertErrorHandler* sslCertErrorHandler = message->sslCertErrorHandler();
    bool isMainResource = message->isMainResource();

    if (d->m_cancelled) {
        if (sslCertErrorHandler)
            sslCertErrorHandler->cancelProcess();
        return false;
    }

    bool isProceed = false;

    if (isMainResource) {
        ResourceError error(message->sslVerificationResult(), String(message->url()));

        Vector<CString> pems;
        for (int i = 0; i < message->serverCertChainLength(); i++) {
            const char* pem = message->serverCertChainPems(i);
            if (pem)
                pems.append(pem);
        }

        error.setCerts(pems);

        // Ask if the user accepts to proceed this process of the bad cert
        context->onCertificateVerificationRequest(error);

        isProceed = error.confirmCert();
    }

    if (isProceed)
        sslCertErrorHandler->proceedProcess();
    else
        sslCertErrorHandler->cancelProcess();

    return false;
}

bool URLRequestHttpListener::didReceiveTimingInformationMessage(NTF::URLResponseHttpTimingInformationMessage* message)
{
    TRACE("[IN] %s(%d)\n", __FUNCTION__, __LINE__);

    ResourceHandle* resourceHandle = getResourceHandle();
    ASSERT(resourceHandle);
    ResourceHandleInternal* d = resourceHandle->getInternal();
    ASSERT(d);

    if (d->m_cancelled)
        return false;

#if ENABLE(WEB_TIMING)
    // Web timing
    if (message->eventTime() == -1)
        return false;

    int eventTime = message->eventTime();

    switch (message->eventType()) {
    case NTF::URLResponseHttpTimingInformationMessage::DomainLookUpStart :
        d->m_response.resourceLoadTiming().domainLookupStart = eventTime;
        break;

    case NTF::URLResponseHttpTimingInformationMessage::DomainLookUpEnd :
        d->m_response.resourceLoadTiming().domainLookupEnd = eventTime;
        break;

    case NTF::URLResponseHttpTimingInformationMessage::ConnectStart :
        d->m_response.resourceLoadTiming().connectStart = eventTime;
        break;

    case NTF::URLResponseHttpTimingInformationMessage::ConnectEnd :
        d->m_response.resourceLoadTiming().connectEnd = eventTime;
        break;

    case NTF::URLResponseHttpTimingInformationMessage::SecureConnectionStart :
        d->m_response.resourceLoadTiming().secureConnectionStart = eventTime;
        break;

    case NTF::URLResponseHttpTimingInformationMessage::RequestStart :
        d->m_response.resourceLoadTiming().requestStart = eventTime;
        break;

    case NTF::URLResponseHttpTimingInformationMessage::ResponseStart :
        d->m_response.resourceLoadTiming().responseStart = eventTime;
        break;

    case NTF::URLResponseHttpTimingInformationMessage::Redirect :
        {
            // Reset a ResourceLoadTiming
            ResourceLoadTiming defaultTiming;
            d->m_response.resourceLoadTiming() = defaultTiming;
        }
        break;

    default :
        ASSERT(false);
        break;
    }
#endif

    return false;
}

} // namespace WebCore
