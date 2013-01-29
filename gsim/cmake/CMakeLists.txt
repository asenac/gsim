
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/GSIMEnv.cmake.in
    ${CMAKE_CURRENT_SOURCE_DIR}/GSIMEnv.cmake
    @ONLY
)

install(FILES GSIMEnv.cmake 
    DESTINATION share/gsim/cmake)

