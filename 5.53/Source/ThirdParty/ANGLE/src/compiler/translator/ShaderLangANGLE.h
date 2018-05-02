//
// Copyright (c) 2002-2010 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
#ifndef _ANGLE_COMPILER_INTERFACE_INCLUDED_
#define _ANGLE_COMPILER_INTERFACE_INCLUDED_

#include "GLSLANG/ShaderLang.h"

// ANGLE implementation of ShInitialize.
bool ShAngleInitialize();

// ANGLE implementation of ShFinalize.
bool ShAngleFinalize();

// ANGLE implementation of ShInitBuiltInResources.
void ShAngleInitBuiltInResources(ShBuiltInResources *resources);

// ANGLE implementation of ShGetBuiltInResourcesString.
const std::string &ShAngleGetBuiltInResourcesString(const ShHandle handle);

// ANGLE implementation of ShConstructCompiler.
ShHandle ShAngleConstructCompiler(
    sh::GLenum type,
    ShShaderSpec spec,
    ShShaderOutput output,
    const ShBuiltInResources *resources);

// ANGLE implementation of ShDestruct.
void ShAngleDestruct(ShHandle handle);

// ANGLE implementation of ShCompile.
bool ShAngleCompile(
    const ShHandle handle,
    const char * const shaderStrings[],
    size_t numStrings,
    int compileOptions);

// ANGLE implementation of ShGetShaderVersion.
int ShAngleGetShaderVersion(const ShHandle handle);

// ANGLE implementation of ShGetShaderVersion.
ShShaderOutput ShAngleGetShaderOutputType(
    const ShHandle handle);

// ANGLE implementation of ShGetInfoLog.
const std::string &ShAngleGetInfoLog(const ShHandle handle);

// ANGLE implementation of ShGetObjectCode.
const std::string &ShAngleGetObjectCode(const ShHandle handle);

// ANGLE implementation of ShGetNameHashingMap.
const std::map<std::string, std::string> *ShAngleGetNameHashingMap(
    const ShHandle handle);

// ANGLE implementation of ShGetUniforms.
const std::vector<sh::Uniform> *ShAngleGetUniforms(const ShHandle handle);

// ANGLE implementation of ShGetVaryings.
const std::vector<sh::Varying> *ShAngleGetVaryings(const ShHandle handle);

// ANGLE implementation of ShGetAttributes.
const std::vector<sh::Attribute> *ShAngleGetAttributes(const ShHandle handle);

// ANGLE implementation of ShGetOutputVariables.
const std::vector<sh::Attribute> *ShAngleGetOutputVariables(const ShHandle handle);

// ANGLE implementation of ShGetInterfaceBlocks.
const std::vector<sh::InterfaceBlock> *ShAngleGetInterfaceBlocks(const ShHandle handle);

// ANGLE implementation of ShCheckVariablesWithinPackingLimits.
bool ShAngleCheckVariablesWithinPackingLimits(
    int maxVectors,
    ShVariableInfo *varInfoArray,
    size_t varInfoArraySize);

// ANGLE implementation of ShGetInterfaceBlockRegister.
bool ShAngleGetInterfaceBlockRegister(
    const ShHandle handle,
    const std::string &interfaceBlockName,
    unsigned int *indexOut);

// ANGLE implementation of ShGetUniformRegister.
bool ShAngleGetUniformRegister(
    const ShHandle handle,
    const std::string &uniformName,
    unsigned int *indexOut);

#endif // _ANGLE_COMPILER_INTERFACE_INCLUDED_
