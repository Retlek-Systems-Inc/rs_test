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
#include "example_test_suite.h"

#include <gmock/gmock.h>

#include <algorithm>
#include <cctype>
#include <initializer_list>
#include <span>
#include <string>

using namespace ::std;
using namespace ::testing;

//-----------------------------------------------------------------------------
namespace
{

constexpr TestCaseState_t AllTCStates[] = {TestCaseState_Idle, TestCaseState_Disabled, TestCaseState_Executing,
                                           TestCaseState_Pass, TestCaseState_Fail};

const char DefaultTestSuiteName[] = "TSName"; ///< Default Test suite name
} // namespace

//-----------------------------------------------------------------------------
// Yes using a different test environment to test RSTest - just to make sure it
// is working properly.
class RSTestLibTest : public Test
{
public:
    /// Constructor
    /// @param[in] testCases contiguous memory of test cases
    /// @param[in] count count of the number of testcases
    explicit RSTestLibTest(TestCase_t *testCases, size_t count)
        : m_testSuite(TestSuite_t{.name           = DefaultTestSuiteName,
                                  .testCases      = testCases,
                                  .count          = count,
                                  .startupCb      = &RSTestLibTest::startupCallback,
                                  .startupCbUser  = this,
                                  .teardownCb     = &RSTestLibTest::teardownCallback,
                                  .teardownCbUser = this,
                                  .failureCb      = &RSTestLibTest::failureCallback,
                                  .failureCbUser  = this})
    {
    }

    /// Default Constructor
    RSTestLibTest()
        : RSTestLibTest(nullptr, 0U)
    {
    }

    /// Destructor
    ~RSTestLibTest() override = default;

protected:
    /// Startup Callback function to send to rstest
    static void startupCallback(void *user)
    {
        auto that = reinterpret_cast<RSTestLibTest *>(user);
        that->m_startupCb.Call();
    }

    /// Teardown Callback function to send to rstest
    static void teardownCallback(void *user)
    {
        auto that = reinterpret_cast<RSTestLibTest *>(user);
        that->m_teardownCb.Call();
    }

    /// Failure Callback function to send to rstest
    static void failureCallback(const AssertRecord_t *record, void *user)
    {
        auto that = reinterpret_cast<RSTestLibTest *>(user);
        that->m_failureCb.Call(record);
    }

    /// Is the initState valid?
    /// @retval true when the init state is valid
    /// @retval false otherwise
    [[nodiscard]] static bool validInitState(const TestCase_t &testCase)
    {
        return (testCase.state == TestCaseState_Idle) || (testCase.state == TestCaseState_Disabled);
    }

    /// Confirm validity of Test Report values.
    void checkReportValidity(const TestReport_t *report, size_t numRuns = 1)
    {
        EXPECT_THAT(report, NotNull());
        EXPECT_THAT(report->name, NotNull());
        EXPECT_THAT(report->name, StrEq(DefaultTestSuiteName));
        EXPECT_THAT(report->date, NotNull()); // Don't care what it is.
        EXPECT_THAT(report->time, NotNull()); // Don't care what it is.
        EXPECT_THAT(report->testCount, Eq(m_testSuite.count * numRuns));
        EXPECT_THAT(report->disabledCount + report->executedCount, Eq(report->testCount));
        EXPECT_THAT(report->passCount + report->failCount, Eq(report->executedCount));
    }

    MockFunction<void()>                             m_startupCb;  ///< Startup Callback Function check
    MockFunction<void()>                             m_teardownCb; ///< Teardown Callback Function check
    MockFunction<void(const AssertRecord_t *record)> m_failureCb;  ///< Failure Callback function check
    TestSuite_t                                      m_testSuite;  ///< TestSuite to use for this test.
};

//----------------------------------------------------------------------------
// Test a TestSuite of size 1 and a parameterized initial state.
class RSTestLibTestInitStateP : public RSTestLibTest, public WithParamInterface<TestCaseState_t>
{
public:
    /// Constructor
    RSTestLibTestInitStateP()
        : RSTestLibTest(&m_testCase, 1)
        , m_testCase(TESTCASE_DEF(RSTC_pass_end, GetParam()))
    {
        // Init with various states
        EXPECT_THAT(rstest_init(&m_testSuite), Eq(validInitState(m_testCase)));
        EXPECT_THAT(rstest_testSuiteCompleted(), IsFalse());
        EXPECT_THAT(rstest_testSuitePassed(), IsFalse());
        EXPECT_THAT(rstest_getReport(), IsNull());
    }

    ~RSTestLibTestInitStateP() override = default; ///< Destructor

protected:
    TestCase_t m_testCase;
};

TEST_P(RSTestLibTestInitStateP, initWithStates) {}

