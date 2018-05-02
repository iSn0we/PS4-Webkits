/*
 * Copyright (C) 2008 Apple Inc. All Rights Reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

#ifndef TextMetrics_h
#define TextMetrics_h

#include <wtf/Ref.h>
#include <wtf/RefCounted.h>

namespace WebCore {

class TextMetrics : public RefCounted<TextMetrics> {
public:
    static Ref<TextMetrics> create() { return adoptRef(*new TextMetrics); }

    float width() const { return m_width; }
    void setWidth(float w) { m_width = w; }

    // If the returned font width of the text overflows the advance length, for example
    // in the case of a slant font, then the sum of m_actualBoundingBoxLeft and m_actualBoundingBoxRight
    // should be used. Referring to http://www.whatwg.org in such cases, the following
    // metrics should be used to account for the overflow. The addition of actualBoundingBoxLeft
    // and actualBoundingBoxRight is that actual width of the inline box.
    float actualBoundingBoxLeft() const { return m_actualBoundingBoxLeft; }
    void setActualBoundingBoxLeft(float actualBoundingBoxLeft) { m_actualBoundingBoxLeft = actualBoundingBoxLeft; }

    float actualBoundingBoxRight() const { return m_actualBoundingBoxRight; }
    void setActualBoundingBoxRight(float actualBoundingBoxRight) { m_actualBoundingBoxRight = actualBoundingBoxRight; }

private:
    TextMetrics()
        : m_width(0)
        , m_actualBoundingBoxLeft(0)
        , m_actualBoundingBoxRight(0)
    { }

    float m_width;
    float m_actualBoundingBoxLeft;
    float m_actualBoundingBoxRight;

};

} // namespace WebCore

#endif // TextMetrics_h
