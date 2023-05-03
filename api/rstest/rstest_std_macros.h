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
/// @brief Really Small Test Framwork defines
/// Defines the Environment for missing features for very minimal compile
/// environments
//
#pragma once

#include <stdint.h>
#include <string.h>

#if defined(__cplusplus)
// Do nothing
#elif defined(HAS_STDBOOL)
#include <stdbool.h>
#else
typedef int bool;
#if !defined(false)
#define false 0
#endif // !defined(false)
#if !defined(true)
#define true 1
#endif // !defined(true)
#endif // defined(__cplusplus)

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

#if defined(__GNUC__) || defined(__clang__)
static const int PATH_DELIMITER = '/';
#else
static const int PATH_DELIMITER = '\\';
#endif

/// Filename - removes path.
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreserved-macro-identifier"
#endif

#define __FILENAME__ ((strrchr(__FILE__, PATH_DELIMITER) != NULL) ? (strrchr(__FILE__, PATH_DELIMITER) + 1) : __FILE__)

#if defined(__clang__)
#pragma clang diagnostic pop
#endif
