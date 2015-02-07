cmake_minimum_required(VERSION 2.6)

project(idlmsg_gsim C CXX)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

add_definitions(-Wall)
#add_definitions(-Werror -Wno-deprecated-register -std=c++11)

SET( ${PROJECT_NAME}_MAJOR_VERSION 2 )
SET( ${PROJECT_NAME}_MINOR_VERSION 0 )
SET( ${PROJECT_NAME}_PATCH_LEVEL 1 )
SET( ${PROJECT_NAME}_VERSION
    "${${PROJECT_NAME}_MAJOR_VERSION}.${${PROJECT_NAME}_MINOR_VERSION}.${${PROJECT_NAME}_PATCH_LEVEL}" )

# Release by default
if(NOT DEFINED CMAKE_BUILD_TYPE OR "${CMAKE_BUILD_TYPE}" STREQUAL "")
    set(CMAKE_BUILD_TYPE Release)
endif()

include(pkgconfig)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

if (APPLE)
    set(CMAKE_MACOSX_RPATH ON)
    set(Boost_USE_MULTITHREADED ON)
else()
    set(Boost_USE_MULTITHREADED OFF)
endif()
find_package(Boost 1.45.0 REQUIRED COMPONENTS thread system)

if(NOT Boost_FOUND)
    message(FATAL_ERROR "Boost not found!")
else(NOT Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif(NOT Boost_FOUND)

find_package(Qt4)

if (QT_FOUND)
    set(QT_USE_QTSCRIPT 1)
    set(QT_USE_QTNETWORK 1)
    set(QT_USE_QTSCRIPTTOOLS 1)
    include(${QT_USE_FILE})
endif()

add_subdirectory(gsim)
add_subdirectory(examples)

# Uninstall files
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in" "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake" IMMEDIATE @ONLY)
add_custom_target(uninstall "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")

# TODO install uninstall command

# Opciones de empaquetado
set(CPACK_GENERATOR "TGZ")
set(CPACK_PACKAGE_VERSION_MAJOR ${${PROJECT_NAME}_MAJOR_VERSION})
set(CPACK_PACKAGE_VERSION_MINOR ${${PROJECT_NAME}_MINOR_VERSION})
set(CPACK_PACKAGE_VERSION_PATCH ${${PROJECT_NAME}_PATCH_LEVEL})
set(CPACK_SOURCE_GENERATOR "TGZ")
set(CPACK_PACKAGE_FILE_NAME
    ${PROJECT_NAME}-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}-${CMAKE_SYSTEM_NAME}-${CMAKE_BUILD_TYPE})
include(CPack)

