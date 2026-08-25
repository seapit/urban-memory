cmake_minimum_required(VERSION 3.28)

# Tests for either mode - branches on the same CHIPS variable as the
# orchestrator, so this always matches whichever mode Database/CMakeLists.txt
# is in.

include("${CMAKE_SOURCE_DIR}/common-proj-include.in")

include(GoogleTest)

if(CHIPS)
  # One test executable per chip, always built, so ${LIB_NAME}-all (and a
  # plain ctest run) proves every chip actually works.
  project(
    ${LIB_NAME}_Test
    VERSION ${project_version}
    DESCRIPTION ${project_description}
    HOMEPAGE_URL ${project_homepage}
    LANGUAGES C CXX
  )

  foreach(_chip ${CHIPS})
    set(_test_name ${LIB_NAME}-${_chip}_Test)

    message(STATUS "Configuring target '${_test_name}' (Unit-Tests)..")

    add_executable(${_test_name})
    add_test(NAME ${_test_name} COMMAND ${_test_name})

    target_sources(${_test_name} PRIVATE test_Database_ChipAware.cpp)

    target_link_libraries(${_test_name} PRIVATE ${LIB_NAME}-${_chip}
                                                GTest::gtest_main -lgcov)

    applyCommonTargetOptions(${_test_name} PRIVATE)

    # TEST_PREFIX so ctest can tell the chips apart - same source file
    # means the same gtest name otherwise.
    gtest_discover_tests(${_test_name} TEST_PREFIX "${_chip}::")

    add_dependencies(${LIB_NAME}-all ${_test_name})
  endforeach()
else()
  # Plain mode - a single ordinary test executable.
  project(
    ${LIB_NAME}_Test
    VERSION ${project_version}
    DESCRIPTION ${project_description}
    HOMEPAGE_URL ${project_homepage}
    LANGUAGES C CXX
  )

  message(STATUS "Configuring target '${LIB_NAME}_Test' (Unit-Tests)..")

  add_executable(${LIB_NAME}_Test)
  add_test(NAME ${LIB_NAME}_Test COMMAND ${LIB_NAME}_Test)

  target_sources(${LIB_NAME}_Test PRIVATE test_Database.cpp)

  target_include_directories(${LIB_NAME}_Test
                             PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../inc)

  target_link_libraries(${LIB_NAME}_Test PRIVATE ${LIB_NAME} GTest::gtest_main
                                                 -lgcov)

  applyCommonTargetOptions(${LIB_NAME}_Test PRIVATE)

  gtest_discover_tests(${LIB_NAME}_Test)
endif()
