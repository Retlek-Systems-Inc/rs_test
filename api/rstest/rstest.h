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
/// @brief Really Small Test Framwork for creating test cases.
//
#pragma once

#include <stdint.h>
#include <stdarg.h>
#include "rstest/rstest_std_macros.h"

#if defined(__cplusplus)
extern "C"
{
#endif

// ------------------------------------------------------------------
// Defines

/// Maximum number of assertions in the assertion statistics
/// Feel free to change this but this should be sufficient for
/// the tests.
/// If you can afford more, then use a different testing framework.
/// This is not the framework for you.
#define MAX_NUM_ASSERTIONS (128)

    // ------------------------------------------------------------------
    // Type Definitions

    /// TestCase State
    /// @startuml Test Case State
    /// [*]        -d-> Idle
    /// [*]        -d-> Disabled
    /// Idle       -d-> Executing : START_TESTCASE()
    /// Executing  -d-> Pass      : END_TESTCASE_PASS()
    /// Executing  -d-> Fail      : ASSERT_TRUE(false) | END_TESTCASE_FAIL()
    /// @enduml 'Test Case State
    typedef enum TestCaseState_e
    {
        TestCaseState_Idle      = 0, ///< Not Executed (Idle)
        TestCaseState_Disabled  = 1, ///< Disabled
        TestCaseState_Executing = 2, ///< In the process of Executing
        TestCaseState_Pass      = 3, ///< Completed Execution and Passed
        TestCaseState_Fail      = 4  ///< Completed Execution and Failed
    } TestCaseState_t;

    /// Test Case Function Type
    typedef void (*TestCaseFunc_t)(void);

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpadded"
#endif

    /// Test Case Record
    typedef struct TestCase_s
    {
        const char     *name;  ///< TestCase Name
        TestCaseFunc_t  func;  ///< Test Function pointer
        TestCaseState_t state; ///< TestCase state
    } TestCase_t;

    /// Assert Record type
    typedef struct AssertRecord_s
    {
        const char *file; ///< File name
        uint32_t    line; ///< Line number
    } AssertRecord_t;

#if defined(__clang__)
#pragma clang diagnostic pop
#endif

    /// Startup Callback function for test suite.
    typedef void (*TestSuiteStartupCb_t)(void);

    /// Teardown Callback function for test suite.
    typedef void (*TestSuiteTeardownCb_t)(void);

    /// Failure Callback function for test suite.
    /// @param[in] record assertion record type
    typedef void (*TestFailureCb_t)(const AssertRecord_t *record);

    /// Test Suite Structure
    typedef struct TestSuite_s
    {
        const char           *name;       ///< TestSuite Name
        TestCase_t           *testCases;  ///< Array of test cases
        size_t                count;      ///< Count of test cases in testCases array
        TestSuiteStartupCb_t  startupCb;  ///< Callback to be performed before every test case execution
        TestSuiteTeardownCb_t teardownCb; ///< Callback to be performed at the end of every test case execution
        TestFailureCb_t       failureCb;  ///< Callback to be performed if/when a failure occurs
    } TestSuite_t;

    /// Assertion Record List
    typedef struct AssertRecordList_s
    {
        size_t         count;                       ///< Current count of Used assertions
        AssertRecord_t records[MAX_NUM_ASSERTIONS]; ///< Assertion Record Array
    } AssertRecordList_t;

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpadded"
#endif

    /// Test Report
    typedef struct TestReport_s
    {
        const char        *name;          ///< TestSuite name
        const char        *date;          ///< Compilation Date
        const char        *time;          ///< Compilation Time
        uint32_t           testCount;     ///< Total Test cases
        uint32_t           disabledCount; ///< Total Disabled Test cases
        uint32_t           executedCount; ///< Total Executed Test cases
        uint32_t           passCount;     ///< Total Passed Test cases
        uint32_t           failCount;     ///< Total Failed Test cases
        AssertRecordList_t passAsserts;   ///< List of passing assert records
        AssertRecordList_t failAsserts;   ///< List of failing assert recors
    } TestReport_t;

#if defined(__clang__)
#pragma clang diagnostic pop
#endif

// ------------------------------------------------------------------
// Defines helpers to simplify writing test cases
// These are to be performed within a testcase function.

// When MINIMAL_INFO - the first error causes an abort which will cause the
// abort handler (suggest a non return while loop) to execute.
#if defined(RSTEST_MINIMAL_INFO)

/// Start Test Case
/// Add this macro within the testcase function to mark a positive
/// assertion that the testcase in-fact started and change the state
/// of the test to Executing.
#define START_TESTCASE()
/// End Test Case Pass
#define END_TESTCASE_PASS()
/// End Test Case Fail
#define END_TESTCASE_FAIL() abort();

/// Assertion check
/// Confirms that the condition is true and if not, then assert and update test
/// state
/// @param[in] cond condition to check
#define ASSERT_TRUE(cond) \
    if (!(cond))          \
    {                     \
        abort();          \
    }

#else

/// Start Test Case
/// Add this macro within the testcase function to mark a positive
/// assertion that the testcase in-fact started and change the state
/// of the test to Executing.
#define START_TESTCASE() \
    (void)rstest_changeTestCaseState(&(AssertRecord_t){__FILENAME__, __LINE__}, TestCaseState_Executing)
/// End Test Case Pass
#define END_TESTCASE_PASS() \
    (void)rstest_changeTestCaseState(&(AssertRecord_t){__FILENAME__, __LINE__}, TestCaseState_Pass)
/// End Test Case Fail
#define END_TESTCASE_FAIL() \
    (void)rstest_changeTestCaseState(&(AssertRecord_t){__FILENAME__, __LINE__}, TestCaseState_Fail)

/// Assertion check
/// Confirms that the condition is true and if not, then assert and update test
/// state
/// @param[in] cond condition to check
#define ASSERT_TRUE(cond) (void)rstest_assertTrue(&(AssertRecord_t){__FILENAME__, __LINE__}, cond)

#endif // defined(RSTEST_MINIMAL_INFO)

// ------------------------------------------------------------------
// Defines helpers to specify a testcase

/// Test Case Define
/// For defining the static array of testcases within an array of tests
/// @code
///    void TC_Example1(void);
///    void TC_Example2(void);
///    ...
///    TestCase_t k_TestCases[] = {
///         TESTCASE_DEF(TC_Example1, TestCaseState_Idle), //< Active testcase
///         TESTCASE_DEF(TC_Example2, TestCaseState_Disabled), //< Disabled testcase
///    };
/// @endcode
/// @param[in] func Function that defines the testcase
/// @param[in] state initial TestCaseState_t of the testcase, can only be Idle, Disabled
/// @post when the Test suite is defined all tests are checked that they have a proper initial TestCaseState
#define TESTCASE_DEF(func, state) \
    {                             \
        (#func), (func), (state)  \
    }

/// Test Suite Define
/// See definition of rstest_init for parameters.
#define TESTSUITE_DEF(name, initCb, testCases) rstest_init((name), initCb, (testCases), ARRAY_SIZE(testCases))

    // ------------------------------------------------------------------
    // Report API to extract the report location

    /// Query if completed
    /// @retval true when completed
    /// @retval false otherwise
    bool rstest_testSuiteCompleted(void);

    /// Query if all tests passed
    /// @retval true when completed and status is all tests have passed
    /// @retval false otherwise
    bool rstest_testSuitePassed(void);

    /// Get the Report
    /// @returns a pointer to the Test Report.
    const TestReport_t *rstest_getReport(void);

    // ------------------------------------------------------------------
    // Internal functions
    // Not expected to be called (use the macros)

    /// Change the test case state
    /// @param[in] rec assertion record (where the change of test-case state occurred)
    /// @param[in] state state of test case to change to
    /// @pre Following checks are performed before changing state:
    ///     Must be in Idle before transitioning to Executing
    ///     Must be in Executing before transitioning to Pass / Fail
    /// @returns the state of the current testCase after change - if this is Init or Disable
    /// the state was not changed.
    TestCaseState_t rstest_changeTestCaseState(const AssertRecord_t *rec, TestCaseState_t state);

    /// Assert true function
    /// @param[in] rec assertion record (where the assertion takes place)
    /// @param[in] cond condition, when true only the location is tracked,
    ///     when false condition and failure are identified.
    TestCaseState_t rstest_assertTrue(const AssertRecord_t *rec, bool cond);

    /// Initialize the rtest with the TestSuite
    /// @param[in] testSuite the test suite to initalize.
    /// @retval true if initialization worked (correct state and input)
    /// @retval false otherwise
    bool rstest_init(const TestSuite_t *testSuite);

    /// Run the test suite
    /// @retval true if ran
    /// @retval false otherwise
    bool rstest_run(void);

#if defined(__cplusplus)
}
#endif
