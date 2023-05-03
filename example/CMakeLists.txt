# @copyright 2023 Retlek Systems Inc.

if(NOT RSTEST_BUILD_EXAMPLE )
  return()
endif()

#------------------------------------------------------------------------------
add_library(rstest_example_suite STATIC)

target_sources(rstest_example_suite
  PRIVATE
    example_test_suite.c
    example_test_suite.h
)

target_include_directories(rstest_example_suite
  PUBLIC
    .
)

target_link_libraries( rstest_example_suite
  PUBLIC
    RsTest::RsTest
)

#------------------------------------------------------------------------------
add_executable(rstest_example)

target_sources( rstest_example
  PRIVATE
    example_main.c
)

target_link_libraries( rstest_example
  PRIVATE
    rstest_example_suite
    RsTest::RsTest
)

#------------------------------------------------------------------------------
add_subdirectory(test)