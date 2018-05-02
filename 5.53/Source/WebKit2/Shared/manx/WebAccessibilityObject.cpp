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

#include "config.h"
#include "WebAccessibilityObject.h"

#include "ArgumentDecoder.h"
#include "ArgumentEncoder.h"
#include "WebFrame.h"
#include "WebFrameProxy.h"
#include "WebPageProxy.h"
#include "WebProcessProxy.h"

#include <WebCore/Document.h>
#include <WebCore/DocumentLoader.h>
#include <WebCore/Frame.h>

using namespace WebCore;

namespace WebKit {

// Data
WebAccessibilityObject::Data::Data(AccessibilityObject* axObject)
{
    set(axObject);
}

WebAccessibilityObject::Data::~Data()
{
}

void WebAccessibilityObject::Data::encode(IPC::ArgumentEncoder& encoder) const
{
    encoder << static_cast<uint64_t>(m_id);
    encoder << m_webFrameID;
    encoder << static_cast<uint32_t>(m_role);
    encoder << m_title;
    encoder << m_description;
    encoder << m_helpText;
    encoder << m_url;
    encoder << static_cast<uint32_t>(m_buttonState);
}

bool WebAccessibilityObject::Data::decode(IPC::ArgumentDecoder& decoder, WebAccessibilityObject::Data& data)
{
    uint64_t id;
    if (!decoder.decode(id))
        return false;
    data.m_id = static_cast<AXID>(id);
    if (!decoder.decode(data.m_webFrameID))
        return false;
    uint32_t role;
    if (!decoder.decode(role))
        return false;
    data.m_role = static_cast<AccessibilityRole>(role);

    if (!decoder.decode(data.m_title))
        return false;
    if (!decoder.decode(data.m_description))
        return false;
    if (!decoder.decode(data.m_helpText))
        return false;
    if (!decoder.decode(data.m_url))
        return false;
    uint32_t buttonState;
    if (!decoder.decode(buttonState))
        return false;
    data.m_buttonState = static_cast<AccessibilityButtonState>(buttonState);
    return true;
}

String WebAccessibilityObject::Data::getAccessibilityTitle(AccessibilityObject* axObject, Vector<AccessibilityText>& textOrder)
{
    unsigned length = textOrder.size();
    for (unsigned k = 0; k < length; k++) {
        const AccessibilityText& text = textOrder[k];

        if (text.textSource == AlternativeText)
            break;

        if (text.textSource == VisibleText || text.textSource == ChildrenText)
            return text.text;

        // If there's an element that labels this object and it's not exposed, then we should use
        // that text as our title.
        if (text.textSource == LabelByElementText && !axObject->exposesTitleUIElement())
            return text.text;

        // FIXME: The title tag is used in certain cases for the title. This usage should
        // probably be in the description field since it's not "visible".
        if (text.textSource == TitleTagText)
            return text.text;
    }

    return String();
}

String WebAccessibilityObject::Data::getAccessibilityDescription(AccessibilityObject* axObject, Vector<AccessibilityText>& textOrder)
{
    if (axObject->roleValue() == StaticTextRole && !axObject->isMediaControlLabel())
        return axObject->textUnderElement();

    unsigned length = textOrder.size();
    for (unsigned k = 0; k < length; k++) {
        const AccessibilityText& text = textOrder[k];

        if (text.textSource == AlternativeText)
            return text.text;

        if (text.textSource == TitleTagText)
            return text.text;
    }

    return String();
}

String WebAccessibilityObject::Data::getAccessibilityHelpText(AccessibilityObject* axObject, Vector<AccessibilityText>& textOrder)
{
    unsigned length = textOrder.size();
    bool descriptiveTextAvailable = false;
    for (unsigned k = 0; k < length; k++) {
        const AccessibilityText& text = textOrder[k];

        if (text.textSource == HelpText || text.textSource == SummaryText)
            return text.text;

        switch (text.textSource) {
        case AlternativeText:
        case VisibleText:
        case ChildrenText:
        case LabelByElementText:
            descriptiveTextAvailable = true;
        default:
            break;
        }

        if (text.textSource == TitleTagText && descriptiveTextAvailable)
            return text.text;
    }

    return String();
}

void WebAccessibilityObject::Data::set(AccessibilityObject* axObject)
{
    m_id = axObject->axObjectID();
    Frame* frame = axObject->document()->frame();
    WebFrameLoaderClient* webFrameLoaderClient = frame ? toWebFrameLoaderClient(frame->loader().client()) : 0;
    WebFrame* webFrame = webFrameLoaderClient ? webFrameLoaderClient->webFrame() : 0;
    m_webFrameID = webFrame->frameID();

    if (axObject->isStaticText()) {
        AccessibilityObject* obj;
        Node* node = axObject->node();
        if (node) {
            obj = AccessibilityObject::anchorElementForNode(axObject->node());
            if (obj)
                axObject = obj;
        }
    }

    m_role = axObject->roleValue();

    Vector<AccessibilityText> textOrder;
    axObject->accessibilityText(textOrder);
    m_title = getAccessibilityTitle(axObject, textOrder);
    m_description = getAccessibilityDescription(axObject, textOrder);
    m_helpText = getAccessibilityHelpText(axObject, textOrder);

    // link or frame url
    m_url = axObject->url().string();

    // checkbox or radio button state
    m_buttonState = axObject->checkboxOrRadioValue();
}

// Object
PassRefPtr<WebAccessibilityObject> WebAccessibilityObject::create(const Data& data, WebFrameProxy* frame)
{
    return adoptRef(new WebAccessibilityObject(data, frame));
}

WebAccessibilityObject::WebAccessibilityObject(const Data& data, WebFrameProxy* frame)
    : m_frame(frame)
{
    m_data = new WebAccessibilityObject::Data(data);
}

WebAccessibilityObject::~WebAccessibilityObject()
{
    if (m_data)
        delete m_data;
}

AccessibilityRole WebAccessibilityObject::role()
{
    if (!m_data)
        return AccessibilityRole::UnknownRole;

    return m_data->role();
}

String WebAccessibilityObject::title()
{
    if (!m_data)
        return String();

    return m_data->title();
}

String WebAccessibilityObject::description()
{
    if (!m_data)
        return String();

    return m_data->description();
}

String WebAccessibilityObject::helpText()
{
    if (!m_data)
        return String();

    return m_data->helpText();
}

String WebAccessibilityObject::url()
{
    if (!m_data)
        return String();

    return m_data->url();
}

AccessibilityButtonState WebAccessibilityObject::buttonState()
{
    if (!m_data)
        return AccessibilityButtonState::ButtonStateOff;

    return m_data->buttonState();
}

WebPageProxy* WebAccessibilityObject::page()
{
    return frame()->page();
}

} // namespace WebKit
