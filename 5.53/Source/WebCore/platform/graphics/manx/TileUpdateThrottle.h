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

#ifndef TileUpdateThrottle_h
#define TileUpdateThrottle_h

#include <unistd.h>

namespace WebCore {

class TileUpdateThrottle {
public:
    TileUpdateThrottle();
    ~TileUpdateThrottle();

    WEBCORE_EXPORT void enableThrottlingByTileCount(int32_t count);
    WEBCORE_EXPORT void enableThrottlingByTime(double timeInMs);
    WEBCORE_EXPORT void disableThrottlingByTileCount();
    WEBCORE_EXPORT void disbleThrottlingByTime();

    bool isEnabled() const { return m_modeFlags; }
    bool isCountingInvisibleTiles() const { return m_isCountingInvisibleTiles; }

    void startCounting();
    void endCounting();
    bool canUpdateTile();

    void incrementInvisibleTileCount() { m_invisibleTileCount++; }
    void incrementUpdatedInvisibleTileCount() { m_updatedInvisibleTileCount++; }
    int32_t invisibleTileCount() const { return m_invisibleTileCount; }
    int32_t updatedInvisibleTileCount() const { return m_updatedInvisibleTileCount; }

private:
    enum {
        ThrottleByTileCount = 0x1,
        ThrottleByTime = 0x2,
    };

    uint32_t m_modeFlags;
    int32_t m_throttlingTileCount;
    int32_t m_invisibleTileCount;
    int32_t m_updatedInvisibleTileCount;
    double m_throttlingTime;
    double m_timeLimit;
    bool m_isCountingInvisibleTiles;
    bool m_hasExceededTimeLimit;
};

} // namespace WebCore

#endif // TileUpdateThrottle_h
