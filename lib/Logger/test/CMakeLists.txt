cmake_minimum_required(VERSION 3.28)

message(STATUS "Configuring target '${LIB_NAME}_Test' (Unit-Tests)..")

include("${CMAKE_SOURCE_DIR}/common-proj-include.in")

project(
  ${LIB_NAME}_Test # Define the project with the extracted name
  VERSION ${project_version} # Use version specified in a variable
  DESCRIPTION ${project_description} # Use description specified in a variable
  HOMEPAGE_URL ${project_homepage} # Use homepage URL specified in a variable
  LANGUAGES C CXX # Specify supported languages
)

# Create a test executable
add_executable(${LIB_NAME}_Test)
add_test(NAME ${LIB_NAME}_Test COMMAND ${LIB_NAME}_Test)

# Set the sources for gtest, should just be all files beneath /test (same level
# as this cmake)
target_sources(${LIB_NAME}_Test PRIVATE test_logger.cpp)

# Specify the include directories
target_include_directories(${LIB_NAME}_Test
                           PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../inc)

# Link this to the module it's a part of
target_link_libraries(${LIB_NAME}_Test PRIVATE ${LIB_NAME} GTest::gtest_main
                                               -lgcov)

applyCommonTargetOptions(${LIB_NAME}_Test PRIVATE)

include(GoogleTest)
gtest_discover_tests(${LIB_NAME}_Test)
