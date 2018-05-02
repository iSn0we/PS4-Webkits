set(CMAKE_SYSTEM_NAME ORBIS)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

if(${CMAKE_GENERATOR} MATCHES "Visual Studio")
    set(CMAKE_GENERATOR_PLATFORM ORBIS)
endif ()

find_program(orbis_clang_executable orbis-clang
  PATHS ENV SCE_ORBIS_SDK_DIR
  PATH_SUFFIXES host_tools/bin
  NO_SYSTEM_ENVIRONMENT_PATH
)

find_program(orbis_clangxx_executable orbis-clang++
  PATHS ENV SCE_ORBIS_SDK_DIR
  PATH_SUFFIXES host_tools/bin
  NO_SYSTEM_ENVIRONMENT_PATH
)

set(CMAKE_C_COMPILER ${orbis_clang_executable})
set(CMAKE_CXX_COMPILER ${orbis_clangxx_executable})

file(TO_CMAKE_PATH $ENV{SCE_ORBIS_SDK_DIR} sce_orbis_sdk_dir)
