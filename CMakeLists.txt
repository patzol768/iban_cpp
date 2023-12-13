cmake_minimum_required( VERSION 3.14.0 FATAL_ERROR )

project( "iban-cpp" VERSION 0.1.0 LANGUAGES CXX C )
message( STATUS "See README.md" )

set( LIB_NAME "iban-cpp" )

#
# Tools
#
include(FindPkgConfig)
include(FetchContent)

#
# Build types
#
set( CMAKE_CONFIGURATION_TYPES Debug Release)

message("Generated with config types: ${CMAKE_CONFIGURATION_TYPES}")

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

pkg_check_modules(LOG4CPLUS log4cplus REQUIRED)

FetchContent_Declare( nlohmann_json GIT_REPOSITORY https://github.com/nlohmann/json.git GIT_TAG v3.11.2 SOURCE_SUBDIR not_exist )
FetchContent_Declare( Catch2 GIT_REPOSITORY https://github.com/catchorg/Catch2.git GIT_TAG v3.4.0 )

FetchContent_MakeAvailable( nlohmann_json )

set( JSON_INCLUDE ${nlohmann_json_SOURCE_DIR}/include )
message( STATUS "nlohmann::json include: ${JSON_INCLUDE}")

#
# Sources
#
##
## all *.cpp in 'source', except the 'generated'
##
file ( GLOB_RECURSE SOURCE_FILES "${SOURCE_DIR}/iban/*.cpp" )

message( STATUS "Sources in: ${SOURCE_DIR}")

#
# Compile
#

add_library( ${LIB_NAME} SHARED ${SOURCE_FILES} )
target_include_directories( ${LIB_NAME} PUBLIC ${INCLUDE_DIR} SYSTEM ${JSON_INCLUDE} )
set_target_properties( ${LIB_NAME} PROPERTIES VERSION ${PROJECT_VERSION} )

#
# Tests
#
set_property(GLOBAL PROPERTY CTEST_TARGETS_ADDED 1)

include(CTest)
enable_testing()

FetchContent_MakeAvailable( Catch2 )

set( CATCH2_INCLUDE ${Catch2_SOURCE_DIR}/src )
message( STATUS "Catch2 include: ${CATCH2_INCLUDE}")

# each cpp file is a separate test file
file ( GLOB_RECURSE TEST_SOURCE_FILES "${TEST_SOURCE_DIR}/*.cpp" )

foreach ( TMP_PATH ${TEST_SOURCE_FILES} )
    get_filename_component ( TMP_APP_NAME ${TMP_PATH} NAME_WLE )

    add_executable ( ${TMP_APP_NAME} ${TMP_PATH} )

    target_include_directories( ${TMP_APP_NAME} PUBLIC ${INCLUDE_DIR} ${TEST_SOURCE_DIR} ${LOG4CPLUS_INCLUDE_DIRS} )
    target_link_directories( ${TMP_APP_NAME} PUBLIC ${LOG4CPLUS_LIBRARY_DIRS} )
    target_link_libraries( ${TMP_APP_NAME} Catch2::Catch2 ${LIB_NAME} ${LOG4CPLUS_LIBRARIES} )

    target_compile_options( ${TMP_APP_NAME} PUBLIC "-DUNIT_TEST" )

    add_test ( NAME ${TMP_APP_NAME} COMMAND ${TMP_APP_NAME} )

    set_property(TEST ${TMP_APP_NAME} PROPERTY ENVIRONMENT "LOG4CPLUS_CONFIG=../log.ini;IBAN_BANK_REGISTRY=../registry/bank_registry/;IBAN_REGISTRY=../registry/iban_registry/")
endforeach ( TMP_PATH )

# each c file is a separate test file
file ( GLOB_RECURSE TEST_C_SOURCE_FILES "${TEST_SOURCE_DIR}/*.c" )

foreach ( TMP_PATH ${TEST_C_SOURCE_FILES} )
    get_filename_component ( TMP_APP_NAME ${TMP_PATH} NAME_WLE )

    add_executable ( ${TMP_APP_NAME} ${TMP_PATH} )

    target_include_directories( ${TMP_APP_NAME} PUBLIC ${INCLUDE_DIR} ${TEST_SOURCE_DIR} )
    target_link_libraries( ${TMP_APP_NAME} ${LIB_NAME} ${LOG4CPLUS_LIBRARIES} )

    target_compile_options( ${TMP_APP_NAME} PUBLIC "-DUNIT_TEST" )

    add_test ( NAME ${TMP_APP_NAME} COMMAND ${TMP_APP_NAME} )

    set_property(TEST ${TMP_APP_NAME} PROPERTY ENVIRONMENT "LOG4CPLUS_CONFIG=../log.ini;IBAN_BANK_REGISTRY=../registry/bank_registry/;IBAN_REGISTRY=../registry/iban_registry/")
endforeach ( TMP_PATH )

