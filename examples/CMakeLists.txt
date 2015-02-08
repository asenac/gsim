#set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})
include(GSIMEnv.cmake)

# GSIM examples
set(GSIM_EXAMPLES
    empty
    empty_connection
    empty_connection_udp
    multiconnection
    simple
    simple_custom_widget
    simple_unsupported
    status
)

foreach(example_ ${GSIM_EXAMPLES})

    add_subdirectory(${example_})

    install(DIRECTORY ${example_}
        DESTINATION share/gsim/examples PATTERN ".svn" EXCLUDE)

endforeach(example_)

