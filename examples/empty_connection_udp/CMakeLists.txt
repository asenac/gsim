cmake_minimum_required(VERSION 2.6)

project(gsim_example_empty_connection_udp CXX)

if(NOT DEFINED GSIM_INSTALL_PREFIX)      
    set(GSIM_INSTALL_PREFIX /usr/local)
endif()

if(NOT DEFINED GSIM_ENVIRONMENT) 
    set(CMAKE_MODULE_PATH ${GSIM_INSTALL_PREFIX}/share/gsim/cmake)
    include(GSIMEnv)
endif()

add_executable(empty_connection_udp_ui main.cpp)
target_link_libraries(empty_connection_udp_ui ${GSIM_REQUIRED_LIBRARIES})

