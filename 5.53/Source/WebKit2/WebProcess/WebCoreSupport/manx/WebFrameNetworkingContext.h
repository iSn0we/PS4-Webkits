/*
    Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef WebFrameNetworkingContext_h
#define WebFrameNetworkingContext_h

#include "WebFrame.h"

#include "WebPage.h"
#include "WebPageProxyMessages.h"
#include "WebProcess.h"
#include <WebCore/AuthenticationChallenge.h>
#include <WebCore/DocumentLoader.h>
#include <WebCore/FrameNetworkingContext.h>
#include <WebCore/ResourceError.h>
#include <WebCore/ResourceRequest.h>

class WebFrameNetworkingContext : public WebCore::FrameNetworkingContext {
public:
    static PassRefPtr<WebFrameNetworkingContext> create(WebKit::WebFrame* frame)
    {
        return adoptRef(new WebFrameNetworkingContext(frame));
    }

    virtual void onAuthenticationRequired(const WebCore::AuthenticationChallenge& challenge) const 
    {
#if USE(NTF)
        if (frame())
            frame()->loader().client().dispatchDidReceiveAuthenticationChallenge(0, 0, challenge);
#endif
    }

    virtual void onCertificateVerificationRequest(const WebCore::ResourceError& error) const 
    {
#if USE(NTF)
        WebKit::WebPage* webPage = WebKit::WebProcess::singleton().webPage(m_pageID);
        if (webPage) {
            bool ok = false;
            if (webPage->sendSync(Messages::WebPageProxy::CertificateVerificationRequest(m_frameID, error.sslVerificationResult(), error.failingURL(), error.certs()), Messages::WebPageProxy::CertificateVerificationRequest::Reply(ok)))
                error.setConfirmCert(ok);
        }
#endif
    }

    virtual bool isMainResource(const void* client) const
    {
        ASSERT(client);
        if (frame() && frame()->loader().activeDocumentLoader() && frame()->loader().activeDocumentLoader()->mainResourceLoader()) {
            WebCore::SubresourceLoader* mainloader = frame()->loader().activeDocumentLoader()->mainResourceLoader();
            if (mainloader == client)
                return true;
        }
        return false;
    }

    virtual void replaceUrlOfMainResource(const WebCore::URL& responseUrl) const
    {
        if (frame() && frame()->loader().activeDocumentLoader()) {
            if (frame()->loader().activeDocumentLoader()->isLoadingMainResource() && !responseUrl.isEmpty()) 
                frame()->loader().activeDocumentLoader()->replaceRequestURLForSameDocumentNavigation(responseUrl);
        }
    }

private:
    WebFrameNetworkingContext(WebKit::WebFrame* frame)
        : WebCore::FrameNetworkingContext(frame->coreFrame())
        , m_pageID(0), m_frameID(0)
    {
        if (frame && frame->page()) {
            m_pageID = frame->page()->pageID();
            m_frameID = frame->frameID();
        }
    }

    uint64_t m_pageID;
    uint64_t m_frameID;
};

#endif // WebFrameNetworkingContext_h
