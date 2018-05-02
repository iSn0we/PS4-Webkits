//
// Copyright (c) 2002-2013 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

//
// Implement the top-level of interface to the compiler,
// as defined in ShaderLang.h
//

#include "ShaderLangOrbis.h"
#include <kernel.h>
#include <manx/md5.h>
#include <shader/binary.h>
#include <precompiled_shaders.h>
using namespace PrecompiledShaders;

extern precompiled_shader_t scePrecompiledShaderEntries[];

namespace
{
    bool isInitialized = false;
    precompiled_shader_t* entries = nullptr;

    struct shader_t {
        precompiled_shader_t* entry;
        std::string source;
        std::vector<sh::Uniform> uniforms;
        std::vector<sh::Varying> varyings;
        std::vector<sh::Attribute> attributes;

        shader_t() : entry(0), source("a") { }
    };

    shader_t* slNewInstance()
    {
        return new shader_t;
    }

    void slDeleteInstance(shader_t* shader)
    {
        delete shader;
    }

    bool slPopulateValues(shader_t* shader)
    {
        // Set source
        const auto* entry = shader->entry;

        shader->source = std::string((const char*)entry->code, entry->len);

        // Load program
        sce::Shader::Binary::Program program;

        if (program.loadFromMemory(entry->code, entry->len) != sce::Shader::Binary::kStatusOk) {
            return false;
        }

        // Uniforms
        auto uniformCount = program.m_numConstants;

        for (auto i = 0; i < uniformCount; ++i) {
            const auto* uniform = &program.m_constants[i];

            sh::Uniform mappedUniform;

            // Name is not mapped from one value to another
            std::string name(uniform->getName());
            mappedUniform.name = name;
            mappedUniform.mappedName = name;

            shader->uniforms.push_back(mappedUniform);
        }

        // Attributes
        auto attributeCount = program.m_numInputAttributes;

        for (auto i = 0; i < attributeCount; ++i) {
            const auto* attribute = &program.m_inputAttributes[i];

            sh::Attribute mappedAttribute;

            // Name is not mapped from one value to another
            std::string name(attribute->getName());
            mappedAttribute.name = name;
            mappedAttribute.mappedName = name;

            shader->attributes.push_back(mappedAttribute);
        }

        return true;
    }
}  // namespace anonymous

//
// Driver must call this first, once, before doing any other compiler operations.
// Subsequent calls to this function are no-op.
//
bool ShOrbisInitialize()
{
    if (!isInitialized) {
        entries = &scePrecompiledShaderEntries[0];
        isInitialized = true;
    }
    return true;
}

//
// Cleanup symbol tables
//
bool ShOrbisFinalize()
{
    if (isInitialized)
    {
        isInitialized = false;
    }
    return true;
}

//
// Initialize built-in resources with minimum expected values.
//
void ShOrbisInitBuiltInResources(ShBuiltInResources* resources)
{
}

//
// Driver calls these to create and destroy compiler objects.
//
ShHandle ShOrbisConstructCompiler(
    sh::GLenum type,
    ShShaderSpec spec,
    ShShaderOutput output,
    const ShBuiltInResources* resources)
{
    return (ShHandle)slNewInstance();
}

void ShOrbisDestruct(ShHandle handle)
{
    shader_t* shader = (shader_t*)handle;

    if (shader != nullptr) {
        slDeleteInstance(shader);
    }
}

const std::string &ShOrbisGetBuiltInResourcesString(const ShHandle handle)
{
    static std::string unimplemented;

    return unimplemented;
}

//
// Do an actual compile on the given strings.  The result is left
// in the given compile object.
//
// Return:  The return value of ShCompile is really boolean, indicating
// success or failure.
//
bool ShOrbisCompile(
    const ShHandle handle,
    const char *const shaderStrings[],
    size_t numStrings,
    int compileOptions)
{
    // Currently assuming shaderStrings is always 1 value
    shader_t* shader = (shader_t*)handle;

    // Get the hash
    unsigned char hash[16];
    Manx::Md5::Md5Digest((const void* const)shaderStrings[0], strlen(shaderStrings[0]), hash);

    precompiled_shader_t* entry = entries;

    while ((entry != nullptr) && (entry->ident != nullptr)) {
        if (memcmp(entry->hash, hash, 16) == 0) {
            shader->entry = entry;

            return slPopulateValues(shader);
        }

        entry++;
    }

    return false;
}

int ShOrbisGetShaderVersion(const ShHandle handle)
{
    return 0;
}

ShShaderOutput ShOrbisGetShaderOutputType(const ShHandle handle)
{
    return SH_ESSL_OUTPUT;
}

//
// Return any compiler log of messages for the application.
//
const std::string &ShOrbisGetInfoLog(const ShHandle handle)
{
    static std::string unimplemented;

    return unimplemented;
}

//
// Return any object code.
//
const std::string &ShOrbisGetObjectCode(const ShHandle handle)
{
    static std::string unimplemented;
    const shader_t* shader = (const shader_t*)handle;

    return ((shader != nullptr) && (shader->entry != nullptr))
        ? shader->source
        : unimplemented;
}

const std::map<std::string, std::string> *ShOrbisGetNameHashingMap(
    const ShHandle handle)
{
    static std::map<std::string, std::string> unimplemented;

    return (const std::map<std::string, std::string>*)&unimplemented;
}

const std::vector<sh::Uniform> *ShOrbisGetUniforms(const ShHandle handle)
{
    const shader_t* shader = (const shader_t*)handle;

    return &shader->uniforms;
}

const std::vector<sh::Varying> *ShOrbisGetVaryings(const ShHandle handle)
{
    const shader_t* shader = (const shader_t*)handle;

    return &shader->varyings;
}

const std::vector<sh::Attribute> *ShOrbisGetAttributes(const ShHandle handle)
{
    const shader_t* shader = (const shader_t*)handle;

    return &shader->attributes;
}

const std::vector<sh::Attribute> *ShOrbisGetOutputVariables(const ShHandle handle)
{
    // NOT USED IN WEBKIT
    static std::vector<sh::Attribute> unimplemented;

    return (const std::vector<sh::Attribute>*)&unimplemented;
}

const std::vector<sh::InterfaceBlock> *ShOrbisGetInterfaceBlocks(const ShHandle handle)
{
    // NOT USED IN WEBKIT
    static std::vector<sh::InterfaceBlock> unimplementd;

    return (const std::vector<sh::InterfaceBlock>*)&unimplementd;
}

bool ShOrbisCheckVariablesWithinPackingLimits(
    int maxVectors, ShVariableInfo *varInfoArray, size_t varInfoArraySize)
{
    // NOT USED IN WEBKIT
    return false;
}

bool ShOrbisGetInterfaceBlockRegister(const ShHandle handle,
    const std::string &interfaceBlockName,
    unsigned int *indexOut)
{
    // NOT USED IN WEBKIT
    return false;
}

bool ShOrbisGetUniformRegister(const ShHandle handle,
    const std::string &uniformName,
    unsigned int *indexOut)
{
    // NOT USED IN WEBKIT
    return false;
}
