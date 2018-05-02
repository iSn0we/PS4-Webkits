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
#include "TileUpdateThrottle.h"

#include <wtf/CurrentTime.h>

namespace WebCore {

TileUpdateThrottle::TileUpdateThrottle()
    : m_modeFlags(0)
    , m_throttlingTileCount(0)
    , m_invisibleTileCount(0)
    , m_updatedInvisibleTileCount(0)
    , m_throttlingTime(0)
    , m_timeLimit(0)
    , m_isCountingInvisibleTiles(false)
    , m_hasExceededTimeLimit(false)
{
}

TileUpdateThrottle::~TileUpdateThrottle()
{
}

void TileUpdateThrottle::enableThrottlingByTileCount(int32_t count)
{
    m_modeFlags |= ThrottleByTileCount;
    m_throttlingTileCount = count;
}

void TileUpdateThrottle::enableThrottlingByTime(double timeInMs)
{
    m_modeFlags |= ThrottleByTime;
    m_throttlingTime = timeInMs * 0.001;
}

void TileUpdateThrottle::disableThrottlingByTileCount()
{
    m_modeFlags &= ~ThrottleByTileCount;
    m_throttlingTileCount = 0;
}

void TileUpdateThrottle::disbleThrottlingByTime()
{
    m_modeFlags &= ~ThrottleByTime;
    m_throttlingTime = 0;
}

void TileUpdateThrottle::startCounting()
{
    m_invisibleTileCount = 0;
    m_updatedInvisibleTileCount = 0;
    m_isCountingInvisibleTiles = true;

    m_timeLimit = monotonicallyIncreasingTime() + m_throttlingTime;
    m_hasExceededTimeLimit = false;
}

void TileUpdateThrottle::endCounting()
{
    m_isCountingInvisibleTiles = false;
}

bool TileUpdateThrottle::canUpdateTile()
{
    if (m_modeFlags & ThrottleByTileCount) {
        if (m_updatedInvisibleTileCount >= m_throttlingTileCount)
            return false;
    }
    if (m_modeFlags & ThrottleByTime) {
        if (m_hasExceededTimeLimit)
            return false;

        if (monotonicallyIncreasingTime() > m_timeLimit) {
            m_hasExceededTimeLimit = true;
            return false;
        }
    }

    return true;
}

} // namespace WebCore
