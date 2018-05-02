//
// Copyright (c) 2002-2013 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

//
// Implement the top-level of interface to the compiler,
// as defined in ShaderLang.h
//

#include "ShaderLangANGLE.h"
#include "ShaderLangOrbis.h"
#include <GLES2/gl2.h>
#include <stdio.h>

#define COMPILER_RUNTIME GL_TRUE
#define COMPILER_BUILTIN GL_FALSE

// Conditionally output shaders
#define OUTPUT_SHADER_SOURCE 0

#if OUTPUT_SHADER_SOURCE
#include <string.h>
#endif

namespace {

bool isInitialized = false;
GLboolean compiler = COMPILER_BUILTIN;

} // namespace anonymous

//
// Driver must call this first, once, before doing any other compiler operations.
// Subsequent calls to this function are no-op.
//
bool ShInitialize()
{
    if (!isInitialized)
    {
        glGetBooleanv(GL_SHADER_COMPILER, &compiler);

        if (compiler == COMPILER_RUNTIME) {
            fprintf(stderr, "[INFO] Using runtime ESSL compiler\n");
            isInitialized = ShAngleInitialize();
        }
        else {
            fprintf(stderr, "[INFO] Using builtin ESSL shaders\n");
            isInitialized = ShOrbisInitialize();
        }
    }
    return isInitialized;
}

//
// Cleanup symbol tables
//
bool ShFinalize()
{
    if (isInitialized)
    {
        isInitialized = false;
    }

    return (compiler == COMPILER_RUNTIME) ? ShAngleFinalize() : ShOrbisFinalize();
}

//
// Initialize built-in resources with minimum expected values.
//
void ShInitBuiltInResources(ShBuiltInResources* resources)
{
    if (compiler == COMPILER_RUNTIME) {
        ShAngleInitBuiltInResources(resources);
    }
    else {
        ShOrbisInitBuiltInResources(resources);
    }
}

//
// Driver calls these to create and destroy compiler objects.
//
ShHandle ShConstructCompiler(sh::GLenum type, ShShaderSpec spec,
                             ShShaderOutput output,
                             const ShBuiltInResources* resources)
{
    return (compiler == COMPILER_RUNTIME)
        ? ShAngleConstructCompiler(type, spec, output, resources)
        : ShOrbisConstructCompiler(type, spec, output, resources);
}

void ShDestruct(ShHandle handle)
{
    if (compiler == COMPILER_RUNTIME) {
        ShAngleDestruct(handle);
    }
    else {
        ShOrbisDestruct(handle);
    }
}

const std::string &ShGetBuiltInResourcesString(const ShHandle handle)
{
    return (compiler == COMPILER_RUNTIME)
        ? ShAngleGetBuiltInResourcesString(handle)
        : ShOrbisGetBuiltInResourcesString(handle);
}

//
// Do an actual compile on the given strings.  The result is left
// in the given compile object.
//
// Return:  The return value of ShCompile is really boolean, indicating
// success or failure.
//
bool ShCompile(
    const ShHandle handle,
    const char *const shaderStrings[],
    size_t numStrings,
    int compileOptions)
{
#if OUTPUT_SHADER_SOURCE
    static int vertCount = 0;
    static int fragCount = 0;
    static const char* fragColor = "gl_FragColor";

    for (size_t i = 0; i < numStrings; ++i) {
        const char* source = shaderStrings[i];
        bool isFrag = strstr(source, fragColor) != NULL;

        char filename[255];

        if (isFrag) {
            sprintf(filename, "/hostapp/f_%d.frag", fragCount++);
        }
        else {
            sprintf(filename, "/hostapp/v_%d.vert", vertCount++);
        }

        printf("Writing shader source to %s\n", filename);
        FILE* fp = fopen(filename, "w");
        fprintf(fp, source);
        fclose(fp);
    }
#endif
    return (compiler == COMPILER_RUNTIME)
        ? ShAngleCompile(handle, shaderStrings, numStrings, compileOptions)
        : ShOrbisCompile(handle, shaderStrings, numStrings, compileOptions);
}

