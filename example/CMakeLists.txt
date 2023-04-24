# @copyright 2020 Retlek Systems Inc.

add_executable(rstest_example)

target_sources( rstest_example
  PRIVATE
    example_main.c
    example_test_suite.c
    example_test_suite.h
)

target_include_directories(rstest_example
  PRIVATE
    .
)

target_link_libraries( rstest_example
  PRIVATE
    RsTest::RsTest
)
