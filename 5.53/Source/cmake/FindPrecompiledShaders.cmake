# - Try to find Libmanx
# This module defines the following variables:
#
#  PRECOMPIED_SHADERS_FOUND - ScePrecompiledShaders was found
#  PRECOMPILED_SHADERS_INCLUDE_DIRS - ScePrecompiledShaders' include directories
#  PRECOMPILED_SHADERS_LIBRARIES - link these to use ScePrecompiledShaders
#
# Copyright (C) 2016 Sony Interactive Entertainment Inc.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1.  Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
# 2.  Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in the
#     documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND ITS CONTRIBUTORS ``AS
# IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
# THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
# PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR ITS
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
# OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
# OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
# ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

find_path(PRECOMPILED_SHADERS_INCLUDE_DIRS
    NAMES precompiled_shaders.h
)

find_library(PRECOMPILED_SHADERS_LIBRARIES
    NAMES ScePrecompiledShaders_stub_weak
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(PRECOMPILED_SHADERS REQUIRED_VARS PRECOMPILED_SHADERS_INCLUDE_DIRS PRECOMPILED_SHADERS_LIBRARIES)

mark_as_advanced(
    PRECOMPILED_SHADERS_INCLUDE_DIRS
    PRECOMPILED_SHADERS_LIBRARIES
)
