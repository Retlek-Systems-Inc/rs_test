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
/// @brief Really Small Test Framwork Example

#include <rstest/rstest.h>

#include "example_test_suite.h"

int main(int argc, char **argv)
{

    TestCase_t RSTestCases1[] = {TESTCASE_DEF(RSTC_pass_end, TestCaseState_Idle),
                                 TESTCASE_DEF(RSTC_fail_end, TestCaseState_Idle),
                                 TESTCASE_DEF(RSTC_pass_assert_pass_end, TestCaseState_Idle),
                                 TESTCASE_DEF(RSTC_pass_assert_fail_end, TestCaseState_Idle),
                                 TESTCASE_DEF(RSTC_fail_assert_pass_end, TestCaseState_Idle),
                                 TESTCASE_DEF(RSTC_fail_assert_fail_end, TestCaseState_Idle)};

    TestSuite_t RSTestSuite1 = {
        "RSTC Test Suite 1", RSTestCases1, ARRAY_SIZE(RSTestCases1), NULL, NULL, NULL, NULL, NULL, NULL};

    TestCase_t RSTestCases2[] = {TESTCASE_DEF(RSTC_pass_end, TestCaseState_Disabled),
                                 TESTCASE_DEF(RSTC_fail_end, TestCaseState_Idle),
                                 TESTCASE_DEF(RSTC_pass_assert_pass_end, TestCaseState_Disabled),
                                 TESTCASE_DEF(RSTC_pass_assert_fail_end, TestCaseState_Idle),
                                 TESTCASE_DEF(RSTC_fail_assert_pass_end, TestCaseState_Disabled),
                                 TESTCASE_DEF(RSTC_fail_assert_fail_end, TestCaseState_Idle)};

    TestSuite_t RSTestSuite2 = {
        "RSTC Test Suite 3", RSTestCases2, ARRAY_SIZE(RSTestCases2), NULL, NULL, NULL, NULL, NULL, NULL};

    TestCase_t RSTestCases3[] = {TESTCASE_DEF(RSTC_pass_end, TestCaseState_Idle),
                                 TESTCASE_DEF(RSTC_fail_end, TestCaseState_Disabled),
                                 TESTCASE_DEF(RSTC_pass_assert_pass_end, TestCaseState_Idle),
                                 TESTCASE_DEF(RSTC_pass_assert_fail_end, TestCaseState_Disabled),
                                 TESTCASE_DEF(RSTC_fail_assert_pass_end, TestCaseState_Idle),
                                 TESTCASE_DEF(RSTC_fail_assert_fail_end, TestCaseState_Disabled)};

    TestSuite_t RSTestSuite3 = {
        "RSTC Test Suite 3", RSTestCases3, ARRAY_SIZE(RSTestCases3), NULL, NULL, NULL, NULL, NULL, NULL};

    TestCase_t RSTestCases4[] = {
        TESTCASE_DEF(RSTC_pass_end, TestCaseState_Idle),
        TESTCASE_DEF(RSTC_fail_end, TestCaseState_Disabled),
        TESTCASE_DEF(
            RSTC_pass_assert_pass_end,
            TestCaseState_Pass), // Cannot do this - will fail early because it must be either Idle or Disabled.
        TESTCASE_DEF(RSTC_pass_assert_fail_end, TestCaseState_Disabled),
        TESTCASE_DEF(
            RSTC_fail_assert_pass_end,
            TestCaseState_Fail), // Cannot do this - will fail early because it must be either Idle or Disabled.
        TESTCASE_DEF(RSTC_fail_assert_fail_end, TestCaseState_Disabled)};

    TestSuite_t RSTestSuite4 = {
        "RSTC Test Suite 4", RSTestCases4, ARRAY_SIZE(RSTestCases4), NULL, NULL, NULL, NULL, NULL, NULL};

    const TestReport_t *rpt = NULL;

    (void)argc;
    (void)argv;

    (void)rstest_init(&RSTestSuite1);
    (void)rstest_run();
    (void)rstest_run(); // Able to run multiple times - counters will detect failures across multiple runs.

    rpt = rstest_getReport();
    (void)rpt;

    (void)rstest_init(&RSTestSuite2);
    (void)rstest_run();

    rpt = rstest_getReport();

    (void)rstest_init(&RSTestSuite3);
    (void)rstest_run();

    rpt = rstest_getReport();

    (void)rstest_init(&RSTestSuite4);
    (void)rstest_run();

    rpt = rstest_getReport();
}
