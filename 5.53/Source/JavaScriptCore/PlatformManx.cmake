if (${JavaScriptCore_LIBRARY_TYPE} STREQUAL "STATIC")
    add_definitions(-DJS_NO_EXPORT)
endif ()

list(APPEND JavaScriptCore_SOURCES
    API/JSMemoryStatsSCE.cpp

    runtime/WatchdogManx.cpp
)

list(REMOVE_ITEM JavaScriptCore_SOURCES
    runtime/WatchdogNone.cpp
)

list(APPEND JavaScriptCore_INCLUDE_DIRECTORIES
    ${LIBJSCBRIDGE_INCLUDE_DIRS}
)

if (MANX_ENABLE_SPLITPROC_JIT)
    add_definitions(-DENABLE_SPLITPROC_JIT=1)
    add_definitions(-DBUILDING_SPLITPROC_VM=1)

    # these JIT functionality will be provided by the bridge
    list(REMOVE_ITEM JavaScriptCore_SOURCES
        jit/AssemblyHelpers.cpp
        jit/BinarySwitch.cpp
        jit/ExecutableAllocationFuzz.cpp
        jit/ExecutableAllocator.cpp
        jit/HostCallReturnValue.cpp
        jit/JITArithmetic.cpp
        jit/JITArithmetic32_64.cpp
        jit/JITCall.cpp
        jit/JITCall32_64.cpp
        jit/JITDisassembler.cpp
        jit/JITInlineCacheGenerator.cpp
        jit/JITOpcodes.cpp
        jit/JITOpcodes32_64.cpp
        jit/JITPropertyAccess.cpp
        jit/JITPropertyAccess32_64.cpp
        jit/JITStubs.cpp
        jit/JITToDFGDeferredCompilationCallback.cpp
        jit/Reg.cpp
        jit/RegisterPreservationWrapperGenerator.cpp
        jit/RegisterSet.cpp
        jit/Repatch.cpp
        jit/ScratchRegisterAllocator.cpp
        jit/SetupVarargsFrame.cpp
        jit/TempRegisterSet.cpp
        jit/ThunkGenerators.cpp

        llint/LLIntThunks.cpp

        yarr/YarrJIT.cpp
    )
endif (MANX_ENABLE_SPLITPROC_JIT)

