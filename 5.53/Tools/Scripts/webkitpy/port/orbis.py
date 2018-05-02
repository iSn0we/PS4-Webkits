# Copyright (C) 2015 Sony Computer Entertainment Inc.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met:
#
#     * Redistributions of source code must retain the above copyright
# notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above
# copyright notice, this list of conditions and the following disclaimer
# in the documentation and/or other materials provided with the
# distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

"""WebKit Manx port implementation of the Port interface."""

import logging
import subprocess
import sys

from webkitpy.common.system.path import cygpath
from webkitpy.layout_tests.models.test_configuration import TestConfiguration
from webkitpy.port.base import Port

_log = logging.getLogger(__name__)

class OrbisPort(Port):
    port_name = 'orbis'

    DEFAULT_ARCHITECTURE = 'x86_64'

    def _generate_all_test_configurations(self):
        return [TestConfiguration(version=self._version, architecture='x86_64', build_type=build_type) for build_type in self.ALL_BUILD_TYPES]
    def _build_driver(self):
        # Does not support yet. Do nothing
        return True
    def operating_system(self):
        return 'orbis'
    def default_child_processes(self):
        return 1
    def show_results_html_file(self, results_filename):
        win_abspath = cygpath(results_filename)
        subprocess.call(["cygstart", win_abspath])
    def _uses_apache(self):
        return False
    def _path_to_lighttpd(self):
        return "/usr/sbin/lighttpd"
    def _path_to_lighttpd_modules(self):
        return "/usr/lib/lighttpd"
    def _path_to_lighttpd_php(self):
        return "/usr/bin/php-cgi.exe"
