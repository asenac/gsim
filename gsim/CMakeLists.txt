SET(GSIM_MAJOR_VERSION ${${PROJECT_NAME}_MAJOR_VERSION})
SET(GSIM_MINOR_VERSION ${${PROJECT_NAME}_MINOR_VERSION})
SET(GSIM_PATCH_LEVEL   ${${PROJECT_NAME}_PATCH_LEVEL})
SET(GSIM_VERSION       ${${PROJECT_NAME}_VERSION})

find_package(Qt4)

set(Boost_USE_MULTITHREADED OFF)
find_package(Boost 1.45.0 COMPONENTS system thread)

if(NOT Boost_FOUND)
    message(FATAL_ERROR "Boost not found!")
else(NOT Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif(NOT Boost_FOUND)

# Fichero version.hpp
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/version.hpp.in" 
    "${CMAKE_CURRENT_SOURCE_DIR}/version.hpp" IMMEDIATE @ONLY)

set(EXPORTED_HEADERS
	adapted.hpp
    types.hpp
    version.hpp

    # core
    core/copy.hpp
    core/descriptor_fwd.hpp
    core/descriptor.hpp
    core/descriptor_utils.hpp
    core/event_fwd.hpp
    core/event.hpp
    core/file_format_helper.hpp
    core/holder.hpp
    core/mpl.hpp
    core/tag.hpp
    core/properties.hpp
    core/types_repository.hpp
    core/detail/adapted.hpp
    core/detail/descriptor.hpp
    core/detail/descriptor.ipp
    core/detail/descriptor_explicit.ipp
    core/detail/holder.ipp
    core/detail/types_repository.hpp

    # json
    json/converter.hpp
    json/helper.hpp
    json/parser.hpp
    json/reflective.hpp
    json/writer.hpp
    json/detail/json.hpp
    json/detail/json_parse.hpp
    json/detail/json_writer.hpp
)

set(gsim_SRCS 
    core/descriptor.cpp
    core/event.cpp
    core/file_format_helper.cpp
    core/holder.cpp
    core/properties.cpp
    core/types_repository.cpp
    core/detail/types_repository.cpp

    # json
    json/reflective.cpp
)

set(gsim_LIBS
    ${Boost_THREAD_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
)

foreach(_header ${EXPORTED_HEADERS})
	get_filename_component(_header_path ${_header} PATH)
	install(FILES ${_header} DESTINATION include/gsim/${_header_path}) 
endforeach()

add_library(gsim SHARED 
    ${gsim_SRCS})
add_library(gsim_static STATIC 
    ${gsim_SRCS})
set_target_properties(gsim PROPERTIES 
    VERSION ${${PROJECT_NAME}_VERSION} 
    SOVERSION ${${PROJECT_NAME}_MAJOR_VERSION})
set_target_properties(gsim_static PROPERTIES 
    OUTPUT_NAME gsim)
target_link_libraries(gsim ${gsim_LIBS})

install(TARGETS gsim DESTINATION lib)
install(TARGETS gsim_static DESTINATION lib)

# gsim_io
add_subdirectory(io)

if (QT_FOUND)
    set(QT_USE_QTSCRIPT 1)
    set(QT_USE_QTNETWORK 1)
    set(QT_USE_QTSCRIPTTOOLS 1)
    include(${QT_USE_FILE})

    # gsim_qt
    add_subdirectory(qt)

    # Qwt
    find_package(Qwt6)
    if (Qwt6_FOUND)
        # gsim_qwt
        add_subdirectory(qwt)
    endif()
endif()

add_subdirectory(cmake)
