cmake_minimum_required(VERSION 3.28)

include("${CMAKE_SOURCE_DIR}/common-proj-include.in")

# Get the name of the application directory
get_filename_component(APP_NAME ${CMAKE_CURRENT_LIST_DIR} NAME)

# Define the executable with the name of the application
# Chip-agnostic app, wants "whichever chip was selected" rather than a
# specific ${LIB_NAME}-ChipX - link the plain ALIAS instead, e.g. PRIVATE
# Lib1, resolves via -DCHIP=/-D<Lib>_CHIP=.
#
# Mark this add_executable() EXCLUDE_FROM_ALL too in that case - the alias
# only exists once a chip's actually selected, so a plain default build
# without -DCHIP= would break resolving it.
add_executable(${APP_NAME})

set(inc inc/${APP_NAME})

# Specify the sources
target_sources(${APP_NAME} # PUBLIC inc/Template.hpp
               PRIVATE src/main.cpp)

# Replace ${APP_NAME} with the lib(s) this app actually depends on, e.g.
# PRIVATE Lib1.
target_link_libraries(${APP_NAME} PRIVATE HMS RateDamper Databases HMS Logger)
# target_include_directories(${APP_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/inc)

# To include in a package uncomment below
# Note: To include in a package list that COMPONENT (plus any SHARED libs it links)
# in CPACK_COMPONENTS_ALL for whichever package preset should ship it.
#install(TARGETS ${APP_NAME} COMPONENT ${APP_NAME} RUNTIME DESTINATION bin)


applyCommonTargetOptions(${APP_NAME} PRIVATE)
