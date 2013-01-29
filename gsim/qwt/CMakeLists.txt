include_directories(${Qwt6_INCLUDE_DIR})

set(gsim_qwt_EXPORTED_HEADERS
    export.hpp
    SimplePlot.hpp
    PlotModel.hpp
    PlotTool.hpp
)

foreach(_header ${gsim_qwt_EXPORTED_HEADERS})
    get_filename_component(_header_path ${_header} PATH)
    install(FILES ${_header}
        DESTINATION include/gsim/qwt/${_header_path})
endforeach()

set(gsim_qwt_MOC_HDRS
    SimplePlot.hpp
    PlotModel.hpp
    PlotTool.hpp
    private/Plot.hpp
)

set(gsim_qwt_SRCS
    SimplePlot.cpp
    PlotModel.cpp
    PlotTool.cpp
    private/Plot.cpp
    private/HistoricData.cpp
)

set(gsim_qwt_LIBS
    ${QT_LIBRARIES}
    gsim
    ${Qwt6_Qt4_LIBRARY}
)

# NOTE: QT bug: https://bugreports.qt.nokia.com/browse/QTBUG-22829
qt4_wrap_cpp(gsim_qwt_MOC_SRCS ${gsim_qwt_MOC_HDRS} 
    OPTIONS -DBOOST_TT_HAS_OPERATOR_HPP_INCLUDED)

add_library(gsim_qwt SHARED ${gsim_qwt_SRCS}
    ${gsim_qwt_MOC_SRCS}
    ${gsim_qwt_RCC})
add_library(gsim_qwt_static STATIC ${gsim_qwt_SRCS}
    ${gsim_qwt_MOC_SRCS}
    ${gsim_qwt_RCC})

target_link_libraries(gsim_qwt
    ${gsim_qwt_LIBS})

set_target_properties(gsim_qwt PROPERTIES
    VERSION ${${PROJECT_NAME}_VERSION}
    SOVERSION ${${PROJECT_NAME}_MAJOR_VERSION})
set_target_properties(gsim_qwt_static PROPERTIES OUTPUT_NAME gsim_qwt)

install(TARGETS gsim_qwt gsim_qwt_static DESTINATION lib)

