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

void RSTC_pass_end(void)
{
    START_TESTCASE();

    END_TESTCASE_PASS();
}

void RSTC_fail_end(void)
{
    START_TESTCASE();

    END_TESTCASE_PASS();
}

void RSTC_pass_assert_pass_end(void)
{
    START_TESTCASE();

    ASSERT_TRUE(true);

    END_TESTCASE_PASS();
}

void RSTC_pass_assert_fail_end(void)
{
    START_TESTCASE();

    ASSERT_TRUE(true);

    END_TESTCASE_FAIL();
}

void RSTC_fail_assert_pass_end(void)
{
    START_TESTCASE();

    ASSERT_TRUE(false);

    END_TESTCASE_PASS();
}

void RSTC_fail_assert_fail_end(void)
{
    START_TESTCASE();

    ASSERT_TRUE(false);

    END_TESTCASE_FAIL();
}
