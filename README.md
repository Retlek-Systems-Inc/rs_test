# Really Small Test Framework C
## Introduction

This module includes a simple test framework for environments where printf debugging is not an option.

A run of tests consists of a Test Suite which may contain one or more Test Cases.  Each test case is in it's
own function definition, where the function name is the name of the test.

A common Startup, Teardown and Failure callback function may be defined and associated with a TestSuite.
* The Startup function (if defined) is executed before every test case.
* The Teardwon function (if defined) is executed after every test case.
* The Failure function (if defined) is executed only if a failure (ASSERT_TRUE(false) or END_TESTCASE_FAIL()) are detected and executed.


There is a simpler functional execution where no monitoring occurs, and the only means of detecting an error is if the
abort handler, typically and interrupt service routine, is executed.

## Licensing

See [License](License.txt)

## Usage

The intention is this is to be included as-is in a CMake C project and statically linked.
The intent is to hold the following design requirements:

* platform agnostic
* minimal size
* efficient as possible
* use with embedded systems such as arm-none-eabi or other similar cross compilers
* fully tested and confirmation of tests (TODO)
* fully documented (Partial)

## Documentation

TODO(phelter): get documentation running

## Build Status

TODO(phelter): get builds working