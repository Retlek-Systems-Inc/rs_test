#######################################################################
# @copyright 2020 Retlek Systems Inc.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#
# CMake Script for module level

cmake_minimum_required(VERSION 3.22)
cmake_policy(SET CMP0048 NEW) # project version
cmake_policy(SET CMP0076 NEW) # full paths

########################################################################
#
# Project-wide settings
project(rstest VERSION 0.0.1 LANGUAGES C)

########################################################################
# Requirements
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

#set(CMAKE_CXX_EXTENSIONS OFF)

# -----------------------------------------------------------------------------
add_compile_options(
    $<$<COMPILE_LANG_AND_ID:C,GNU>:-fdiagnostics-color=always>
    $<$<COMPILE_LANG_AND_ID:C,Clang>:-fcolor-diagnostics>

    $<$<COMPILE_LANG_AND_ID:C,Clang,GNU>:-Wall>
    $<$<COMPILE_LANG_AND_ID:C,Clang,GNU>:-Wextra>
    $<$<COMPILE_LANG_AND_ID:C,Clang>:-Wpedantic> # Note not used in GNU to allow C99 designated initializers.
    $<$<COMPILE_LANG_AND_ID:C,Clang,GNU>:-Werror>
    $<$<COMPILE_LANG_AND_ID:C,Clang>:-Weverything>

    $<$<COMPILE_LANG_AND_ID:C,Clang>:-Wno-c++98-compat>
    $<$<COMPILE_LANG_AND_ID:C,Clang>:-Wno-c++98-compat-pedantic>
    $<$<COMPILE_LANG_AND_ID:C,Clang>:-Wno-declaration-after-statement>
    $<$<COMPILE_LANG_AND_ID:C,Clang>:-Wno-covered-switch-default> # Allowing defaults in case statements with full coverage of defined enum.

    # Documentation types
    $<$<COMPILE_LANG_AND_ID:C,Clang>:-fcomment-block-commands=retval> # Doesn't recongize doxygen retval
    $<$<COMPILE_LANG_AND_ID:C,Clang>:-fcomment-block-commands=copydetails> # Doesn't recongize doxygen copydetails
)

# -----------------------------------------------------------------------------
add_library(rstest_lib STATIC)
add_library(RsTest::RsTest ALIAS rstest_lib)

target_sources(rstest_lib
  PRIVATE
    api/rstest/rstest_std_macros.h
    api/rstest/rstest.h

    src/rstest_internal.h
    src/rstest.c
)

target_include_directories(rstest_lib
  PUBLIC
    api
  PRIVATE
    src
)

# -----------------------------------------------------------------------------
add_subdirectory(example)


########################################################################
# Install rules
# Not intended for installation but for inclusion with a pre-existing
# cmake build environment.