TEST_P(RSTestLibTestInitStateP, runPassing)
{
    if (!validInitState(m_testCase))
    {
        EXPECT_THAT(rstest_run(), IsFalse());
        return;
    }

    // Init passed so should see runs - but execution only if Idle and not disabled.
    // Expectations
    if (m_testCase.state == TestCaseState_Idle)
    {
        InSequence s;
        EXPECT_CALL(m_startupCb, Call());
        EXPECT_CALL(m_teardownCb, Call());
    }

    // Execution
    EXPECT_THAT(rstest_run(), IsTrue());
    EXPECT_THAT(rstest_testSuiteCompleted(), IsTrue());
    EXPECT_THAT(rstest_testSuitePassed(), IsTrue());
    const auto *report = rstest_getReport();
    checkReportValidity(report);
    if (m_testCase.state == TestCaseState_Disabled)
    {
        EXPECT_THAT(report->disabledCount, Eq(1));
        EXPECT_THAT(report->passCount, Eq(0));
        EXPECT_THAT(report->failCount, Eq(0));
        EXPECT_THAT(report->passAsserts.count, Eq(0));
        EXPECT_THAT(report->failAsserts.count, Eq(0));
    }
    else
    {
        EXPECT_THAT(report->disabledCount, Eq(0));
        EXPECT_THAT(report->passCount, Eq(1));
        EXPECT_THAT(report->failCount, Eq(0));
        EXPECT_THAT(report->passAsserts.count, Eq(1));
        EXPECT_THAT(report->failAsserts.count, Eq(0));
    }
}

TEST_P(RSTestLibTestInitStateP, runFailing)
{
    m_testCase = TESTCASE_DEF(RSTC_fail_end, m_testCase.state); // Override with a failing one.
    if (!validInitState(m_testCase))
    {
        EXPECT_THAT(rstest_run(), IsFalse());
        return;
    }

    // Init passed so should see runs - but execution only if Idle and not disabled.
    // Expectations
    if (m_testCase.state == TestCaseState_Idle)
    {
        InSequence s;
        EXPECT_CALL(m_startupCb, Call());
        EXPECT_CALL(m_failureCb, Call(NotNull()));
        EXPECT_CALL(m_teardownCb, Call());
    }

    // Execution
    EXPECT_THAT(rstest_run(), IsTrue());
    EXPECT_THAT(rstest_testSuiteCompleted(), IsTrue());
    const auto *report = rstest_getReport();
    checkReportValidity(report);
    if (m_testCase.state == TestCaseState_Disabled)
    {
        EXPECT_THAT(rstest_testSuitePassed(), IsTrue());
        EXPECT_THAT(report->disabledCount, Eq(1));
        EXPECT_THAT(report->passCount, Eq(0));
        EXPECT_THAT(report->failCount, Eq(0));
        EXPECT_THAT(report->passAsserts.count, Eq(0));
        EXPECT_THAT(report->failAsserts.count, Eq(0));
    }
    else
    {
        EXPECT_THAT(rstest_testSuitePassed(), IsFalse());
        EXPECT_THAT(report->disabledCount, Eq(0));
        EXPECT_THAT(report->passCount, Eq(0));
        EXPECT_THAT(report->failCount, Eq(1));
        EXPECT_THAT(report->passAsserts.count, Eq(0));
        EXPECT_THAT(report->failAsserts.count, Eq(1));
    }
}

INSTANTIATE_TEST_SUITE_P(AllStates, RSTestLibTestInitStateP, ValuesIn(AllTCStates));

//-----------------------------------------------------------------------------

using RSTestLibMultiTest = RSTestLibTest;

