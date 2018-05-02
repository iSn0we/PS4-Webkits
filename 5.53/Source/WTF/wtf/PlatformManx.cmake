if (SHARED_CORE)
    set(WTF_LIBRARY_TYPE SHARED)
else ()
    set(WTF_LIBRARY_TYPE STATIC)
endif ()

list(APPEND WTF_SOURCES
    manx/MainThreadManx.cpp
    manx/RunLoopManx.cpp
    manx/WorkQueueManx.cpp
)

list(APPEND WTF_LIBRARIES
    ${LIBMANX_LIBRARIES}
    ${sce_sdk_libraries}
)

list(APPEND WTF_INCLUDE_DIRECTORIES
    ${LIBMANX_INCLUDE_DIRS}
    ${LIBJSCBRIDGE_INCLUDE_DIRS}
)

if (MANX_ENABLE_SPLITPROC_JIT)
    add_definitions(-DENABLE_SPLITPROC_JIT=1)
endif (MANX_ENABLE_SPLITPROC_JIT)
