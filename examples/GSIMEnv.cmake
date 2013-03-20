set(GSIM_ENVIRONMENT TRUE)

# gsim
set(GSIM_CORE_LIBRARY 
    gsim)
set(GSIM_QT_LIBRARY 
    gsim_qt)

# gsim static libraries
set(GSIM_CORE_STATIC_LIBRARY 
    gsim_static)
set(GSIM_QT_STATIC_LIBRARY 
    gsim_qt_static)

find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})

find_package(Boost 1.45.0)

if(NOT Boost_FOUND)
    message(FATAL_ERROR "Boost not found!")
else()
    include_directories(${Boost_INCLUDE_DIRS})
endif()

set(GSIM_REQUIRED_SHARED_LIBRARIES
    # Qt
    ${QT_LIBRARIES}
    # gsim
    ${GSIM_CORE_LIBRARY}
    ${GSIM_QT_LIBRARY})

set(GSIM_REQUIRED_STATIC_LIBRARIES
    # Qt
    ${QT_LIBRARIES}
    # gsim
    ${GSIM_CORE_STATIC_LIBRARY}
    ${GSIM_QT_STATIC_LIBRARY})

if(DEFINED GSIM_STATIC_LINKING)
    set(GSIM_REQUIRED_LIBRARIES 
        ${GSIM_REQUIRED_STATIC_LIBRARIES})
else()
    set(GSIM_REQUIRED_LIBRARIES 
        ${GSIM_REQUIRED_SHARED_LIBRARIES})
endif()