TEST_F(RSTestLibMultiTest, allIdle)
{

    TestCase_t RSTC_AllIdle[] = {TESTCASE_DEF(RSTC_pass_end, TestCaseState_Idle),
                                 TESTCASE_DEF(RSTC_fail_end, TestCaseState_Idle),
                                 TESTCASE_DEF(RSTC_pass_assert_pass_end, TestCaseState_Idle),
                                 TESTCASE_DEF(RSTC_pass_assert_fail_end, TestCaseState_Idle),
                                 TESTCASE_DEF(RSTC_fail_assert_pass_end, TestCaseState_Idle),
                                 TESTCASE_DEF(RSTC_fail_assert_fail_end, TestCaseState_Idle)};

    m_testSuite.testCases = &(RSTC_AllIdle[0]);
    m_testSuite.count     = ARRAY_SIZE(RSTC_AllIdle);

    // Expectations
    {
        InSequence s;
        // expect following:
        // Pass End
        EXPECT_CALL(m_startupCb, Call());
        EXPECT_CALL(m_teardownCb, Call());

        // Fail End
        EXPECT_CALL(m_startupCb, Call());
        EXPECT_CALL(m_failureCb, Call(NotNull()));
        EXPECT_CALL(m_teardownCb, Call());

        // Pass Assert Pass End
        EXPECT_CALL(m_startupCb, Call());
        EXPECT_CALL(m_teardownCb, Call());

        // Pass Assert Fail End
        EXPECT_CALL(m_startupCb, Call());
        EXPECT_CALL(m_failureCb, Call(NotNull()));
        EXPECT_CALL(m_teardownCb, Call());

        // Fail Assert Pass End
        EXPECT_CALL(m_startupCb, Call());
        EXPECT_CALL(m_failureCb, Call(NotNull()));
        EXPECT_CALL(m_teardownCb, Call());

        // Fail Assert Fail End
        EXPECT_CALL(m_startupCb, Call());
        EXPECT_CALL(m_failureCb, Call(NotNull()));
        EXPECT_CALL(m_failureCb, Call(NotNull()));
        EXPECT_CALL(m_teardownCb, Call());
    }

    // Execution
    EXPECT_THAT(rstest_init(&m_testSuite), IsTrue());
    EXPECT_THAT(rstest_testSuiteCompleted(), IsFalse());
    EXPECT_THAT(rstest_testSuitePassed(), IsFalse());
    EXPECT_THAT(rstest_getReport(), IsNull());

    EXPECT_THAT(rstest_run(), IsTrue());

    EXPECT_THAT(rstest_testSuiteCompleted(), IsTrue());
    EXPECT_THAT(rstest_testSuitePassed(), IsFalse());
    const auto *report = rstest_getReport();
    checkReportValidity(report);

    EXPECT_THAT(report->disabledCount, Eq(0));
    EXPECT_THAT(report->passCount, Eq(2));
    EXPECT_THAT(report->failCount, Eq(4));
    // Two passing tests and two ASSERT_TRUE, but one failing pass_end not accounted for.
    EXPECT_THAT(report->passAsserts.count, Eq(4));
    EXPECT_THAT(report->failAsserts.count, Eq(5));
}

TEST_F(RSTestLibMultiTest, allDisabled)
{

    TestCase_t RSTC_AllDisabled[] = {TESTCASE_DEF(RSTC_pass_end, TestCaseState_Disabled),
                                     TESTCASE_DEF(RSTC_fail_end, TestCaseState_Disabled),
                                     TESTCASE_DEF(RSTC_pass_assert_pass_end, TestCaseState_Disabled),
                                     TESTCASE_DEF(RSTC_pass_assert_fail_end, TestCaseState_Disabled),
                                     TESTCASE_DEF(RSTC_fail_assert_pass_end, TestCaseState_Disabled),
                                     TESTCASE_DEF(RSTC_fail_assert_fail_end, TestCaseState_Disabled)};

    m_testSuite.testCases = &(RSTC_AllDisabled[0]);
    m_testSuite.count     = ARRAY_SIZE(RSTC_AllDisabled);

    // Expectations
    {
        EXPECT_CALL(m_startupCb, Call()).Times(0);
        EXPECT_CALL(m_failureCb, Call(_)).Times(0);
        EXPECT_CALL(m_teardownCb, Call()).Times(0);
    }

    // Execution
    EXPECT_THAT(rstest_init(&m_testSuite), IsTrue());
    EXPECT_THAT(rstest_testSuiteCompleted(), IsFalse());
    EXPECT_THAT(rstest_testSuitePassed(), IsFalse());
    EXPECT_THAT(rstest_getReport(), IsNull());

    EXPECT_THAT(rstest_run(), IsTrue());

    EXPECT_THAT(rstest_testSuiteCompleted(), IsTrue());
    EXPECT_THAT(rstest_testSuitePassed(), IsTrue());
    const auto *report = rstest_getReport();
    checkReportValidity(report);

    EXPECT_THAT(report->disabledCount, Eq(6));
    EXPECT_THAT(report->passCount, Eq(0));
    EXPECT_THAT(report->failCount, Eq(0));
    EXPECT_THAT(report->passAsserts.count, Eq(0)); // Two passing tests and one ASSERT_TRUE.
    EXPECT_THAT(report->failAsserts.count, Eq(0));
}

