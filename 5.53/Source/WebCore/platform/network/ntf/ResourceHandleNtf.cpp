/*
 * Copyright (C) 2004, 2006 Apple Computer, Inc.  All rights reserved.
 * Copyright (C) 2005, 2006 Michael Emmel mike.emmel@gmail.com
 * All rights reserved.
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
#include "ResourceHandle.h"

#include "NotImplemented.h"
#include "ResourceHandleClient.h"
#include "ResourceHandleInternal.h"
#include "SynchronousLoaderClient.h"
#include "URLRequestJobFactory.h"

namespace WebCore {

ResourceHandleInternal::~ResourceHandleInternal()
{
    if (m_urlRequestJob) {
        NTF::URLRequestJob::destruct(m_urlRequestJob);
        m_urlRequestJob = 0;
    }

    if (m_urlRequestListener) {
        delete m_urlRequestListener;
        m_urlRequestListener = 0;
    }
}

ResourceHandle::~ResourceHandle()
{
    cancel(); // FIXME: Why is cancel() invoked here? Is it necessary?
}

bool ResourceHandle::start()
{
    // The frame could be null if the ResourceHandle is not associated to any
    // Frame, e.g. if we are downloading a file.
    // If the frame is not null but the page is null this must be an attempted
    // load from an unload handler, so let's just block it.
    // If both the frame and the page are not null the context is valid.
    if (d->m_context && !d->m_context->isValid())
        return false;

    NTF::URLRequestJob* job = 0;
    NTF::URLRequestListener* listener = 0;

    bool result = URLRequestJobFactory::create(this, &job, &listener);
    if (!result)
        return false;

    d->m_urlRequestJob = job;
    d->m_urlRequestListener = listener;
    d->m_urlRequestJob->start(false);

    return true;
}

void ResourceHandle::cancel()
{
    d->m_cancelled = true;

    if (d->m_urlRequestJob)
        d->m_urlRequestJob->cancel();

    setClient(0);
}

void ResourceHandle::platformSetDefersLoading(bool defers)
{
    notImplemented();
}

void ResourceHandle::platformLoadResourceSynchronously(NetworkingContext* context, const ResourceRequest& request, StoredCredentials storedCredentials, ResourceError& error, ResourceResponse& response, Vector<char>& data)
{
    SynchronousLoaderClient client;
    RefPtr<ResourceHandle> handle = adoptRef(new ResourceHandle(context, request, &client, false, false));
    if (!handle)
        return;

    ResourceHandleInternal* d = handle->getInternal();
    ASSERT(d);

    if (d->m_scheduledFailureType != NoFailure)
        return;

    NTF::URLRequestJob* job = 0;
    NTF::URLRequestListener* listener = 0;

    bool result = URLRequestJobFactory::create(handle.get(), &job, &listener);
    if (!result)
        return;

    d->m_urlRequestJob = job;
    d->m_urlRequestListener = listener;
    d->m_urlRequestJob->start(true);

    error = client.error();

    if (error.isNull())
        response = client.response();

    data.swap(client.mutableData());
}

// stubs needed for windows version
void ResourceHandle::didReceiveAuthenticationChallenge(const AuthenticationChallenge& challenge) 
{
    ASSERT(client());

    d->m_currentWebChallenge = challenge;
    d->m_currentWebChallenge.setAuthenticationClient(static_cast<AuthenticationClient*>(this));
    client()->didReceiveAuthenticationChallenge(this, d->m_currentWebChallenge);
}

void ResourceHandle::receivedCredential(const AuthenticationChallenge& challenge, const Credential& credential) 
{
    ASSERT(challenge.m_authenticationChallengeHandler);
    challenge.m_authenticationChallengeHandler->didReceiveAuthenticationCredential(challenge.m_authTarget, challenge.m_authScheme, credential.user().utf8().data(), credential.password().utf8().data());
    d->m_currentWebChallenge.setAuthenticationClient(0);
    clearAuthentication();
}

void ResourceHandle::receivedRequestToContinueWithoutCredential(const AuthenticationChallenge& challenge) 
{
    ASSERT(challenge.m_authenticationChallengeHandler);
    challenge.m_authenticationChallengeHandler->didReceiveAuthenticationCredential(challenge.m_authTarget, challenge.m_authScheme, "", "");
    d->m_currentWebChallenge.setAuthenticationClient(0);
    clearAuthentication();
}

void ResourceHandle::receivedCancellation(const AuthenticationChallenge& challenge)
{
    ASSERT(challenge.m_authenticationChallengeHandler);
    challenge.m_authenticationChallengeHandler->didReceiveAuthenticationCancellation();
    d->m_currentWebChallenge.setAuthenticationClient(0);
    clearAuthentication();
}

void ResourceHandle::receivedRequestToPerformDefaultHandling(const AuthenticationChallenge&)
{
    ASSERT_NOT_REACHED();
}

void ResourceHandle::receivedChallengeRejection(const AuthenticationChallenge&)
{
    ASSERT_NOT_REACHED();
}

} // namespace WebCore
