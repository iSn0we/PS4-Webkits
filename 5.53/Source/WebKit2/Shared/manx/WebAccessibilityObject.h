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

#ifndef WebAccessibilityObject_h
#define WebAccessibilityObject_h

#if HAVE(ACCESSIBILITY)

#include "APIObject.h"

#include <WebCore/AccessibilityObject.h>

namespace IPC {
class ArgumentDecoder;
class ArgumentEncoder;
}

namespace WebKit {

class WebPageProxy;
class WebFrameProxy;

class WebAccessibilityObject : public API::ObjectImpl<API::Object::Type::AccessibilityObject> {
public:
    class Data {
    public:
        explicit Data() { };
        Data(WebCore::AccessibilityObject* axObject);
        ~Data();

        // IPC
        void encode(IPC::ArgumentEncoder&) const;
        static bool decode(IPC::ArgumentDecoder&, WebAccessibilityObject::Data&);

        void set(WebCore::AccessibilityObject* axObject);

        WebCore::AXID id() const { return m_id; };
        uint64_t webFrameID() const { return m_webFrameID; };
        WebCore::AccessibilityRole role() const { return m_role; };
        String title() const { return m_title; };
        String description() const { return m_description; };
        String helpText() const { return m_helpText; };
        String url() const { return m_url; };
        WebCore::AccessibilityButtonState buttonState() const { return m_buttonState; };

    private:
        String getAccessibilityTitle(WebCore::AccessibilityObject* axObject, Vector<WebCore::AccessibilityText>& textOrder);
        String getAccessibilityDescription(WebCore::AccessibilityObject* axObject, Vector<WebCore::AccessibilityText>& textOrder);
        String getAccessibilityHelpText(WebCore::AccessibilityObject* axObject, Vector<WebCore::AccessibilityText>& textOrder);

        // common
        WebCore::AXID m_id;
        uint64_t m_webFrameID;
        WebCore::AccessibilityRole m_role;
        String m_title;
        String m_description;
        String m_helpText;
        String m_url;

        // checkbox or radio button state
        WebCore::AccessibilityButtonState m_buttonState;
    };

    static PassRefPtr<WebAccessibilityObject> create(const Data&, WebFrameProxy*);
    virtual ~WebAccessibilityObject();

    WebCore::AccessibilityRole role();
    String title();
    String description();
    String helpText();
    String url();

    WebCore::AccessibilityButtonState buttonState();

    WebPageProxy* page();
    WebFrameProxy* frame() { return m_frame; };

private:
    WebAccessibilityObject(const Data&, WebFrameProxy*);

    WebFrameProxy* m_frame;
    Data* m_data;
};

} // namespace WebKit

#endif // HAVE(ACCESSIBILITY)
#endif // WebAccessibilityObject_h