TEST_F(RSTestLibMultiTest, onlyPassing)
{

    TestCase_t RSTC_OnlyPassing[] = {TESTCASE_DEF(RSTC_pass_end, TestCaseState_Idle),
                                     TESTCASE_DEF(RSTC_fail_end, TestCaseState_Disabled),
                                     TESTCASE_DEF(RSTC_pass_assert_pass_end, TestCaseState_Idle),
                                     TESTCASE_DEF(RSTC_pass_assert_fail_end, TestCaseState_Disabled),
                                     TESTCASE_DEF(RSTC_fail_assert_pass_end, TestCaseState_Disabled),
                                     TESTCASE_DEF(RSTC_fail_assert_fail_end, TestCaseState_Disabled)};

    m_testSuite.testCases = &(RSTC_OnlyPassing[0]);
    m_testSuite.count     = ARRAY_SIZE(RSTC_OnlyPassing);

    // Expectations
    {
        InSequence s;
        for (const auto &tc : RSTC_OnlyPassing)
        {
            if (tc.state == TestCaseState_Idle)
            {
                // expect following:
                EXPECT_CALL(m_startupCb, Call());
                EXPECT_CALL(m_teardownCb, Call());
            }
        }
    }

    // Execution
    EXPECT_THAT(rstest_init(&m_testSuite), IsTrue());
    EXPECT_THAT(rstest_testSuiteCompleted(), IsFalse());
    EXPECT_THAT(rstest_testSuitePassed(), IsFalse());
    EXPECT_THAT(rstest_getReport(), IsNull());

    EXPECT_THAT(rstest_run(), IsTrue());
    EXPECT_THAT(rstest_testSuiteCompleted(), IsTrue());
    EXPECT_THAT(rstest_testSuitePassed(), IsTrue());
    const auto *report = rstest_getReport();
    checkReportValidity(report);

    EXPECT_THAT(report->disabledCount, Eq(4));
    EXPECT_THAT(report->passCount, Eq(2));
    EXPECT_THAT(report->failCount, Eq(0));
    EXPECT_THAT(report->passAsserts.count, Eq(3)); // Two passing tests and one ASSERT_TRUE.
    EXPECT_THAT(report->failAsserts.count, Eq(0));
}

TEST_F(RSTestLibMultiTest, onlyPassingDoubleRun)
{

    TestCase_t RSTC_OnlyPassing[] = {TESTCASE_DEF(RSTC_pass_end, TestCaseState_Idle),
                                     TESTCASE_DEF(RSTC_fail_end, TestCaseState_Disabled),
                                     TESTCASE_DEF(RSTC_pass_assert_pass_end, TestCaseState_Idle),
                                     TESTCASE_DEF(RSTC_pass_assert_fail_end, TestCaseState_Disabled),
                                     TESTCASE_DEF(RSTC_fail_assert_pass_end, TestCaseState_Disabled),
                                     TESTCASE_DEF(RSTC_fail_assert_fail_end, TestCaseState_Disabled)};

    m_testSuite.testCases = &(RSTC_OnlyPassing[0]);
    m_testSuite.count     = ARRAY_SIZE(RSTC_OnlyPassing);

    // Expectations
    {
        InSequence s;
        for (const auto &tc : RSTC_OnlyPassing)
        {
            if (tc.state == TestCaseState_Idle)
            {
                // expect following:
                EXPECT_CALL(m_startupCb, Call());
                EXPECT_CALL(m_teardownCb, Call());
            }
        }
        // Run 2
        for (const auto &tc : RSTC_OnlyPassing)
        {
            if (tc.state == TestCaseState_Idle)
            {
                // expect following:
                EXPECT_CALL(m_startupCb, Call());
                EXPECT_CALL(m_teardownCb, Call());
            }
        }
    }

    // Execution
    EXPECT_THAT(rstest_init(&m_testSuite), IsTrue());
    EXPECT_THAT(rstest_testSuiteCompleted(), IsFalse());
    EXPECT_THAT(rstest_testSuitePassed(), IsFalse());
    EXPECT_THAT(rstest_getReport(), IsNull());

    EXPECT_THAT(rstest_run(), IsTrue());
    EXPECT_THAT(rstest_testSuiteCompleted(), IsTrue());
    EXPECT_THAT(rstest_testSuitePassed(), IsTrue());
    const auto *report = rstest_getReport();
    checkReportValidity(report);

    EXPECT_THAT(report->disabledCount, Eq(4));
    EXPECT_THAT(report->passCount, Eq(2));
    EXPECT_THAT(report->failCount, Eq(0));
    EXPECT_THAT(report->passAsserts.count, Eq(3)); // Two passing tests and one ASSERT_TRUE.
    EXPECT_THAT(report->failAsserts.count, Eq(0));

    // Second run will double report values.
    EXPECT_THAT(rstest_run(), IsTrue());
    EXPECT_THAT(rstest_testSuiteCompleted(), IsTrue());
    EXPECT_THAT(rstest_testSuitePassed(), IsTrue());
    report = rstest_getReport();
    checkReportValidity(report, 2);

    EXPECT_THAT(report->disabledCount, Eq(8));
    EXPECT_THAT(report->passCount, Eq(4));
    EXPECT_THAT(report->failCount, Eq(0));
    EXPECT_THAT(report->passAsserts.count, Eq(6)); // Two passing tests and one ASSERT_TRUE.
    EXPECT_THAT(report->failAsserts.count, Eq(0));
}
