cmake_minimum_required(VERSION 3.28)

set(CMAKE_CXX_SCAN_FOR_MODULES
    OFF
    CACHE BOOL "" FORCE)

# Include functions
include(${CMAKE_SOURCE_DIR}/.cmakefiles/Functions.cmake)

# Project-specific pre-includes set(CMAKE_PROJECT_INCLUDE_BEFORE
# "${CMAKE_CURRENT_LIST_DIR}/common-proj-include.in") The above doesn't work, so
# I'll include it directly and hope it propagates.
include("${CMAKE_SOURCE_DIR}/common-proj-include.in")

# Project Name and details get_filename_component(CURRENT_DIR
# ${CMAKE_BINARY_DIR} ABSOLUTE) # (Commented out) Get the current directory
# (absolute path)
get_filename_component(
  REPO_NAME ${CMAKE_CURRENT_LIST_DIR} NAME) # Get the project name from the
                                            # current directory name
project(
  ${REPO_NAME} # Define the project with the extracted name
  VERSION ${project_version} # Use version specified in a variable
  DESCRIPTION ${project_description} # Use description specified in a variable
  HOMEPAGE_URL ${project_homepage} # Use homepage URL specified in a variable
  LANGUAGES C CXX # Specify supported languages
)

# Set C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Export compilation commands for SonarLint
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Enable color diagnostics in output
set(CMAKE_COLOR_DIAGNOSTICS ON)

# Handle different build types
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE
      "DEBUG"
      CACHE STRING "Choose the type of build" FORCE)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "DEBUG" OR CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_compile_definitions(DEBUG=1)
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "C++ compiler ID: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "C++ compiler version: ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "CMAKE_CXX_SCAN_FOR_MODULES = ${CMAKE_CXX_SCAN_FOR_MODULES}")

# Enable unit tests if BUILD_TESTING is set
if(BUILD_TESTING)
  message(STATUS "Enabling Unit Tests")

  # Include CTest for testing support
  include(CTest)

  # Add Google Test using FetchContent
  include(FetchContent)
  FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.16.0 # Specify the tag/version of Google Test to fetch
    FIND_PACKAGE_ARGS NAMES GTest googletest)
  unset(CMAKE_PROJECT_INCLUDE_BEFORE)
  FetchContent_MakeAvailable(googletest)
  set(CMAKE_PROJECT_INCLUDE_BEFORE
      "${CMAKE_CURRENT_LIST_DIR}/common-proj-include.in")
endif()

# Add subdirectories for libs and apps
add_subdirectory(lib)
add_subdirectory(apps)

# CPack picks up PROJECT_NAME/PROJECT_VERSION from the project() call above.
# No install() rules exist yet, so packages will be near-empty until targets
# opt in with install(TARGETS ...).
include(CPack)
