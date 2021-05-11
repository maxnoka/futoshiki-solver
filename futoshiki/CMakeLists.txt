# Generated on 2021-5-11 22:15:43 by version 0.9.0 of mulle-xcode-to-cmake
# Command line:
#    mulle-xcode-to-cmake -l c++ -t futoshiki export futoshiki.xcodeproj


cmake_minimum_required (VERSION 3.4)
project( futoshiki CXX)

# ###
# Build for mulle-objc (https://mulle-objc.github.io/):
#
#    mulle-sde init -m foundation/objc-porter executable
#    mulle-sde craft
#
# [mulle-clang >= 8.0.0.0 and mulle-objc >= 0.15 required]
#

if( NOT DEPENDENCY_DIR)
   set( DEPENDENCY_DIR "$ENV{DEPENDENCY_DIR}")
endif()
if( NOT DEPENDENCY_DIR)
   set( DEPENDENCY_DIR "${PROJECT_SOURCE_DIR}/dependency")
endif()
if( EXISTS "${DEPENDENCY_DIR}")
   if( NOT ADDICTION_DIR)
      get_filename_component( ADDICTION_DIR "${DEPENDENCY_DIR}" DIRECTORY)
      set( ADDICTION_DIR "${ADDICTION_DIR}/addiction")
   endif()
   set( CMAKE_FIND_FRAMEWORK "LAST")
   include_directories( BEFORE SYSTEM
      ${DEPENDENCY_DIR}/include
      ${ADDICTION_DIR}/include
   )
   set( CMAKE_FRAMEWORK_PATH
      ${DEPENDENCY_DIR}/Frameworks
      ${ADDICTION_DIR}/Frameworks
      ${CMAKE_FRAMEWORK_PATH}
   )
   set( CMAKE_LIBRARY_PATH
      ${DEPENDENCY_DIR}/lib
      ${ADDICTION_DIR}/lib
      ${CMAKE_LIBRARY_PATH}
   )
   set( SKIP_FIND_FOUNDATION_LIBRARY ON)
   execute_process( COMMAND mulle-sde stash-dir
                    OUTPUT_VARIABLE MULLE_SOURCETREE_STASH_DIR)
   string( STRIP "${MULLE_SOURCETREE_STASH_DIR}" MULLE_SOURCETREE_STASH_DIR)
   execute_process( COMMAND mulle-sde linkorder --stash-dir "${MULLE_SOURCETREE_STASH_DIR}" --startup --simplify --output-format cmake
                    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
                    OUTPUT_VARIABLE MULLE_SDE_LINKER_FLAGS
                    RESULT_VARIABLE RVAL)
   if( NOT ${RVAL} EQUAL 0)
      message( FATAL_ERROR "Failed to procure linkorder from mulle-sde. Old version ?")
   endif()
   message( STATUS "MULLE_SDE_LINKER_FLAGS=\"${MULLE_SDE_LINKER_FLAGS}\"")
else()
   message( STATUS "Not a mulle-sde build (\"${DEPENDENCY_DIR}\" does not exist)")
endif()


#
# Platform specific definitions
#

if( APPLE)
   # # CMAKE_OSX_SYSROOT must be set for CMAKE_OSX_DEPLOYMENT_TARGET (cmake bug)
   # if( NOT CMAKE_OSX_SYSROOT)
   #    set( CMAKE_OSX_SYSROOT "/" CACHE STRING "SDK for OSX" FORCE)   # means current OS X
   # endif()
   #
   # # baseline set to 10.6 for rpath
   # if( NOT CMAKE_OSX_DEPLOYMENT_TARGET)
   #   set(CMAKE_OSX_DEPLOYMENT_TARGET "10.6" CACHE STRING "Deployment target for OSX" FORCE)
   # endif()

   set( CMAKE_POSITION_INDEPENDENT_CODE FALSE)
   set( CMAKE_INSTALL_RPATH "@rpath/../lib")

   set( BEGIN_ALL_LOAD "-all_load")
   set( END_ALL_LOAD)
else()
   set( CMAKE_POSITION_INDEPENDENT_CODE TRUE)

   if( WIN32)
   # windows
   else()
   # linux / gcc
      set( BEGIN_ALL_LOAD "-Wl,--whole-archive")
      set( END_ALL_LOAD "-Wl,--no-whole-archive")
      set( CMAKE_INSTALL_RPATH "\$ORIGIN/../lib")
   endif()
endif()


##
## futoshiki Files
##

# uncomment this for mulle-objc to search libraries first
# set( CMAKE_FIND_FRAMEWORK "LAST")

set( INCLUDE_DIRS
futoshiki-lib
)

set( PUBLIC_HEADERS
)

set( PROJECT_HEADERS
futoshiki-lib/include
)

set( PRIVATE_HEADERS
)

set( SOURCES
futoshiki-lib/src/Cell.cpp
futoshiki-lib/src/Constraint.cpp
futoshiki-lib/src/ConstraintSatisfactionProblem.cpp
futoshiki-lib/src/CspBuilder.cpp
futoshiki-lib/src/CspSolver.cpp
futoshiki-lib/src/EqualityConstraint.cpp
futoshiki-lib/src/Futoshiki.cpp
futoshiki-lib/src/InequalityConstraint.cpp
futoshiki-lib/src/LatinSquare.cpp
futoshiki-lib/src/SquareCsp.cpp
futoshiki-lib/src/TwoDimCsp.cpp
futoshiki-lib/src/utils/easylogging++.cc
futoshiki-lib/src/utils/utils.cpp
)

set( STATIC_DEPENDENCIES
)

set( DEPENDENCIES
)


##
## futoshiki
##

add_library( futoshiki STATIC
${SOURCES}
${PUBLIC_HEADERS}
${PROJECT_HEADERS}
${PRIVATE_HEADERS}
${RESOURCES}
)

target_include_directories( futoshiki
   PUBLIC
futoshiki-lib
)

install( TARGETS futoshiki DESTINATION "lib")
install( FILES ${PUBLIC_HEADERS} DESTINATION "include/futoshiki")
