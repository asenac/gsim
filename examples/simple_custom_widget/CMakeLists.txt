cmake_minimum_required(VERSION 2.6)

project(gsim_example_simple_custom_widget CXX)

if(NOT DEFINED GSIM_INSTALL_PREFIX)      
    set(GSIM_INSTALL_PREFIX /usr/local)
endif()

if(NOT DEFINED GSIM_ENVIRONMENT) 
    set(CMAKE_MODULE_PATH ${GSIM_INSTALL_PREFIX}/share/gsim/cmake)
    include(GSIMEnv)
endif()

qt4_wrap_cpp(simple_custom_widget_MOC_SRCS ForbiddenStringWidget.hpp
    OPTIONS -DBOOST_TT_HAS_OPERATOR_HPP_INCLUDED)

add_executable(simple_custom_widget_ui main.cpp ForbiddenStringWidget.cpp 
    ${simple_custom_widget_MOC_SRCS})
target_link_libraries(simple_custom_widget_ui ${GSIM_REQUIRED_LIBRARIES})

