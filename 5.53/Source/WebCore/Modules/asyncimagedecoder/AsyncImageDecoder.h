/*
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

#ifndef AsyncImageDecoder_h
#define AsyncImageDecoder_h

#if ENABLE(MANX_ASYNC_IMAGE_DECODER)

#include "DOMWindowProperty.h"
#include "HTMLImageElement.h"
#include "VoidCallback.h"

#include <wtf/HashMap.h>
#include <wtf/PassRefPtr.h>
#include <wtf/ThreadSafeRefCounted.h>

namespace WTF {
class WorkQueue;
}

namespace WebCore {

class BitmapImage;
class DOMWindow;

class AsyncImageDecoder : public ThreadSafeRefCounted<AsyncImageDecoder>, public DOMWindowProperty {
public:
    virtual ~AsyncImageDecoder();

    WEBCORE_EXPORT static bool registerOnFrame(const String& propertyName, Frame*, DOMWrapperWorld&);

    void decode(HTMLImageElement*, PassRefPtr<VoidCallback>);

private:
    class DOMWindowSupplement;
    class ImageDecodeJobData;
    explicit AsyncImageDecoder(DOMWindow*);

    RefPtr<WTF::WorkQueue> workQueue();

    RefPtr<WTF::WorkQueue> m_workQueue;
};

} // namespace WebCore

#endif // ENABLE(MANX_ASYNC_IMAGE_DECODER)

#endif // AsyncImageDecoder_h
