set(GSIM_MAJOR_VERSION 2)
set(GSIM_MINOR_VERSION 0)
set(GSIM_PATCH_LEVEL   1)
set(GSIM_VERSION       "2.0.1")
if (NOT DEFINED GSIM_INSTALL_PREFIX)
    set(GSIM_INSTALL_PREFIX  "/usr/local")
endif()
set(GSIM_QWT_AVAILABLE )
set(GSIM_ENVIRONMENT TRUE)

set(Qwt6_Qt4_LIBRARY )

# gsim
set(GSIM_CORE_LIBRARY 
    ${GSIM_INSTALL_PREFIX}/lib/libgsim.so.${GSIM_VERSION})
set(GSIM_IO_LIBRARY 
    ${GSIM_INSTALL_PREFIX}/lib/libgsim_io.so.${GSIM_VERSION})
set(GSIM_QT_LIBRARY 
    ${GSIM_INSTALL_PREFIX}/lib/libgsim_qt.so.${GSIM_VERSION})
set(GSIM_QWT_LIBRARY 
    ${GSIM_INSTALL_PREFIX}/lib/libgsim_qwt.so.${GSIM_VERSION})

# gsim static libraries
set(GSIM_CORE_STATIC_LIBRARY 
    ${GSIM_INSTALL_PREFIX}/lib/libgsim.a)
set(GSIM_IO_STATIC_LIBRARY 
    ${GSIM_INSTALL_PREFIX}/lib/libgsim_io.a)
set(GSIM_QT_STATIC_LIBRARY 
    ${GSIM_INSTALL_PREFIX}/lib/libgsim_qt.a)
set(GSIM_QWT_STATIC_LIBRARY 
    ${GSIM_INSTALL_PREFIX}/lib/libgsim_qwt.a)

include_directories(${GSIM_INSTALL_PREFIX}/include)
# For RPATH
link_directories(${GSIM_INSTALL_PREFIX}/lib)

find_package(Qt4 REQUIRED)
set(QT_USE_QTSCRIPT 1)
set(QT_USE_QTNETWORK 1)
set(QT_USE_QTSCRIPTTOOLS 1)
include(${QT_USE_FILE})

set(Boost_USE_MULTITHREADED OFF)
find_package(Boost 1.45.0 COMPONENTS system thread serialization)

if(NOT Boost_FOUND)
    message(FATAL_ERROR "Boost not found!")
else()
    include_directories(${Boost_INCLUDE_DIRS})
endif()

set(Boost_REQUIRED_LIBRARIES
    pthread
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_THREAD_LIBRARY}
    ${Boost_SERIALIZATION_LIBRARY})

set(Boost_REQUIRED_STATIC_LIBRARIES pthread)
if(DEFINED GSIM_STATIC_LINKING)
    set(USE_GSIM_STATIC_LINKING ${GSIM_STATIC_LINKING})
    foreach(lib_ system thread serialization)
        add_library(boost_${lib_} STATIC IMPORTED)
        set_property(TARGET boost_${lib_} PROPERTY
            IMPORTED_LOCATION ${Boost_LIBRARY_DIRS}/libboost_${lib_}.a)
        list(APPEND Boost_REQUIRED_STATIC_LIBRARIES boost_${lib_})
    endforeach()
endif()

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
    # gsim
    ${GSIM_QT_STATIC_LIBRARY}
    ${GSIM_IO_STATIC_LIBRARY}
    ${GSIM_CORE_STATIC_LIBRARY}
    # Boost
    ${Boost_REQUIRED_STATIC_LIBRARIES}
    # Qt
    ${QT_LIBRARIES})

if(GSIM_QWT_AVAILABLE)
    list(APPEND GSIM_REQUIRED_SHARED_LIBRARIES 
        ${GSIM_QWT_LIBRARY} ${Qwt6_Qt4_LIBRARY})
    # list(APPEND GSIM_REQUIRED_STATIC_LIBRARIES ${GSIM_QWT_STATIC_LIBRARY})
endif()

if(DEFINED GSIM_STATIC_LINKING)
    set(GSIM_REQUIRED_LIBRARIES 
        ${GSIM_REQUIRED_STATIC_LIBRARIES})
else()
    set(GSIM_REQUIRED_LIBRARIES 
        ${GSIM_REQUIRED_SHARED_LIBRARIES})
endif()