int ShGetShaderVersion(const ShHandle handle)
{
    return (compiler == COMPILER_RUNTIME)
        ? ShAngleGetShaderVersion(handle)
        : ShOrbisGetShaderVersion(handle);
}

ShShaderOutput ShGetShaderOutputType(const ShHandle handle)
{
    return (compiler == COMPILER_RUNTIME)
        ? ShAngleGetShaderOutputType(handle)
        : ShOrbisGetShaderOutputType(handle);
}

//
// Return any compiler log of messages for the application.
//
const std::string &ShGetInfoLog(const ShHandle handle)
{
    return (compiler == COMPILER_RUNTIME)
        ? ShAngleGetInfoLog(handle)
        : ShOrbisGetInfoLog(handle);
}

//
// Return any object code.
//
const std::string &ShGetObjectCode(const ShHandle handle)
{
    return (compiler == COMPILER_RUNTIME)
        ? ShAngleGetObjectCode(handle)
        : ShOrbisGetObjectCode(handle);
}

const std::map<std::string, std::string> *ShGetNameHashingMap(
    const ShHandle handle)
{
    return (compiler == COMPILER_RUNTIME)
        ? ShAngleGetNameHashingMap(handle)
        : ShOrbisGetNameHashingMap(handle);
}

const std::vector<sh::Uniform> *ShGetUniforms(const ShHandle handle)
{
    return (compiler == COMPILER_RUNTIME)
        ? ShAngleGetUniforms(handle)
        : ShOrbisGetUniforms(handle);
}

const std::vector<sh::Varying> *ShGetVaryings(const ShHandle handle)
{
    return (compiler == COMPILER_RUNTIME)
        ? ShAngleGetVaryings(handle)
        : ShOrbisGetVaryings(handle);
}

const std::vector<sh::Attribute> *ShGetAttributes(const ShHandle handle)
{
    return (compiler == COMPILER_RUNTIME)
        ? ShAngleGetAttributes(handle)
        : ShOrbisGetAttributes(handle);
}

const std::vector<sh::Attribute> *ShGetOutputVariables(const ShHandle handle)
{
    return (compiler == COMPILER_RUNTIME)
        ? ShAngleGetOutputVariables(handle)
        : ShOrbisGetOutputVariables(handle);
}

const std::vector<sh::InterfaceBlock> *ShGetInterfaceBlocks(const ShHandle handle)
{
    return (compiler == COMPILER_RUNTIME)
        ? ShAngleGetInterfaceBlocks(handle)
        : ShOrbisGetInterfaceBlocks(handle);
}

bool ShCheckVariablesWithinPackingLimits(
    int maxVectors, ShVariableInfo *varInfoArray, size_t varInfoArraySize)
{
    return (compiler == COMPILER_RUNTIME)
        ? ShAngleCheckVariablesWithinPackingLimits(maxVectors, varInfoArray, varInfoArraySize)
        : ShOrbisCheckVariablesWithinPackingLimits(maxVectors, varInfoArray, varInfoArraySize);
}

bool ShGetInterfaceBlockRegister(const ShHandle handle,
                                 const std::string &interfaceBlockName,
                                 unsigned int *indexOut)
{
    return (compiler == COMPILER_RUNTIME)
        ? ShAngleGetInterfaceBlockRegister(handle, interfaceBlockName, indexOut)
        : ShOrbisGetInterfaceBlockRegister(handle, interfaceBlockName, indexOut);
}

bool ShGetUniformRegister(const ShHandle handle,
                          const std::string &uniformName,
                          unsigned int *indexOut)
{
    return (compiler == COMPILER_RUNTIME)
        ? ShAngleGetUniformRegister(handle, uniformName, indexOut)
        : ShOrbisGetUniformRegister(handle, uniformName, indexOut);
}
