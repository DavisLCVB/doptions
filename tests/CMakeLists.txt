include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.14.0
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
FetchContent_MakeAvailable(googletest)

# Enable testing
enable_testing()

# Test executable
add_executable(doptions_tests
  option_test.cpp
  validations_test.cpp
  command_test.cpp
  application_test.cpp
  bug_regression_test.cpp
  custom_structures_test.cpp
)

target_link_libraries(doptions_tests
  PRIVATE
    doptions::doptions
    GTest::gtest_main
)

target_include_directories(doptions_tests PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

# Discover tests for CTest
include(GoogleTest)
gtest_discover_tests(doptions_tests)
