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

#include "config.h"
#include "AsyncImageDecoder.h"

#if ENABLE(MANX_ASYNC_IMAGE_DECODER)

#include "BitmapImage.h"
#include "CachedImage.h"
#include "Frame.h"
#include "JSAsyncImageDecoder.h"
#include "JSDOMWindow.h"
#include "ScriptState.h"
#include "Supplementable.h"

#include <wtf/Functional.h>
#include <wtf/MainThread.h>
#include <wtf/WorkQueue.h>

using namespace WTF;
using namespace WebCore;

class AsyncImageDecoder::DOMWindowSupplement: public Supplement<DOMWindow> {
public:
    explicit DOMWindowSupplement(DOMWindow* window)
        : m_decoder(adoptRef(new AsyncImageDecoder(window))) { }

    static const char* supplementName() { return "AsyncImageDecoder"; }
    static AsyncImageDecoder* from(DOMWindow* window)
    {
        DOMWindowSupplement* supplement = static_cast<DOMWindowSupplement*>(Supplement<DOMWindow>::from(window, supplementName()));
        if (!supplement) {
            supplement = new DOMWindowSupplement(window);
            provideTo(window, supplementName(), std::unique_ptr<Supplement<DOMWindow>>(supplement));
        }
        return supplement->decoder();
    }

    AsyncImageDecoder* decoder() const { return m_decoder.get(); }
private:
    RefPtr<AsyncImageDecoder> m_decoder;
};

class AsyncImageDecoder::ImageDecodeJobData : public ThreadSafeRefCounted<ImageDecodeJobData> {
public:
    CachedResourceHandle<CachedImage> m_cachedImage;
    RefPtr<VoidCallback> m_callback;
    RefPtr<BitmapImage> m_bitmap;
};

AsyncImageDecoder::AsyncImageDecoder(DOMWindow* window)
    : DOMWindowProperty(window->frame())
    , m_workQueue(nullptr)
{
}

AsyncImageDecoder::~AsyncImageDecoder()
{
}

bool AsyncImageDecoder::registerOnFrame(const String& propertyName, Frame* frame, DOMWrapperWorld& world)
{
    ASSERT(isMainThread());
    JSDOMWindow* window = toJSDOMWindow(frame, world);
    ScriptState* exec = window->globalExec();
    JSC::Identifier identifier = JSC::Identifier::fromString(exec, propertyName);
    JSC::JSValue value = toJS(exec, window->globalObject(), DOMWindowSupplement::from(JSDOMWindow::toWrapped(window)));

    window->putDirect(exec->vm(), identifier, value);

    return true;
}

void AsyncImageDecoder::decode(HTMLImageElement* imageElem, PassRefPtr<VoidCallback> callback)
{
    ASSERT(isMainThread());
    if (!imageElem || !imageElem->cachedImage()->hasImage() || !imageElem->cachedImage()->image()->isBitmapImage())
        return;

    // If the image is already decoded, do nothing (just call the callback right away if one is set)
    if (imageElem->cachedImage()->decodedSize()) {
        if (callback)
            callback->handleEvent();
        return;
    }

    RefPtr<ImageDecodeJobData> jobData = adoptRef(new ImageDecodeJobData());
    jobData->m_cachedImage = imageElem->cachedImage();
    jobData->m_bitmap = BitmapImage::create();
    jobData->m_bitmap->setData(imageElem->cachedImage()->resourceBuffer(), true);
    jobData->m_callback = callback;

    workQueue()->dispatch([jobData] {
        ASSERT(!isMainThread());

        // BitmapImage::size() needs to be called once to avoid
        // inconsistent state of BitmapImage::isSizeAvailable() and
        // ImageSource::isSizeAvailable() ater
        // BitmapImage::destroyDecodedData is called
        jobData->m_bitmap->size();
        // Triggers full decoding
        jobData->m_bitmap->nativeImageForCurrentFrame();

        // MemoryCache can only be accessed from main thread, so we have to
        // finish the job by caching the image from the main thread.
        callOnMainThread([jobData] {
            ASSERT(isMainThread());
            jobData->m_cachedImage->setDecodedImage(jobData->m_bitmap);
            if (jobData->m_callback)
                jobData->m_callback->handleEvent();
        });
    });
}

RefPtr<WTF::WorkQueue> AsyncImageDecoder::workQueue()
{
    if (!m_workQueue)
        m_workQueue = WTF::WorkQueue::create("AsyncImageDecoder");

    return m_workQueue;
}

#endif // ENABLE(MANX_ASYNC_IMAGE_DECODER)
