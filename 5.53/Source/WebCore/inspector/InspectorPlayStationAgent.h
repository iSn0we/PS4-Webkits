/*
 * Copyright (C) 2016 Sony Computer Entertainment Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef InspectorPlayStationAgent_h
#define InspectorPlayStationAgent_h

#if ENABLE(MANX_PLAYSTATION_INSPECTOR_DOMAIN)

#include "InspectorWebAgentBase.h"
#include <inspector/InspectorBackendDispatchers.h>
#include <wtf/text/WTFString.h>

namespace Inspector {
class InjectedScriptManager;
}

namespace WebCore {

class InspectorClient;
class InspectorPageAgent;

typedef String ErrorString;

class InspectorPlayStationAgent final : public InspectorAgentBase, public Inspector::PlayStationBackendDispatcherHandler {
    WTF_MAKE_FAST_ALLOCATED;
public:
    InspectorPlayStationAgent(InstrumentingAgents*, InspectorPageAgent*, Inspector::InjectedScriptManager*, InspectorClient*);
    virtual ~InspectorPlayStationAgent();

    virtual void didCreateFrontendAndBackend(Inspector::FrontendChannel*, Inspector::BackendDispatcher*) override;
    virtual void willDestroyFrontendAndBackend(Inspector::DisconnectReason) override;

    // Called from the front-end.
    virtual void getACMemoryInfo(ErrorString&, RefPtr<Inspector::Protocol::PlayStation::ACMemoryInfo>& result) override;
    virtual void showLayerTable(ErrorString&, bool in_show) override;

private:
    InspectorPageAgent* m_pageAgent;
    Inspector::InjectedScriptManager* m_injectedScriptManager;
    InspectorClient* m_client;
    RefPtr<Inspector::PlayStationBackendDispatcher> m_backendDispatcher;
};

} // namespace WebCore

#endif // ENABLE(MANX_PLAYSTATION_INSPECTOR_DOMAIN)

#endif
