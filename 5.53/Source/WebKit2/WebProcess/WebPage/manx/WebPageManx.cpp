/*
 * Copyright (C) 2010 Apple Inc. All rights reserved.
 * Portions Copyright (c) 2010 Motorola Mobility, Inc.  All rights reserved.
 * Copyright (C) 2011 Igalia S.L.
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
#include "WebPage.h"

#include "EditorState.h"
#include "NotImplemented.h"
#include "TextIterator.h"
#include "WebCoreArgumentCoders.h"
#include "WebEvent.h"
#include "WebEventConversion.h"
#include "WebPageProxyMessages.h"
#include "WebPreferencesKeys.h"
#include "WebPreferencesStore.h"
#include "htmlediting.h"
#include <WebCore/AXObjectCache.h>
#include <WebCore/ApplicationCacheStorage.h>
#include <WebCore/Document.h>
#include <WebCore/Element.h>
#include <WebCore/FocusController.h>
#include <WebCore/Frame.h>
#include <WebCore/FrameSelection.h>
#include <WebCore/FrameView.h>
#include <WebCore/HTMLFormElement.h>
#include <WebCore/HTMLInputElement.h>
#include <WebCore/HTMLTextAreaElement.h>
#include <WebCore/KeyboardEvent.h>
#include <WebCore/MainFrame.h>
#include <WebCore/Node.h>
#include <WebCore/Page.h>
#include <WebCore/PageGroup.h>
#include <WebCore/PlatformKeyboardEvent.h>
#include <WebCore/RenderElement.h>
#include <WebCore/RenderObject.h>
#include <WebCore/Settings.h>
#include <manx/InputFieldTypes.h>
#include <manx/InputLanguages.h>
#include <manx/KeyboardCodes.h>
#if USE(NTF)
#include <ntf/CookieJarDB.h>
#include <ntf/Privacy.h>
#endif

using namespace WebCore;

namespace WebKit {

void WebPage::platformInitialize()
{
    m_page->settings().setFixedBackgroundsPaintRelativeToDocument(true);
#if ENABLE(SMOOTH_SCROLLING)
    // ScrollableArea::scheduleAnimation is not implemented yet
    m_page->settings().setEnableScrollAnimator(false);
#endif
#if ENABLE(MANX_CURSOR_NAVIGATION)
    m_page->settings().setCursorNavigationEnabled(true);
#endif
    const char* secureWebProcess = getenv("MANX_SECURE_WEB_PROCESS");
    bool isSecureWebProcess = secureWebProcess && !strcmp(secureWebProcess, "1");
    const char* webProcessHTMLTile = getenv("MANX_WEB_PROCESS_HTML_TILE");
    bool isWebProcessHTMLTile = webProcessHTMLTile && !strcmp(webProcessHTMLTile, "1");

    if (isSecureWebProcess) {
        m_page->settings().setAllowDisplayOfInsecureContent(false);
        m_page->settings().setAllowRunningOfInsecureContent(false);
    }

    if (isWebProcessHTMLTile) {
        // Limited quotas for HTMLTile, cf. bug 42150
        m_page->settings().setSessionStorageQuota(1 * MB);
    } else if (isSecureWebProcess) {
        // For secure contents, bug 120906
        m_page->settings().setSessionStorageQuota(100 * MB);
    } else {
        // Defaults :
        // * HTML5 ApplicationCache (per origin): unlimited
        // * HTML5 ApplicationCache (total): unlimited
        // * HTML5 SessionStorage (per origin): 5MB, cf. bug 6887
        m_page->settings().setSessionStorageQuota(5 * MB);
    }
}

void WebPage::platformDetach()
{
}

void WebPage::platformPreferencesDidChange(const WebPreferencesStore& store)
{
#if USE(NTF)
    NTF::Cookie::CookieJarDB::setEnable(store.getBoolValueForKey(WebPreferencesKey::cookieEnabledKey()));
#if ENABLE(DNT)
    NTF::Privacy::setDoNotTrack(store.getBoolValueForKey(WebPreferencesKey::doNotTrackKey()));
#endif
#endif
}

bool WebPage::performDefaultBehaviorForKeyEvent(const WebKeyboardEvent& keyboardEvent)
{
    bool consumed = false;
#if ENABLE(MANX_CURSOR_NAVIGATION)
    if (isCursorNavigationEnabled(mainFrame())) {
        IntPoint newCursorPosition;
        bool moved;
        consumed = m_page->cursorNavigation().handleKeyEvent(platform(keyboardEvent), moved, newCursorPosition);
        if (moved)
            send(Messages::WebPageProxy::SetCursorPosition(newCursorPosition));
    }
#endif
    return consumed;
}

bool WebPage::platformHasLocalDataForURL(const URL&)
{
    notImplemented();
    return false;
}

String WebPage::cachedResponseMIMETypeForURL(const URL&)
{
    notImplemented();
    return String();
}

bool WebPage::platformCanHandleRequest(const ResourceRequest&)
{
    notImplemented();
    return true;
}

String WebPage::cachedSuggestedFilenameForURL(const URL&)
{
    notImplemented();
    return String();
}

PassRefPtr<SharedBuffer> WebPage::cachedResponseDataForURL(const URL&)
{
    notImplemented();
    return 0;
}

void WebPage::keyEventManx(const WebKeyboardEvent& event, int caretOffset)
{
    bool handled = false;

    if (caretOffset == -1)
        caretOffset = event.text().length();

    Frame& frame = m_page->focusController().focusedOrMainFrame();
    if (frame.editor().canEdit()) {

        Element* element = frame.document()->focusedElement();
        if (element) {
            if (event.keyIdentifier() == "TabJump") {
                if (frame.editor().hasComposition())
                    frame.editor().cancelComposition();
                m_page->focusController().setMoveFocusByTabKey(true);
                moveFocusInTabOrder(!event.shiftKey());
                m_page->focusController().setMoveFocusByTabKey(false);
                handled = true;
            } else if (event.keyIdentifier() == "Accept") {
                frame.editor().confirmComposition(event.text(), caretOffset);
                handled = true;
            } else if (event.keyIdentifier() == "Convert") {
                if (element->renderer()) {
                    WebCore::Color underlineColor = static_cast<Node*>(element)->renderer()->style().visitedDependentColor(CSSPropertyColor);
                    Vector<CompositionUnderline> underlines;
                    underlines.append(CompositionUnderline(0, caretOffset, underlineColor, true));
                    if (event.text().length() > caretOffset)
                        underlines.append(CompositionUnderline(caretOffset, event.text().length(), underlineColor, false));
                    frame.editor().setComposition(event.text(), underlines, caretOffset, caretOffset);
                    handled = true;
                }
            } else if (event.keyIdentifier() == "Cancel") {
                frame.editor().cancelComposition();
                handled = true;
            }
        }
    }

    if (handled)
        send(Messages::WebPageProxy::DidReceiveEvent(static_cast<uint32_t>(event.type()), true));
    else
        keyEvent(event);
}

void WebPage::setComposition(const String& text, Vector<WebCore::CompositionUnderline> underlines, uint64_t selectionStart, uint64_t selectionEnd, uint64_t replacementRangeStart, uint64_t replacementRangeEnd)
{
    Frame& frame = m_page->focusController().focusedOrMainFrame();
    if (!frame.editor().canEdit())
        return;

    frame.editor().setComposition(text, underlines, selectionStart, selectionEnd);
}

void WebPage::confirmComposition(const String& text, int64_t selectionStart, int64_t selectionLength)
{
    Frame& frame = m_page->focusController().focusedOrMainFrame();
    if (!frame.editor().canEdit())
        return;

    frame.editor().confirmComposition(text, selectionStart);
}

void WebPage::cancelComposition()
{
    Frame& frame = m_page->focusController().focusedOrMainFrame();
    if (!frame.editor().canEdit())
        return;

    frame.editor().cancelComposition();
}

void WebPage::exitComposition()
{
    Frame& frame = m_page->focusController().focusedOrMainFrame();

    if (frame.editor().hasComposition())
        frame.editor().cancelComposition();

    Node* node = frame.document()->focusedElement();
    if (!node || !node->isElementNode())
        return;

    static_cast<Element*>(node)->blur();
}

void WebPage::setValueToFocusedNode(const String& value)
{
    Frame& frame = m_page->focusController().focusedOrMainFrame();
    if (!frame.editor().canEdit())
        return;

    if (frame.editor().hasComposition())
        frame.editor().cancelComposition();

    Element* element = frame.document()->focusedElement();
    if (!element)
        return;
    if (is<HTMLInputElement>(element)) {
        downcast<HTMLInputElement>(element)->setValue(value, WebCore::DispatchInputAndChangeEvent);
    } else if (is<HTMLTextAreaElement>(element)) {
        downcast<HTMLTextAreaElement>(element)->setValue(value);
        element->dispatchInputEvent();
    } else {
        ExceptionCode ec = 0;
        element->setTextContent(value, ec);
        element->dispatchInputEvent();
    }
    send(Messages::WebPageProxy::EditorStateChanged(editorState()));
}

void WebPage::clearSelectionWithoutBlur()
{
    Frame& frame = m_page->focusController().focusedOrMainFrame();
    frame.selection().setBase(frame.selection().selection().extent(), frame.selection().selection().affinity());
}

void WebPage::setCaretVisible(bool visible)
{
    Frame& frame = m_page->focusController().focusedOrMainFrame();
    frame.selection().setCaretVisible(visible);
}

void WebPage::moveFocusInTabOrder(bool forward)
{
    Frame& frame = m_page->focusController().focusedOrMainFrame();

    Element* node = frame.document()->focusedElement();
    if (!node)
        return;

    Element* nodeNext;
    if (forward)
        nodeNext = m_page->focusController().nextEditableElement(node);
    else
        nodeNext = m_page->focusController().previousEditableElement(node);

    if (nodeNext) {
        if (!nodeNext->dispatchEvent(Event::create(eventNames().selectstartEvent, true, true)))
            return;
        FrameView* view = frame.document()->view();
        IntPoint oldPosition = view->scrollPosition();
        view->scrollElementToRect(nodeNext, bounds());
        IntPoint newPosition = view->scrollPosition();
        view->scrollPositionChangedViaPlatformWidget(oldPosition, newPosition);

        frame.document()->setFocusedElement(nodeNext);

        if (is<HTMLInputElement>(nodeNext)) {
            HTMLInputElement* element = downcast<HTMLInputElement>(nodeNext);
            element->setSelectionRange(0, element->value().length());
        } else if (is<HTMLTextAreaElement>(nodeNext)) {
            HTMLTextAreaElement* element = downcast<HTMLTextAreaElement>(nodeNext);
            element->setSelectionRange(0, 0);
        } else {
            VisibleSelection newSelection(firstPositionInNode(nodeNext), firstPositionInNode(nodeNext));
            if (frame.selection().shouldChangeSelection(newSelection))
                frame.selection().setSelection(newSelection);
        }
    }
}

static const unsigned CtrlKey = 1 << 0;
static const unsigned AltKey = 1 << 1;
static const unsigned ShiftKey = 1 << 2;

struct KeyDownEntry {
    unsigned virtualKey;
    unsigned modifiers;
    const char* name;
};

struct KeyPressEntry {
    unsigned charCode;
    unsigned modifiers;
    const char* name;
};

static const KeyDownEntry keyDownEntries[] = {
    { Manx::VK_LEFT,   0,                  "MoveLeft"                                    },
    { Manx::VK_LEFT,   ShiftKey,           "MoveLeftAndModifySelection"                  },
    { Manx::VK_RIGHT,  0,                  "MoveRight"                                   },
    { Manx::VK_RIGHT,  ShiftKey,           "MoveRightAndModifySelection"                 },
    { Manx::VK_UP,     0,                  "MoveUp"                                      },
    { Manx::VK_UP,     ShiftKey,           "MoveUpAndModifySelection"                    },
    { Manx::VK_DOWN,   0,                  "MoveDown"                                    },
    { Manx::VK_DOWN,   ShiftKey,           "MoveDownAndModifySelection"                  },
    { Manx::VK_PRIOR,  0,                  "MovePageUp"                                  },
    { Manx::VK_PRIOR,  ShiftKey,           "MovePageUpAndModifySelection"                },
    { Manx::VK_NEXT,   0,                  "MovePageDown"                                },
    { Manx::VK_NEXT,   ShiftKey,           "MovePageDownAndModifySelection"              },
    { Manx::VK_HOME,   0,                  "MoveToBeginningOfLine"                       },
    { Manx::VK_HOME,   ShiftKey,           "MoveToBeginningOfLineAndModifySelection"     },
    { Manx::VK_HOME,   CtrlKey,            "MoveToBeginningOfDocument"                   },
    { Manx::VK_HOME,   CtrlKey | ShiftKey, "MoveToBeginningOfDocumentAndModifySelection" },
    { Manx::VK_END,    0,                  "MoveToEndOfLine"                             },
    { Manx::VK_END,    ShiftKey,           "MoveToEndOfLineAndModifySelection"           },
    { Manx::VK_END,    CtrlKey,            "MoveToEndOfDocument"                         },
    { Manx::VK_END,    CtrlKey | ShiftKey, "MoveToEndOfDocumentAndModifySelection"       },
    { Manx::VK_BACK,   0,                  "DeleteBackward"                              },
    { Manx::VK_DELETE, 0,                  "DeleteForward"                               },
    { Manx::VK_ESCAPE, 0,                  "Cancel"                                      },
    { Manx::VK_TAB,    0,                  "InsertTab"                                   },
    { Manx::VK_RETURN, 0,                  "InsertNewline"                               },
    { 'A',             CtrlKey,            "SelectAll"                                   },

    { Manx::VKX_PS_CARET_LEFT,  0,        "MoveLeft"                                     },
    { Manx::VKX_PS_CARET_LEFT,  ShiftKey, "MoveLeftAndModifySelection"                   },
    { Manx::VKX_PS_CARET_RIGHT, 0,        "MoveRight"                                    },
    { Manx::VKX_PS_CARET_RIGHT, ShiftKey, "MoveRightAndModifySelection"                  },
    { Manx::VKX_PS_CARET_UP,    0,        "MoveUp"                                       },
    { Manx::VKX_PS_CARET_UP,    ShiftKey, "MoveUpAndModifySelection"                     },
    { Manx::VKX_PS_CARET_DOWN,  0,        "MoveDown"                                     },
    { Manx::VKX_PS_CARET_DOWN,  ShiftKey, "MoveDownAndModifySelection"                   },

    { Manx::VKX_PS_TAB_JUMP,    0,        "TabJumpForward"                               },
    { Manx::VKX_PS_TAB_JUMP,    ShiftKey, "TabJumpBackword"                              },
};

static const KeyPressEntry keyPressEntries[] = {
    { '\t',            0,                  "InsertTab"                                   },
    { '\r',            0,                  "InsertNewline"                               },
};

const char* WebPage::interpretKeyEvent(const KeyboardEvent* evt)
{
    ASSERT(evt->type() == eventNames().keydownEvent || evt->type() == eventNames().keypressEvent);

    static HashMap<int, const char*>* keyDownCommandsMap = 0;
    static HashMap<int, const char*>* keyPressCommandsMap = 0;

    if (!keyDownCommandsMap) {
        keyDownCommandsMap = new HashMap<int, const char*>;
        keyPressCommandsMap = new HashMap<int, const char*>;

        for (unsigned i = 0; i < (sizeof(keyDownEntries) / sizeof(keyDownEntries[0])); i++)
            keyDownCommandsMap->set(keyDownEntries[i].modifiers << 16 | keyDownEntries[i].virtualKey, keyDownEntries[i].name);

        for (unsigned i = 0; i < (sizeof(keyPressEntries) / sizeof(keyPressEntries[0])); i++)
            keyPressCommandsMap->set(keyPressEntries[i].modifiers << 16 | keyPressEntries[i].charCode, keyPressEntries[i].name);
    }

    unsigned modifiers = 0;
    if (evt->shiftKey())
        modifiers |= ShiftKey;
    if (evt->altKey())
        modifiers |= AltKey;
    if (evt->ctrlKey())
        modifiers |= CtrlKey;

    if (evt->type() == eventNames().keydownEvent) {
        int mapKey = modifiers << 16 | evt->keyEvent()->windowsVirtualKeyCode();
        return mapKey ? keyDownCommandsMap->get(mapKey) : 0;
    }

    int mapKey = modifiers << 16 | evt->charCode();
    return mapKey ? keyPressCommandsMap->get(mapKey) : 0;
}

String WebPage::platformUserAgent(const URL&) const
{
    return String();
}

void WebPage::platformEditorState(Frame& frame, EditorState& state, IncludePostLayoutDataHint hint) const
{
    state.fieldType = Manx::InputFieldTypeUndefined;
    state.fieldLang = Manx::InputLanguageUndefined;

    Element* element = frame.document()->focusedElement();
    if (!element || !element->renderer())
        state.isContentEditable = false;

    if (!state.isContentEditable)
        return;

    state.fieldRect = frame.view()->contentsToWindow(static_cast<Node*>(element)->renderer()->absoluteBoundingBoxRect());

    WTF::AtomicString str = element->computeInheritedLanguage();
    if (!str.isEmpty())
        state.fieldLang = Manx::getInputLanguage(str.string().utf8().data());
    if (element->hasAttribute("x-ps-osk"))
        state.psOSKAttr = WTF::String(element->getAttribute("x-ps-osk").string());

    state.hasPreviousNode = m_page->focusController().hasPreviousEditableElement(element);
    state.hasNextNode = m_page->focusController().hasNextEditableElement(element);
    state.isFocusedByTabKey = m_page->focusController().isFocusedByTabKey();
    Element* selectionRoot = frame.selection().selection().rootEditableElement();
    Element* scope = selectionRoot ? selectionRoot : frame.document()->documentElement();

    if (element->hasTagName(HTMLNames::inputTag)) {
        state.fieldText = scope->textContent();
        HTMLInputElement* input = static_cast<HTMLInputElement*>(element);
        if (input->isTextField()) {
            if (input->isPasswordField())
                state.fieldType = Manx::INPUT_FIELD_PASSWORD;
            else if (input->isEmailField())
                state.fieldType = Manx::INPUT_FIELD_EMAIL;
            else if (input->isURLField())
                state.fieldType = Manx::INPUT_FIELD_URL;
            else if (input->isSearchField())
                state.fieldType = Manx::INPUT_FIELD_SEARCH;
            else if (input->isNumberField())
                state.fieldType = Manx::INPUT_FIELD_NUMBER;
            else if (input->isTelephoneField())
                state.fieldType = Manx::INPUT_FIELD_TELEPHONE;
            else
                state.fieldType = Manx::INPUT_FIELD_TEXT;
            HTMLFormElement* form = input->form();
            if (form) {
                if (form->action().contains("login"))
                    state.isInLoginForm = true;
                // the following code should be moved more appropriate place.
                {
                    bool canSubmitImplicitly = false;
                    int submissionTriggerCount = 0;
                    const Vector<FormAssociatedElement*>& associatedElements = form->associatedElements();

                    for (unsigned i = 0; i < associatedElements.size(); ++i) {
                        FormAssociatedElement* formAssociatedElement = associatedElements[i];
                        if (!formAssociatedElement->isFormControlElement())
                            continue;
                        HTMLFormControlElement* formElement = static_cast<HTMLFormControlElement*>(formAssociatedElement);
                        if (formElement->isSuccessfulSubmitButton()) {
                            if (formElement->renderer()) {
                                canSubmitImplicitly = true;
                                break;
                            }
                        }
                        if (formElement->canTriggerImplicitSubmission())
                            ++submissionTriggerCount;
                    }
                    if (submissionTriggerCount == 1)
                        canSubmitImplicitly = true;

                    state.canSubmitImplicitly = canSubmitImplicitly;
                }
            }
        } else
            state.fieldType = Manx::InputFieldTypeUndefined;
    } else {
        state.fieldText = scope->innerText().replace((UChar)0xA0, (UChar)0x20);
        if (element->hasTagName(HTMLNames::textareaTag))
            state.fieldType = Manx::INPUT_FIELD_TEXTAREA;
        else
            state.fieldType = Manx::INPUT_FIELD_OTHER;
    }

    RefPtr<Range> range;
    if (state.selectionIsRange && !state.shouldIgnoreCompositionSelectionChange && (range = frame.selection().selection().toNormalizedRange())) {
        state.selectedRect = frame.view()->contentsToWindow(range->boundingBox());
        state.selectedText = range->text();
    }

    range = nullptr;
    if (!state.selectionIsRange && state.hasComposition && (range = frame.editor().compositionRange()))
        state.compositionRect = frame.view()->contentsToWindow(range->boundingBox());

    range = nullptr;
    if (!state.shouldIgnoreCompositionSelectionChange && !state.selectionIsNone && (range = frame.selection().selection().firstRange())) {
        size_t location = 0;
        size_t length = 0;
        TextIterator::getLocationAndLengthFromRange(scope, range.get(), location, length);
        bool baseIsFirst = frame.selection().selection().isBaseFirst();
        state.caretOffset = (baseIsFirst) ? location + length : location;
    } else
        state.caretOffset = -1;

}

#if HAVE(ACCESSIBILITY)
void WebPage::accessibilityRootObject()
{
    if (!WebCore::AXObjectCache::accessibilityEnabled())
        return;

    WebCore::Page* page = corePage();
    if (!page)
        return;

    WebCore::Frame& core = page->mainFrame();
    if (!core.document())
        return;

    WebCore::AccessibilityObject* root = core.document()->axObjectCache()->rootObject();
    if (!root || root->isDetached())
        return;

    send(Messages::WebPageProxy::DidReceiveAccessibilityRootObject(WebAccessibilityObject::Data(root)));
}

void WebPage::accessibilityFocusedObject()
{
    if (!WebCore::AXObjectCache::accessibilityEnabled())
        return;

    WebCore::Page* page = corePage();
    if (!page)
        return;

    WebCore::AccessibilityObject* focused = WebCore::AXObjectCache::focusedUIElementForPage(page);
    if (!focused || focused->isDetached())
        return;

    send(Messages::WebPageProxy::DidReceiveAccessibilityFocusedObject(WebAccessibilityObject::Data(focused)));
}

void WebPage::accessibilityHitTest(const IntPoint& point)
{
    if (!WebCore::AXObjectCache::accessibilityEnabled())
        return;

    WebCore::Page* page = corePage();
    if (!page)
        return;

    WebCore::Frame& core = page->mainFrame();
    if (!core.document())
        return;

    WebCore::AccessibilityObject* root = core.document()->axObjectCache()->rootObject();
    if (!root)
        return;
    IntPoint contentsPoint = root->documentFrameView()->windowToContents(point);
    AccessibilityObject* axObject = root->accessibilityHitTest(contentsPoint);
    if (!axObject || axObject->isDetached())
        return;

    send(Messages::WebPageProxy::DidReceiveAccessibilityHitTest(WebAccessibilityObject::Data(axObject)));
}
#endif

} // namespace WebKit
