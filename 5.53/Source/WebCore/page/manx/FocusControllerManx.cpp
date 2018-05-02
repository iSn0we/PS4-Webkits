/*
 * Copyright (C) 2014 Sony Computer Entertainment Inc.
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
#include "FocusController.h"

#include "HTMLInputElement.h"
#include "HTMLNames.h"

namespace WebCore {

static bool isEditableElement(Element* e)
{
    if (!e->isHTMLElement())
        return false;

    if (e->hasTagName(HTMLNames::inputTag)) {
        HTMLInputElement* input = static_cast<HTMLInputElement*>(e);
        return (input->isTextField() && !input->isReadOnly());
    }

    if (e->hasTagName(HTMLNames::textareaTag))
        return !static_cast<HTMLElement*>(e)->isReadOnlyNode();

    return static_cast<HTMLElement*>(e)->isContentEditable();
}

Element* FocusController::nextEditableElement(Element* element)
{
    Element* lastFocusableElement = previousFocusableElement(FocusNavigationScope::focusNavigationScopeOf(element), 0, 0);
    Element* startingElement = 0;

    if (element->isSameNode(lastFocusableElement))
        startingElement = nextFocusableElement(FocusNavigationScope::focusNavigationScopeOf(element), 0, 0);
    else
        startingElement = nextFocusableElement(FocusNavigationScope::focusNavigationScopeOf(element), element, 0);

    if (!startingElement)
        return element;

    Element* e = startingElement;
    do {
        if (e->isHTMLElement() && !e->isFrameOwnerElement() && isEditableElement(e))
            return e;

        if (e->isSameNode(lastFocusableElement))
            e = nextFocusableElement(FocusNavigationScope::focusNavigationScopeOf(e), 0, 0);
        else
            e = nextFocusableElement(FocusNavigationScope::focusNavigationScopeOf(e), e, 0);
    } while (e && !e->isSameNode(startingElement));

    return element;
}

Element* FocusController::previousEditableElement(Element* element)
{
    Element* firstFocusableElement = nextFocusableElement(FocusNavigationScope::focusNavigationScopeOf(element), 0, 0);
    Element* startingElement = 0;

    if (element->isSameNode(firstFocusableElement))
        startingElement = previousFocusableElement(FocusNavigationScope::focusNavigationScopeOf(element), 0, 0);
    else
        startingElement = previousFocusableElement(FocusNavigationScope::focusNavigationScopeOf(element), element, 0);

    if (!startingElement)
        return element;

    Element* e = startingElement;
    do {
        if (e->isHTMLElement() && !e->isFrameOwnerElement() && isEditableElement(e))
            return e;

        if (e->isSameNode(firstFocusableElement))
            e = previousFocusableElement(FocusNavigationScope::focusNavigationScopeOf(e), 0, 0);
        else
            e = previousFocusableElement(FocusNavigationScope::focusNavigationScopeOf(e), e, 0);
    } while (e && !e->isSameNode(startingElement));

    return element;
}

bool FocusController::hasNextEditableElement(Element* element)
{
    Element* e = nextEditableElement(element);
    return (e && !e->isSameNode(element));
}

bool FocusController::hasPreviousEditableElement(Element* element)
{
    Element* e = previousEditableElement(element);
    return (e && !e->isSameNode(element));
}

}
