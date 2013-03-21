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

set(GSIM_REQUIRED_SHARED_LIBRARIES
    # Qt
    ${QT_LIBRARIES}
    ${Boost_THREAD_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    # gsim
    ${GSIM_CORE_LIBRARY}
    ${GSIM_QT_LIBRARY})

set(GSIM_REQUIRED_STATIC_LIBRARIES
    # Qt
    ${QT_LIBRARIES}
    ${Boost_THREAD_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
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

