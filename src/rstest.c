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

#include "rstest/rstest.h"

#include <assert.h>

// ------------------------------------------------------------------
// Local Types

/// Test Suite State
/// @startuml Test Suite State
/// [*]        -d-> NotReady
/// NotReady   -d-> Ready     : rstest_init()
/// Ready      -d-> Running   : rstest_run() - entry
/// Running    -d-> Complete  : rsttest_run() - exit
/// Complete     -> Ready     : rstest_init()
/// @enduml 'Test Case State
typedef enum TestSuiteState_e
{
    TestSuiteState_NotReady = 0, ///< Not Initialized
    TestSuiteState_Ready    = 1, ///< Initialized
    TestSuiteState_Running  = 2, ///< Running
    TestSuiteState_Complete = 3  ///< Completed
} TestSuiteState_t;

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpadded"
#endif

/// Test Suite info Structure
typedef struct TestInfo_s
{
    const TestSuite_t *testSuite; ///< Test Suite information.
    TestCase_t        *current;   ///< Current TestCase
    TestReport_t       report;    ///< Report for this test case - only valid once complete
    TestSuiteState_t   state;     ///< Test Suite State
} TestInfo_t;

#if defined(__clang__)
#pragma clang diagnostic pop
#endif

// ------------------------------------------------------------------
// Local Static Variables

// Test Suite State Control Block (Singleton)
static TestInfo_t k_info = {0};

// ------------------------------------------------------------------
// Local Functions

/// Add an assertion record to the appropriate list.
/// If it is a failure change the state of the current test case.
static void addAssertion(const AssertRecord_t *rec, bool cond)
{
    AssertRecordList_t *recordList = (cond) ? &(k_info.report.passAsserts) : &(k_info.report.failAsserts);
    if (recordList->count < MAX_NUM_ASSERTIONS)
    {
        recordList->count++;
        recordList->records[recordList->count] = *rec;
    }

    assert(k_info.state == TestSuiteState_Running);
    if (!cond)
    {
        assert(k_info.current != NULL);
        k_info.current->state = TestCaseState_Fail;

        TestFailureCb_t failure = k_info.testSuite->failureCb;
        if (failure != NULL)
        {
            failure(rec, k_info.testSuite->failureCbUser);
        }
    }
}

// ------------------------------------------------------------------
// Report API

bool rstest_testSuiteCompleted(void) { return (k_info.state == TestSuiteState_Complete); }

bool rstest_testSuitePassed(void)
{
    // Note some of these are redundant but better to confirm state of report is correct
    return (rstest_testSuiteCompleted() && (k_info.report.testCount != 0) &&
            (k_info.report.testCount == (k_info.report.disabledCount + k_info.report.executedCount)) &&
            (k_info.report.executedCount == k_info.report.passCount) && (k_info.report.failCount == 0));
}

const TestReport_t *rstest_getReport(void)
{
    if (k_info.state != TestSuiteState_Complete)
    {
        return NULL;
    }
    return &(k_info.report);
}

// ------------------------------------------------------------------
// Internal API - used by Macros

TestCaseState_t rstest_changeTestCaseState(const AssertRecord_t *rec, TestCaseState_t state)
{
    // Not initialized
    if (k_info.state == TestSuiteState_NotReady)
    {
        return TestCaseState_Disabled;
    }

    // Confirm correct state change.
    assert(k_info.current != NULL);
    switch (k_info.current->state)
    {
    case TestCaseState_Idle:
    {
        if (state != TestCaseState_Executing)
        {
            // Invalid state transition - use of Macros is incorrect.
            addAssertion(rec, false);
            return TestCaseState_Disabled;
        }
        break;
    }
    case TestCaseState_Disabled:
    {
        return TestCaseState_Disabled;
    }
    case TestCaseState_Executing:
    case TestCaseState_Pass:
    case TestCaseState_Fail:
    {
        if ((state == TestCaseState_Idle) || (state == TestCaseState_Disabled))
        {
            // Invalid state transition - not using Macros
            addAssertion(rec, false);
            return TestCaseState_Disabled;
        }
        break;
    }
    default:
    {
        // Unknown state - not using Macros
        addAssertion(rec, false);
        return TestCaseState_Disabled;
    }
    }

    // Correct transition and can change state.
    // When Ending - always add an assertion of passing or failing.
    if (state == TestCaseState_Fail)
    {
        addAssertion(rec, false);
        k_info.current->state = state;
    }
    // Can only pass if current state is executing.
    else if ((state == TestCaseState_Pass) && (k_info.current->state == TestCaseState_Executing))
    {
        addAssertion(rec, true);
        k_info.current->state = state;
    }
    return state;
}

