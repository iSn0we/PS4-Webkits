/*
 * Copyright (C) 2013 Igalia S.L.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "config.h"
#include "CustomProtocolManager.h"

namespace WebKit {

const char* CustomProtocolManager::supplementName()
{
    return "CustomProtocolManager";
}

CustomProtocolManager::CustomProtocolManager(ChildProcess* childProcess)
{
}

void CustomProtocolManager::initializeConnection(IPC::Connection* connection)
{
}

void CustomProtocolManager::initialize(const WebProcessCreationParameters& parameters)
{
}

#if ENABLE(NETWORK_PROCESS)
void CustomProtocolManager::initialize(const NetworkProcessCreationParameters& parameters)
{
}
#endif

void CustomProtocolManager::registerScheme(const String& scheme)
{
}

void CustomProtocolManager::unregisterScheme(const String&)
{
}

bool CustomProtocolManager::supportsScheme(const String& scheme)
{
    return false;
}

void CustomProtocolManager::didFailWithError(uint64_t customProtocolID, const WebCore::ResourceError& error)
{
}

void CustomProtocolManager::didLoadData(uint64_t customProtocolID, const IPC::DataReference& dataReference)
{
}

void CustomProtocolManager::didReceiveResponse(uint64_t customProtocolID, const WebCore::ResourceResponse& response, uint32_t)
{
}

void CustomProtocolManager::didFinishLoading(uint64_t customProtocolID)
{
}

} // namespace WebKit
