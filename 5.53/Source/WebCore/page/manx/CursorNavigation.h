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

#ifndef CursorNavigation_h
#define CursorNavigation_h

#if ENABLE(MANX_CURSOR_NAVIGATION)

#include "IntPoint.h"
#include "PlatformKeyboardEvent.h"
#include "PlatformMouseEvent.h"

namespace WebCore {

class Frame;
class Page;

enum CursorNavDirection {
    CursorNavDirectionUp,
    CursorNavDirectionDown,
    CursorNavDirectionLeft,
    CursorNavDirectionRight,
};

class CursorNavigation {
    WTF_MAKE_FAST_ALLOCATED;

public:
    CursorNavigation(Page&);
    bool moveCursor(CursorNavDirection, bool& moved, IntPoint& newCursorPosition);
    WEBCORE_EXPORT bool handleKeyEvent(const PlatformKeyboardEvent&, bool& moved, IntPoint& newCursorPosition);
    WEBCORE_EXPORT void setCursorPosition(const PlatformMouseEvent&);

private:
    Page& m_page;
    IntPoint m_cursorPosition;
};

bool WEBCORE_EXPORT isCursorNavigationEnabled(const Frame*);

} // namespace WebCore

#endif
#endif // CursorNavigation_h
