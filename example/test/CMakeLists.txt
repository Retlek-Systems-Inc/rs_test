# @copyright 2023 Retlek Systems Inc.

TestTarget (
  TARGET rstest_example_suite
  TYPE   UNIT
  FRAMEWORK GMock
  SOURCES
    test_example_test_suite.cpp
  LINK_LIBRARY
    RsTest::RsTest
)

if(TARGET UnitTest_rstest_example_suite)

target_compile_options(UnitTest_rstest_example_suite
  PRIVATE
    $<$<COMPILE_LANG_AND_ID:CXX,Clang>:-Wno-padded>
  )
endif()