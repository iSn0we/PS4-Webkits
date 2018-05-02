/*
 * Copyright (C) 2014 Sony Interactive Entertainment Inc.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "NetworkStateNotifier.h"

#include <ntf/NetworkControl.h>

namespace WebCore {

static const double StateChangeTimerInterval = 2.5;

NetworkStateNotifier::NetworkStateNotifier()
    : m_isOnLine(false)
    , m_networkStateChangeTimer(*this, &NetworkStateNotifier::networkStateChangeTimerFired)
{
    updateState();
    m_networkStateChangeTimer.startOneShot(StateChangeTimerInterval);
}

NetworkStateNotifier::~NetworkStateNotifier()
{
    m_networkStateChangeTimer.stop();
}

void NetworkStateNotifier::networkStateChangeTimerFired()
{
    updateState();
    m_networkStateChangeTimer.startOneShot(StateChangeTimerInterval);
}

void NetworkStateNotifier::updateState()
{
    bool oldOnLine = m_isOnLine;
    m_isOnLine = NTF::NetworkControl::isOnLine();

    if (m_isOnLine == oldOnLine)
        return;

    notifyNetworkStateChange();
}

} // namespace WebCore