TestCaseState_t rstest_assertTrue(const AssertRecord_t *rec, bool cond)
{
    // Not initialized
    if (k_info.state == TestSuiteState_NotReady)
    {
        return TestCaseState_Disabled;
    }
    if (k_info.state != TestSuiteState_Running)
    {
        assert(k_info.current != NULL);
        return k_info.current->state;
    }
    addAssertion(rec, cond);
    return k_info.current->state;
}

bool rstest_init(const TestSuite_t *testSuite)
{
    k_info.testSuite   = testSuite;
    k_info.report      = (TestReport_t){0}; // Clear out the report
    k_info.report.name = testSuite->name;

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdate-time"
#endif

    k_info.report.date = __DATE__;
    k_info.report.time = __TIME__;

    assert(k_info.testSuite->testCases != NULL);
    const TestCase_t *begin = k_info.testSuite->testCases;
    const TestCase_t *end   = begin + k_info.testSuite->count;

    for (const TestCase_t *current = begin; current < end; current++)
    {
        if ((current->state != TestCaseState_Idle) && (current->state != TestCaseState_Disabled))
        {
            k_info.state = TestSuiteState_NotReady;
            return false;
        }
    }
#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#elif defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-qual"
#endif
    k_info.current = (TestCase_t *)begin;
#if defined(__GNUC__)
#pragma GCC diagnostic pop
#elif defined(__clang__)
#pragma clang diagnostic pop
#endif
    k_info.state = TestSuiteState_Ready;
    return true;
}

bool rstest_run(void)
{

    if (k_info.state == TestSuiteState_NotReady)
    {
        return false;
    }

    // Init takes care of clearing the report info.
    // Additive -to account for re-running suite multiple times.
    k_info.report.testCount += k_info.testSuite->count;
    k_info.state = TestSuiteState_Running;
    assert(k_info.testSuite->testCases != NULL);
    const TestCase_t *begin = k_info.testSuite->testCases;
    const TestCase_t *end   = begin + k_info.testSuite->count;

    // Allow re-run.
#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#elif defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-qual"
#endif
    for (k_info.current = (TestCase_t *)begin; k_info.current < end; k_info.current++)
    {
        if (k_info.current->state == TestCaseState_Disabled)
        {
            k_info.report.disabledCount++;
            continue;
        }

        // Execute - and func() changes the state but if still in executing and
        // hasn't changed to Pass, then this is considered a fail.
        k_info.current->state        = TestCaseState_Executing;
        TestSuiteStartupCb_t startup = k_info.testSuite->startupCb;
        if (startup != NULL)
        {
            startup(k_info.testSuite->startupCbUser);
        }

        k_info.current->func();

        TestSuiteTeardownCb_t teardown = k_info.testSuite->teardownCb;
        if (teardown != NULL)
        {
            teardown(k_info.testSuite->teardownCbUser);
        }

        // Update report info
        k_info.report.executedCount++;
        if (k_info.current->state == TestCaseState_Pass)
        {
            k_info.report.passCount++;
        }
        else
        {
            k_info.report.failCount++;
        }
    }
#if defined(__GNUC__)
#pragma GCC diagnostic pop
#elif defined(__clang__)
#pragma clang diagnostic pop
#endif

    k_info.state = TestSuiteState_Complete;
    return true;
}
