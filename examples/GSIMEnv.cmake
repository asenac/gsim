set(GSIM_ENVIRONMENT TRUE)

# gsim
set(GSIM_CORE_LIBRARY 
    gsim)
set(GSIM_IO_LIBRARY 
    gsim_io)
set(GSIM_QT_LIBRARY 
    gsim_qt)

# gsim static libraries
set(GSIM_CORE_STATIC_LIBRARY 
    gsim_static)
set(GSIM_IO_STATIC_LIBRARY 
    gsim_io_static)
set(GSIM_QT_STATIC_LIBRARY 
    gsim_qt_static)

find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})

set(Boost_USE_MULTITHREADED OFF)
find_package(Boost 1.45.0 COMPONENTS system thread serialization)

if(NOT Boost_FOUND)
    message(FATAL_ERROR "Boost not found!")
else()
    include_directories(${Boost_INCLUDE_DIRS})
endif()

set(Boost_REQUIRED_LIBRARIES
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_THREAD_LIBRARY}
    ${Boost_SERIALIZATION_LIBRARY})

set(Boost_REQUIRED_STATIC_LIBRARIES)
foreach(lib_ system thread serialization)
    add_library(boost_${lib_} STATIC IMPORTED)
    set_property(TARGET boost_${lib_} PROPERTY
        IMPORTED_LOCATION ${Boost_LIBRARY_DIRS}/libboost_${lib_}.a)
    list(APPEND Boost_REQUIRED_STATIC_LIBRARIES 
        ${Boost_LIBRARY_DIRS}/libboost_${lib_}.a)
endforeach()

set(GSIM_REQUIRED_SHARED_LIBRARIES
    # Qt
    ${QT_LIBRARIES}
    # Boost
    ${Boost_REQUIRED_LIBRARIES}
    # gsim
    ${GSIM_CORE_LIBRARY}
    ${GSIM_IO_LIBRARY}
    ${GSIM_QT_LIBRARY})

set(GSIM_REQUIRED_STATIC_LIBRARIES
    # Qt
    ${QT_LIBRARIES}
    # Boost
    ${Boost_REQUIRED_STATIC_LIBRARIES}
    # gsim
    ${GSIM_CORE_STATIC_LIBRARY}
    ${GSIM_IO_STATIC_LIBRARY}
    ${GSIM_QT_STATIC_LIBRARY})

if(DEFINED GSIM_STATIC_LINKING)
    set(GSIM_REQUIRED_LIBRARIES 
        ${GSIM_REQUIRED_STATIC_LIBRARIES})
else()
    set(GSIM_REQUIRED_LIBRARIES 
        ${GSIM_REQUIRED_SHARED_LIBRARIES})
endif()

