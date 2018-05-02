# Copyright (C) 2013 Google, Inc. All Rights Reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
# PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
# PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
# OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 

package StaticString;

use strict;
use Hasher;

sub GenerateStrings($)
{
    my $stringsRef = shift;
    my %strings = %$stringsRef;

    my @result = ();

    for my $name (sort keys %strings) {
        push(@result, "static const LChar ${name}String8[] = \"$strings{$name}\";\n");
    }

    push(@result, "\n");

    for my $name (sort keys %strings) {
        my $value = $strings{$name};
        my $length = length($value);
        my $hash = Hasher::GenerateHashValue($value);
        my $mask = CalculateMaskFromValue($length);
        push(@result, <<END);
static StringImpl::StaticASCIILiteral ${name}Data = {
    StringImpl::StaticASCIILiteral::s_initialRefCount,
    $length,
    ${name}String8,
    StringImpl::StaticASCIILiteral::s_initialFlags | (${hash} << StringImpl::StaticASCIILiteral::s_hashShift),
    $mask
};
END
    }

    push(@result, "\n");

    return join "", @result;
}

sub GenerateStringAsserts($)
{
    my $stringsRef = shift;
    my %strings = %$stringsRef;

    my @result = ();

    push(@result, "#ifndef NDEBUG\n");

    for my $name (sort keys %strings) {
        push(@result, "    reinterpret_cast<StringImpl*>(&${name}Data)->assertHashIsCorrect();\n");
    }

    push(@result, "#endif // NDEBUG\n");

    push(@result, "\n");

    return join "", @result;
}

sub CalculateMaskFromValue($)
{
    my $v = shift;

    if (!$v) {
        return 0;
    }

    $v--;
    $v |= $v >> 1;
    $v |= $v >> 2;
    $v |= $v >> 4;
    $v |= $v >> 8;
    $v |= $v >> 16;
}

1;