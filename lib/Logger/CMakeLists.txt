cmake_minimum_required(VERSION 3.28)

include("${CMAKE_SOURCE_DIR}/common-proj-include.in")

get_filename_component(
  LIB_NAME ${CMAKE_CURRENT_LIST_DIR} NAME) # Get the project name from the
                                            # current directory name
project(
  ${LIB_NAME} # Define the project with the extracted name
  VERSION ${project_version} # Use version specified in a variable
  DESCRIPTION ${project_description} # Use description specified in a variable
  HOMEPAGE_URL ${project_homepage} # Use homepage URL specified in a variable
  LANGUAGES C CXX # Specify supported languages
)

message(STATUS "Configuring target '${LIB_NAME}'..")

# Define the library with the name of the library
#
# STATIC - baked into whatever links it, one copy per consumer, no
# install() needed. Right for anything that always ships bundled with
# its consumer.
#
# SHARED (.so) - one copy on disk/in memory, loaded at runtime. Use it
# when multiple independent binaries need the same lib without each
# duplicating it, or it needs patching/upgrading without rebuilding
# everything that links it. Needs install(TARGETS ${LIB_NAME} COMPONENT
# ${LIB_NAME} LIBRARY DESTINATION lib), or CPack has nothing to package
# - plus install(DIRECTORY inc/${LIB_NAME} DESTINATION include) for the
# headers.
#
# MODULE - like SHARED but never linked at build time. For a plugin
# something dlopen()s itself, not a declared dependency of any target.
#
# INTERFACE - header-only, no compiled artifact at all. Nothing to
# link, nothing to install() as a binary - if headers need to ship on
# their own, install(DIRECTORY inc/${LIB_NAME} DESTINATION include)
# covers it, since there's no LIBRARY/ARCHIVE/RUNTIME artifact to place.
add_library(${LIB_NAME} INTERFACE) # STATIC) # SHARED) # MODULE)

set(inc inc/${LIB_NAME})

# Configure the version header file
configure_file("${CMAKE_SOURCE_DIR}/version.h.in"
               "${CMAKE_CURRENT_BINARY_DIR}/inc/${LIB_NAME}/version.h")

# Specify the sources
target_sources(
  ${LIB_NAME}
  INTERFACE "${CMAKE_CURRENT_BINARY_DIR}/inc/${LIB_NAME}/version.h"
  INTERFACE ${inc}/Logger.hpp
  INTERFACE src/Logger.cpp
  # INTERFACE src/.cpp
)

# Specify the include directories
target_include_directories(
  ${LIB_NAME} INTERFACE ${PROJECT_BINARY_DIR}/inc/
                        ${CMAKE_CURRENT_SOURCE_DIR}/inc)

# Specify the link libraries
target_link_libraries(${LIB_NAME} INTERFACE Common)

applyCommonTargetOptions(${LIB_NAME} INTERFACE)

if(BUILD_TESTING)
  add_subdirectory(test)
endif()