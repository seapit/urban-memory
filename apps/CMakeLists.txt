cmake_minimum_required(VERSION 3.28)

file(
  GLOB SUBDIRS
  RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/*)

foreach(DIR ${SUBDIRS})
  if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${DIR})
    message(STATUS "Directory: ${DIR}")
    add_subdirectory(${DIR})
    list(APPEND ALL_APPS ${DIR})
  endif()
endforeach()
