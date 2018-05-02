/*
 * (C) 2016 Sony Interactive Entertainment Inc.
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

#ifndef WebViewAccessibilityClient_h
#define WebViewAccessibilityClient_h

#if HAVE(ACCESSIBILITY)

#include "APIClient.h"
#include "WebCore/AXObjectCache.h"
#include <WKView.h>
#include <wtf/Forward.h>

namespace API {
template<> struct ClientTraits<WKViewAccessibilityClientBase> {
    typedef std::tuple<WKViewAccessibilityClientV0> Versions;
};
}

namespace WebKit {

class WebView;
class WebAccessibilityObject;

class WebViewAccessibilityClient : public API::Client<WKViewAccessibilityClientBase> {
public:
    void accessibilityNotification(WebView*, WebAccessibilityObject*, WebCore::AXObjectCache::AXNotification);
    void accessibilityTextChanged(WebView*, WebAccessibilityObject*, WebCore::AXTextChange, uint32_t, const String&);
    void accessibilityLoadingEvent(WebView*, WebAccessibilityObject*, WebCore::AXObjectCache::AXLoadingEvent);
    void handleAccessibilityRootObject(WebView*, WebAccessibilityObject*);
    void handleAccessibilityFocusedObject(WebView*, WebAccessibilityObject*);
    void handleAccessibilityHitTest(WebView*, WebAccessibilityObject*);
};

} // namespace WebKit

#endif // HAVE(ACCESSIBILITY)
#endif // WebViewAccessibilityClient_h
