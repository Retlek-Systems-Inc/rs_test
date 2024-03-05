# @copyright 2023 Retlek Systems Inc.

#------------------------------------------------------------------------------
add_library(rstest_example_suite STATIC)

target_sources( rstest_example_suite
  PRIVATE
    example_test_suite.c
    example_test_suite.h
)

target_include_directories( rstest_example_suite
  PUBLIC
    .
)

# TODO(phelter): Clean this up later
target_compile_options( rstest_example_suite
  PRIVATE
    $<$<COMPILE_LANG_AND_ID:C,Clang>:-Wno-unsafe-buffer-usage>
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