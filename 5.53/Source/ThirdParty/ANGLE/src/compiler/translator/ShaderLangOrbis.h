//
// Copyright (c) 2002-2010 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
#ifndef _ORBIS_COMPILER_INTERFACE_INCLUDED_
#define _ORBIS_COMPILER_INTERFACE_INCLUDED_

#include "GLSLANG/ShaderLang.h"

// Orbis implementation of ShInitialize.
bool ShOrbisInitialize();

// Orbis implementation of ShFinalize.
bool ShOrbisFinalize();

// Orbis implementation of ShInitBuiltInResources.
void ShOrbisInitBuiltInResources(ShBuiltInResources *resources);

// Orbis implementation of ShGetBuiltInResourcesString.
const std::string &ShOrbisGetBuiltInResourcesString(const ShHandle handle);

// Orbis implementation of ShConstructCompiler.
ShHandle ShOrbisConstructCompiler(
    sh::GLenum type,
    ShShaderSpec spec,
    ShShaderOutput output,
    const ShBuiltInResources *resources);

// Orbis implementation of ShDestruct.
void ShOrbisDestruct(ShHandle handle);

// Orbis implementation of ShCompile.
bool ShOrbisCompile(
    const ShHandle handle,
    const char * const shaderStrings[],
    size_t numStrings,
    int compileOptions);

// Orbis implementation of ShGetShaderVersion.
int ShOrbisGetShaderVersion(const ShHandle handle);

// Orbis implementation of ShGetShaderVersion.
ShShaderOutput ShOrbisGetShaderOutputType(
    const ShHandle handle);

// Orbis implementation of ShGetInfoLog.
const std::string &ShOrbisGetInfoLog(const ShHandle handle);

// Orbis implementation of ShGetObjectCode.
const std::string &ShOrbisGetObjectCode(const ShHandle handle);

// Orbis implementation of ShGetNameHashingMap.
const std::map<std::string, std::string> *ShOrbisGetNameHashingMap(
    const ShHandle handle);

// Orbis implementation of ShGetUniforms.
const std::vector<sh::Uniform> *ShOrbisGetUniforms(const ShHandle handle);

// Orbis implementation of ShGetVaryings.
const std::vector<sh::Varying> *ShOrbisGetVaryings(const ShHandle handle);

// Orbis implementation of ShGetAttributes.
const std::vector<sh::Attribute> *ShOrbisGetAttributes(const ShHandle handle);

// Orbis implementation of ShGetOutputVariables.
const std::vector<sh::Attribute> *ShOrbisGetOutputVariables(const ShHandle handle);

// Orbis implementation of ShGetInterfaceBlocks.
const std::vector<sh::InterfaceBlock> *ShOrbisGetInterfaceBlocks(const ShHandle handle);

// Orbis implementation of ShCheckVariablesWithinPackingLimits.
bool ShOrbisCheckVariablesWithinPackingLimits(
    int maxVectors,
    ShVariableInfo *varInfoArray,
    size_t varInfoArraySize);

// Orbis implementation of ShGetInterfaceBlockRegister.
bool ShOrbisGetInterfaceBlockRegister(
    const ShHandle handle,
    const std::string &interfaceBlockName,
    unsigned int *indexOut);

// Orbis implementation of ShGetUniformRegister.
bool ShOrbisGetUniformRegister(
    const ShHandle handle,
    const std::string &uniformName,
    unsigned int *indexOut);

#endif // _ORBIS_COMPILER_INTERFACE_INCLUDED_
