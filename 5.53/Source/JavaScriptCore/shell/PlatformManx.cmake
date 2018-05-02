if (ORBIS)
  if (ENABLE_JIT)
    if (MANX_ENABLE_SPLITPROC_JIT)
      list(APPEND JSC_LIBRARIES JscBridge_vm SceJitBridge)
    endif (MANX_ENABLE_SPLITPROC_JIT)
  endif (ENABLE_JIT)
endif (ORBIS)

