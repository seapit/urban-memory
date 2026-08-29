cmake_minimum_required(VERSION 3.28)

include(${CMAKE_CURRENT_LIST_DIR}/Message.cmake)

function(setProjInclude)
  set(CMAKE_PROJECT_INCLUDE_BEFORE "${CMAKE_CURRENT_LIST_DIR}/common-proj-include.in")
endfunction()

function(unsetProjInclude)
  unset(CMAKE_PROJECT_INCLUDE_BEFORE)
endfunction()

# Warnings, debug/release flags, coverage instrumentation - every target
# (lib, app, test) calls this instead of repeating it.
function(applyCommonTargetOptions TARGET_NAME TARGET_SCOPE)
  target_compile_options(${TARGET_NAME} ${TARGET_SCOPE} -Wall -Wextra -Werror)

  if(CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "DEBUG")
    target_compile_options(${TARGET_NAME} ${TARGET_SCOPE} -g -O0)
    target_compile_definitions(${TARGET_NAME} ${TARGET_SCOPE} DEBUG=1)
  else()
    target_compile_options(${TARGET_NAME} ${TARGET_SCOPE} -O2)
  endif()

  if(BUILD_TESTING)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
      target_compile_options(${TARGET_NAME} ${TARGET_SCOPE} --coverage)

      # --coverage at link time, not just -lgcov - that's what actually pulls
      # in the __gcov_init/__gcov_exit constructor glue, not just the raw
      # archive.
      target_link_options(${TARGET_NAME} ${TARGET_SCOPE} --coverage)
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
      target_compile_options(${TARGET_NAME} ${TARGET_SCOPE} -fprofile-instr-generate
                                                            -fcoverage-mapping)
    endif()
  endif()
endfunction()
