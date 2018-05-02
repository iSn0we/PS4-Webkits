# - Try to find Pixman
# This module defines the following variables:
#
#  PIXMAN_FOUND - Pixman 2.4 was found
#  PIXMAN_INCLUDE_DIRS - the Pixman 2.4 include directories
#  PIXMAN_LIBRARIES - link these to use Pixman 2.4
#
# Copyright (C) 2015 Sony Interactive Entertainment Inc.
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

find_path(PIXMAN_INCLUDE_DIRS
    NAMES pixman.h
)

find_library(PIXMAN_LIBRARIES
    NAMES pixman-1
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(PIXMAN REQUIRED_VARS PIXMAN_INCLUDE_DIRS PIXMAN_LIBRARIES)

mark_as_advanced(
    PIXMAN_INCLUDE_DIRS
    PIXMAN_LIBRARIES
)
