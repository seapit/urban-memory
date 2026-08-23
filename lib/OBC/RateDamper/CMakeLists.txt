cmake_minimum_required(VERSION 3.28)
# Every module has this shape: orchestrator + optional per-chip subdirs +
# test/.
# When CHIPS is empty this directory defines the target directly
#
# LIB_NAME set here, test/ needs it too and isn't a child of a chip subdir.
get_filename_component(LIB_NAME ${CMAKE_CURRENT_LIST_DIR} NAME)

# Populate CHIPS for a per-chip implementation, e.g. set(CHIPS ChipA).
# Leave empty for a generic module - this directory becomes the target directly
set(CHIPS)

if(CHIPS)
  # One target per chip, an "-all" target to build them all, and a
  # CHIP-selected ALIAS for whoever doesn't care which chip. inc/ and src/
  # here aren't built as their own target in this mode see ChipA/CMakeLists.txt.
  foreach(_chip ${CHIPS})
    add_subdirectory(${_chip}) # target ${LIB_NAME}-${_chip}
  endforeach()

  # No ALL keyword - only builds when asked for (--target ${LIB_NAME}-all),
  # not as part of a plain default build.
  add_custom_target(${LIB_NAME}-all)
  foreach(_chip ${CHIPS})
    add_dependencies(${LIB_NAME}-all ${LIB_NAME}-${_chip})
  endforeach()

  # Per-module override so independent modules can each resolve their own
  # bare alias in the same build; falls back to the shared CHIP.
  if(DEFINED ${LIB_NAME}_CHIP)
    set(_selected_chip ${${LIB_NAME}_CHIP})
  else()
    set(_selected_chip ${CHIP})
  endif()

  if(_selected_chip AND _selected_chip IN_LIST CHIPS)
    add_library(${LIB_NAME} ALIAS ${LIB_NAME}-${_selected_chip})
    message(STATUS "Module '${LIB_NAME}': plain '${LIB_NAME}' target resolves to CHIP='${_selected_chip}'")
  else()
    message(STATUS "Module '${LIB_NAME}': no specific chip implementation selected. Build one directly (${LIB_NAME}-<chip>, ...), all of them via ${LIB_NAME}-all, or pass -D${LIB_NAME}_CHIP=<chip> (or the shared -DCHIP=<chip>) to make the plain '${LIB_NAME}' target available.")
  endif()
else()
  include("${CMAKE_SOURCE_DIR}/common-proj-include.in")

  project(
    ${LIB_NAME} # Define the project with the extracted name
    VERSION ${project_version} # Use version specified in a variable
    DESCRIPTION ${project_description} # Use description specified in a variable
    HOMEPAGE_URL ${project_homepage} # Use homepage URL specified in a variable
    LANGUAGES C CXX # Specify supported languages
  )

  message(STATUS "Configuring target '${LIB_NAME}'..")

  # No chips needed. Define the target directly. If it
  # ends up depending on another module's CHIP-gated alias, mark that
  # EXCLUDE_FROM_ALL below
  #
  # STATIC - baked into whatever links it, one copy per consumer, no
  # install() needed. Right for anything that always ships bundled with
  # its consumer.
  #
  # SHARED (.so) - one copy on disk/in memory, loaded at runtime. Use it
  # when multiple independent binaries need the same lib without each
  # duplicating it, or it needs patching/upgrading without rebuilding
  # everything that links it. Needs install(TARGETS ${LIB_NAME}
  # COMPONENT ${LIB_NAME} LIBRARY DESTINATION lib), or CPack has nothing
  # to package - plus install(DIRECTORY inc/${LIB_NAME} DESTINATION
  # include) for the headers.
  #
  # MODULE - like SHARED but never linked at build time. For a plugin
  # something dlopen()s itself, not a declared dependency of any target.
  #
  # INTERFACE - header-only, no compiled artifact at all. Nothing to
  # link, nothing to install() as a binary - if headers need to ship on
  # their own, install(DIRECTORY inc/${LIB_NAME} DESTINATION include)
  # covers it, since there's no LIBRARY/ARCHIVE/RUNTIME artifact to place.
  add_library(${LIB_NAME} STATIC) # SHARED) # MODULE) # INTERFACE)

  set(inc inc/${LIB_NAME})

  # Configure the version header file
  configure_file("${CMAKE_SOURCE_DIR}/version.h.in"
                 "${CMAKE_CURRENT_BINARY_DIR}/inc/${LIB_NAME}/version.h")

  target_sources(
    ${LIB_NAME}
    PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/inc/${LIB_NAME}/version.h" ${inc}/RateDamper.hpp
    PRIVATE src/RateDamper.cpp
  )

  target_include_directories(${LIB_NAME} PUBLIC ${PROJECT_BINARY_DIR}/inc/
                                                ${CMAKE_CURRENT_SOURCE_DIR}/inc)

  applyCommonTargetOptions(${LIB_NAME} PRIVATE)
endif()

if(BUILD_TESTING)
  add_subdirectory(test)
endif()
