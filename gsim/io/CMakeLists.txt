
set(gsim_io_EXPORTED_HEADERS
    Connection.hpp
    UDPConnection.hpp
    TCPConnection.hpp
)

foreach(_header ${gsim_io_EXPORTED_HEADERS})
    get_filename_component(_header_path ${_header} PATH)
    install(FILES ${_header}
        DESTINATION include/gsim/io/${_header_path})
endforeach()

set(gsim_io_SRCS
    Connection.cpp
    UDPConnection.cpp
    TCPConnection.cpp
)

set(gsim_io_LIBS
    ${Boost_THREAD_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
)

add_library(gsim_io SHARED 
    ${gsim_io_SRCS})
add_library(gsim_io_static STATIC
    ${gsim_io_SRCS})

target_link_libraries(gsim_io
    ${gsim_io_LIBS})

set_target_properties(gsim_io PROPERTIES
    VERSION ${${PROJECT_NAME}_VERSION}
    SOVERSION ${${PROJECT_NAME}_MAJOR_VERSION})
set_target_properties(gsim_io_static PROPERTIES OUTPUT_NAME gsim_io)

install(TARGETS gsim_io gsim_io_static DESTINATION lib)

