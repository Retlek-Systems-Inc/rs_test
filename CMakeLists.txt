#######################################################################
# @copyright 2023 Retlek Systems Inc.
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
cmake_policy(SET CMP0065 NEW) # do not add rdynamic unless explicitly stated
cmake_policy(SET CMP0076 NEW) # full paths
cmake_policy(SET CMP0077 NEW) # options do nothing when defined as variable.
#cmake_policy(SET CMP0135 NEW) # fetchcontent url timestamp cmake 3.25

option(BUILD_TEST      "Builds the tests"           ON )
option(BUILD_DOC       "Builds the documentation"   OFF)
option(STATIC_ANALYSIS "Use Static Analysis tools." ON )
option(BUILD_BENCHMARK "No Benchmark tests"         OFF)

set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

# Design & Implementation
include(FetchContent)

#options from front-end build environment: BUILD_TEST, STATIC_ANALYSIS, BUILD_DOC, VERILOG_TEST
FetchContent_Declare( cmake
  GIT_REPOSITORY  https://rscmake_clone_access:h1SwGbFex6ScmtSWzzGJ@gitlab.com/retleksystems/env/cmake.git
  GIT_TAG         v0.2.3
)

FetchContent_GetProperties(cmake)
if(NOT cmake_POPULATED)
  FetchContent_Populate(cmake)
  include(${cmake_SOURCE_DIR}/Init.cmake)
endif()

option(RSTEST_MINIMAL_INFO "Builds with absolute minimum stored information"   OFF)
option(RSTEST_BUILD_EXAMPLE "Build example code."   OFF)

########################################################################
#
# Project-wide settings
project(rstest VERSION 0.0.1 LANGUAGES C CXX)

########################################################################
# Requirements
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
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

    $<$<COMPILE_LANG_AND_ID:C,Clang>:-Wno-declaration-after-statement>
    $<$<COMPILE_LANG_AND_ID:C,Clang>:-Wno-covered-switch-default> # Allowing defaults in case statements with full coverage of defined enum.

    # CXX for test environment
    # Documentation types
    $<$<COMPILE_LANG_AND_ID:CXX,Clang>:-fcomment-block-commands=retval> # Doesn't recongize doxygen retval
    $<$<COMPILE_LANG_AND_ID:CXX,Clang>:-fcomment-block-commands=copydetails> # Doesn't recongize doxygen copydetails

    $<$<COMPILE_LANG_AND_ID:CXX,GNU>:-fdiagnostics-color=always>
    $<$<COMPILE_LANG_AND_ID:CXX,Clang>:-fcolor-diagnostics>

    $<$<COMPILE_LANG_AND_ID:CXX,Clang,GNU>:-Wall>
    $<$<COMPILE_LANG_AND_ID:CXX,Clang,GNU>:-Wextra>
    $<$<COMPILE_LANG_AND_ID:CXX,Clang>:-Wpedantic> # Note not used in GNU to allow C99 designated initializers.
    $<$<COMPILE_LANG_AND_ID:CXX,Clang,GNU>:-Werror>
    $<$<COMPILE_LANG_AND_ID:CXX,Clang>:-Weverything>

    $<$<COMPILE_LANG_AND_ID:CXX,Clang>:-Wno-c++98-compat>
    $<$<COMPILE_LANG_AND_ID:CXX,Clang>:-Wno-c++98-compat-pedantic>
    $<$<COMPILE_LANG_AND_ID:CXX,Clang>:-Wno-declaration-after-statement>
    $<$<COMPILE_LANG_AND_ID:CXX,Clang>:-Wno-covered-switch-default> # Allowing defaults in case statements with full coverage of defined enum.

    # Documentation types
    $<$<COMPILE_LANG_AND_ID:CXX,Clang>:-fcomment-block-commands=retval> # Doesn't recongize doxygen retval
    $<$<COMPILE_LANG_AND_ID:CXX,Clang>:-fcomment-block-commands=copydetails> # Doesn't recongize doxygen copydetails
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

target_compile_definitions(rstest_lib
  PUBLIC
    $<$<BOOL:${RSTEST_MINIMAL_INFO}>:RSTEST_MINIMAL_INFO>
)

# -----------------------------------------------------------------------------
add_subdirectory(example)

########################################################################
# Install rules
# Not intended for installation but for inclusion with a pre-existing
# cmake build environment.
