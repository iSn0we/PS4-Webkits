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

#include "config.h"

#if ENABLE(MANX_PLAYSTATION_INSPECTOR_DOMAIN)

#include "InspectorPlayStationAgent.h"

#include "InspectorClient.h"
#include "InspectorPageAgent.h"
#include "InstrumentingAgents.h"
#include "Page.h"
#include <inspector/InjectedScript.h>
#include <inspector/InjectedScriptManager.h>
#include <inspector/InspectorFrontendDispatchers.h>
#include <inspector/InspectorValues.h>
#include <wtf/NeverDestroyed.h>
#include <wtf/Vector.h>

using namespace Inspector;

namespace WebCore {

InspectorPlayStationAgent::InspectorPlayStationAgent(InstrumentingAgents* instrumentingAgents, InspectorPageAgent* pageAgent, InjectedScriptManager* injectedScriptManager, InspectorClient* client)
    : InspectorAgentBase(ASCIILiteral("PlayStation"), instrumentingAgents)
    , m_pageAgent(pageAgent)
    , m_injectedScriptManager(injectedScriptManager)
    , m_client(client)
{
}

InspectorPlayStationAgent::~InspectorPlayStationAgent()
{
}

void InspectorPlayStationAgent::didCreateFrontendAndBackend(Inspector::FrontendChannel*, Inspector::BackendDispatcher* backendDispatcher)
{
    m_backendDispatcher = Inspector::PlayStationBackendDispatcher::create(backendDispatcher, this);
}

void InspectorPlayStationAgent::willDestroyFrontendAndBackend(Inspector::DisconnectReason)
{
    m_backendDispatcher = nullptr;
}

void InspectorPlayStationAgent::getACMemoryInfo(ErrorString& errorString, RefPtr<Inspector::Protocol::PlayStation::ACMemoryInfo>& result)
{
    Page* page = m_pageAgent->page();
    const GLMemoryInfo& info = page->acMemoryInfo();
    result = Inspector::Protocol::PlayStation::ACMemoryInfo::create()
        .setHeap(info.heap)
        .setTexture(info.texture)
        .setSurfaces(info.surfaces)
        .setPrograms(info.programs)
        .setBuffers(info.buffers)
        .setCommandBuffers(info.commandBuffers)
        .setTotal(info.total)
        .setMaxAllowed(info.maxAllowed)
        .setFmemMappedSizeTotal(info.fmemMappedSizeTotal)
        .setFmemMappedSizeLimit(info.fmemMappedSizeLimit)
        .release();
}

void InspectorPlayStationAgent::showLayerTable(ErrorString&, bool in_show)
{
    m_client->showLayerTable(in_show);
}

} // namespace WebCore
#endif // ENABLE(MANX_PLAYSTATION_INSPECTOR_DOMAIN)
