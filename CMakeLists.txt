cmake_minimum_required( VERSION 3.9.0 FATAL_ERROR )

project( "iban-cpp" VERSION 0.1.0 LANGUAGES CXX )
message( STATUS "See README.md" )

set( LIB_NAME "iban-cpp" )
set( APP_NAME "iban" )

#
# Tools
#
include(FindPkgConfig)

#
# Build Options
#
if( CMAKE_BUILD_TYPE STREQUAL "" )
    set( CMAKE_BUILD_TYPE "debug" )
endif ( )

#
# Build types
#
set( CMAKE_CONFIGURATION_TYPES debug release)

#
# Configuration
#
set( CMAKE_CXX_STANDARD 17 )
set( INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include" )
set( SOURCE_DIR  "${PROJECT_SOURCE_DIR}/src" )
set( TEST_SOURCE_DIR "${PROJECT_SOURCE_DIR}/test" )

if ( NOT DEFINED CMAKE_INSTALL_LIBDIR )
    set( CMAKE_INSTALL_LIBDIR library )
endif ( )

if ( CMAKE_PREFIX_INITIALIZED_TO_DEFAULT )
    set( CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}/distribution" CACHE PATH "Install path prefix" FORCE )
endif ( )

if( NOT WIN32 )
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -finput-charset=utf-8" )

    if( CMAKE_BUILD_TYPE STREQUAL "debug" )
        message( STATUS "Debug build" )
        set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb -Wall -Wno-deprecated-declarations -O0" )
    endif ( )

    if( CMAKE_BUILD_TYPE STREQUAL "release" )
        message( STATUS "Release build" )
        set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2" )
    endif ( )
endif ( )

set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake" )

#
# Environment
#

#
# Libraries
#
find_package( Catch2 REQUIRED )

# LIB_DYNLOAD = -ldl    --> TODO: Not a package, just add to library list

#
# Sources
#
##
## all *.cpp in 'source', except the 'generated'
##
file ( GLOB_RECURSE SOURCE_FILES "${SOURCE_DIR}/*.cpp" )

message( STATUS "Sources in: ${SOURCE_DIR}")

#
# Compile
#

add_library( ${LIB_NAME} SHARED ${SOURCE_FILES} )
target_include_directories( ${LIB_NAME} PUBLIC ${INCLUDE_DIR} )
set_target_properties( ${LIB_NAME} PROPERTIES VERSION ${PROJECT_VERSION} )

add_executable( ${APP_NAME} "${SOURCE_DIR}/main.cpp")
target_include_directories( ${APP_NAME} PUBLIC ${INCLUDE_DIR} )
target_link_directories( ${APP_NAME} PUBLIC "." )
target_link_libraries( ${APP_NAME} ${LIB_NAME} )

#
# Tests
#
include(CTest)
enable_testing()

# each cpp file is a separate test file
file ( GLOB_RECURSE TEST_SOURCE_FILES "${TEST_SOURCE_DIR}/*.cpp" )

foreach ( TMP_PATH ${TEST_SOURCE_FILES} )
    get_filename_component ( TMP_APP_NAME ${TMP_PATH} NAME_WLE )

    add_executable ( ${TMP_APP_NAME} ${TMP_PATH} )

    target_include_directories( ${TMP_APP_NAME} PUBLIC ${INCLUDE_DIR} ${TEST_SOURCE_DIR} )
    target_link_directories( ${TMP_APP_NAME} PUBLIC "." )
    target_link_libraries( ${TMP_APP_NAME} Catch2::Catch2WithMain ${LIB_NAME} )

    target_compile_options( ${TMP_APP_NAME} PUBLIC "-DUNIT_TEST" )

    add_test ( NAME ${TMP_APP_NAME} COMMAND ${TMP_APP_NAME} )
endforeach ( TMP_PATH )


