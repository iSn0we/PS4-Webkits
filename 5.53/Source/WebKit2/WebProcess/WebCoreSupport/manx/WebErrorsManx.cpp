/*
 * Copyright (C) 2010 Apple Inc. All rights reserved.
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

#include "config.h"
#include "WebErrors.h"

#include "APIError.h"
#include <WebCore/ResourceRequest.h>
#include <WebCore/ResourceResponse.h>
#include <manx/LoadTypes.h>

using namespace WebCore;

namespace WebKit {

ResourceError cancelledError(const ResourceRequest& request)
{
    return ResourceError(API::Error::webKitErrorDomain(),
        LOAD_ERROR_CANCELLED,
        request.url().string(),
        String("Load cancelled"));
}

ResourceError blockedError(const ResourceRequest& request)
{
    return ResourceError(API::Error::webKitErrorDomain(),
        LOAD_ERROR_BLOCKED,
        request.url().string(),
        String("Load blocked"));
}

ResourceError cannotShowURLError(const ResourceRequest& request)
{
    return ResourceError(API::Error::webKitErrorDomain(),
        LOAD_ERROR_CANNOT_SHOW_URL,
        request.url().string(),
        String("Cannot show url"));
}

ResourceError interruptedForPolicyChangeError(const ResourceRequest& request)
{
    return ResourceError(API::Error::webKitErrorDomain(),
        LOAD_ERROR_INTERRUPTED_FOR_POLICY_CHANGE,
        request.url().string(),
        String("Interrupted for policy change"));
}

ResourceError cannotShowMIMETypeError(const ResourceResponse& response)
{
    return ResourceError(API::Error::webKitErrorDomain(),
        LOAD_ERROR_CANNOT_SHOW_MIME_TYPE,
        response.url().string(),
        String("Cannot show mime type"));
}

ResourceError fileDoesNotExistError(const ResourceResponse& response)
{
    return ResourceError(API::Error::webKitErrorDomain(),
        LOAD_ERROR_FILE_DOES_NOT_EXIST,
        response.url().string(),
        String("File does not exist"));
}

ResourceError pluginWillHandleLoadError(const ResourceResponse& response)
{
    return ResourceError(API::Error::webKitErrorDomain(),
        LOAD_ERROR_PLUGIN_WILL_HANDLE_LOAD,
        response.url().string(),
        String("Plugin will handle load"));
}

} // namespace WebKit
