/// @copyright 2023 Retlek Systems Inc.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
///
/// @brief Minimal Test Framwork for creating test cases.
//
#pragma once

#include "rstest_framework.h"

/// Test Suite State
/// @startuml Test Suite State
/// [*]        -d-> NotReady
/// NotReady   -d-> Ready : rstest_init()
/// Executing  -d-> Pass      : END_TESTCASE_PASS()
/// Executing  -d-> Fail      : ASSERT_TRUE(false) | END_TESTCASE_FAIL()
/// @enduml 'Test Case State
typedef enum TestSuiteState_e {
    NotReady = 0, ///< Not Initialized
    Ready    = 1, ///< Initialized
    Complete = 2, ///< Completed
} TestSuiteState_t;

/// Test Suite Structure
/// Define an array of testCases
typedef struct TestSuite_s
{
    const char       *file;   ///< File defining the Testsuite
    const char       *date;   ///< Compilation Date
    const char       *time;   ///< Compilation Time
    const char       *name;   ///< TestSuite name
    TestSuiteInitCb_t initCb; ///< Init Callback performed once init complete.

    TestCase_t *testCases;    ///< Array Of test cases
    uint32_t    testCaseSize; ///< Size of Array of test cases

    TestReport_t     report; ///< Report for this test case - only valid once complete
    TestSuiteState_t state;  ///< Test Suite State
} TestSuite_t;
