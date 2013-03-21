
set(gsim_qt_EXPORTED_HEADERS
    initialize.hpp
    types.hpp
    constants.hpp
    json.hpp
    Application.hpp

    item/ModelNode.hpp
    item/MessageModel.hpp
    item/ParameterModel.hpp
    item/ApplicationLogModel.hpp
    item/LogModel.hpp
    item/HolderModel.hpp
    item/OperationsView.hpp
    script/ReflectiveScriptClass.hpp
    script/ScriptEvaluator.hpp
    script/ScriptEngine.hpp

    # Model
    model/Connection.hpp
    model/Connection.ipp
    model/GenericConnection.hpp
    # model/GenericConnection_Data.hpp
    model/Controller.hpp
    model/StatusManager.hpp
    
    # QWidgets
    widgets/TreeView.hpp
    widgets/ConnectionWidget.hpp
    widgets/CustomLayouts.hpp
    widgets/FormWidget.hpp
    widgets/SizeGrip.hpp
    widgets/SortableGroup.hpp
    widgets/StartStopButton.hpp
    widgets/Status.hpp
    widgets/TabWidget.hpp
    widgets/IpAddressLineEdit.hpp
    widgets/MainWindow.hpp
    widgets/ConnectionView.hpp
    
    reflective/path.hpp
    reflective/qvariant.hpp
    reflective/QVariantHelper.hpp
    reflective/ReflectiveWidget.hpp
    reflective/WidgetFactory.hpp
    reflective/BoolWidget.hpp
    reflective/EnumWidget.hpp
    reflective/FloatWidget.hpp
    reflective/IntegerWidget.hpp
    reflective/SequenceWidget.hpp
    reflective/StringWidget.hpp
    reflective/StructWidget.hpp
    reflective/SendMessageDialog.hpp
    reflective/MessageForm.hpp
    reflective/DragAndDrop.hpp
    reflective/ReflectiveItemDelegate.hpp

    reflective/CustomWidgets.hpp
    reflective/LSBWidget.hpp
    reflective/FixedPointWidget.hpp
    reflective/BitSetWidget.hpp
    reflective/IPAddressWidget.hpp
    reflective/BigTextWidget.hpp
    reflective/DateTimeWidget.hpp
    reflective/BCDWidget.hpp
    reflective/HexWidget.hpp
    reflective/TableWidget.hpp
    reflective/ReflectiveTreeWidget.hpp
    
    proc/MessageProcessor.hpp
    proc/MessageProcessorManager.hpp
    proc/HolderEmitter.hpp
    proc/DumpProcessor.hpp
    proc/FileLoaderProcessor.hpp
    
    tools/Engine.hpp
    tools/Sender.hpp
    tools/AbstractTool.hpp
    tools/AbstractInputTool.hpp
    tools/AbstractSequenceTool.hpp
    tools/SenderSequence.hpp
    tools/ValueViewerTool.hpp
    tools/DumpTool.hpp
    tools/ParametersFromFilesTool.hpp
    tools/DetailedLog.hpp
    tools/StatusTool.hpp
)

set(gsim_qt_MOC_HDRS 
    item/ApplicationLogModel.hpp
    item/LogModel.hpp
    item/HolderModel.hpp
    item/MessageModel.hpp
    item/ParameterModel.hpp
    item/OperationsView.hpp
    
    # Model
    model/Connection.hpp
    model/GenericConnection.hpp
    model/GenericConnection_Data.hpp
    model/Controller.hpp
    model/StatusManager.hpp

    # Widgets
    widgets/TreeView.hpp
    widgets/ConnectionWidget.hpp
    widgets/CustomLayouts.hpp
    widgets/FormWidget.hpp
    widgets/SizeGrip.hpp
    widgets/SortableGroup.hpp
    widgets/StartStopButton.hpp
    widgets/Status.hpp
    widgets/TabWidget.hpp
    widgets/IpAddressLineEdit.hpp
    widgets/MainWindow.hpp
    widgets/ConnectionView.hpp
    
    script/ScriptEvaluator.hpp
    script/ScriptEngine.hpp

    # private
    widgets/private/ScriptEditor.hpp
    widgets/private/codeeditor.h

    reflective/ReflectiveWidget.hpp
    reflective/BoolWidget.hpp
    reflective/EnumWidget.hpp
    reflective/FloatWidget.hpp
    reflective/IntegerWidget.hpp
    reflective/SequenceWidget.hpp
    reflective/StringWidget.hpp
    reflective/StructWidget.hpp
    reflective/SendMessageDialog.hpp
    reflective/MessageForm.hpp
    reflective/DragAndDrop.hpp
    reflective/ReflectiveItemDelegate.hpp
    
    reflective/LSBWidget.hpp
    reflective/FixedPointWidget.hpp
    reflective/BitSetWidget.hpp
    reflective/IPAddressWidget.hpp
    reflective/BigTextWidget.hpp
    reflective/DateTimeWidget.hpp
    reflective/BCDWidget.hpp
    reflective/HexWidget.hpp
    reflective/TableWidget.hpp
    reflective/ReflectiveTreeWidget.hpp
    
    proc/MessageProcessor.hpp
    proc/MessageProcessorManager.hpp
    proc/HolderEmitter.hpp
    proc/DumpProcessor.hpp
    proc/FileLoaderProcessor.hpp
    
    tools/Engine.hpp
    tools/Sender.hpp
    tools/AbstractTool.hpp
    tools/AbstractInputTool.hpp
    tools/AbstractSequenceTool.hpp
    tools/SenderSequence.hpp
    tools/ValueViewerTool.hpp
    tools/DumpTool.hpp
    tools/ParametersFromFilesTool.hpp
    tools/DetailedLog.hpp
    tools/StatusTool.hpp
)

set(gsim_qt_SRCS 
    initialize.cpp
    Application.cpp

    # Model
    model/Connection.cpp
    model/GenericConnection.cpp
    model/Controller.cpp
    model/StatusManager.cpp

    # qt
    json.cpp
    item/ModelNode.cpp
    item/LogModel.cpp
    item/ApplicationLogModel.cpp
    item/HolderModel.cpp
    item/MessageModel.cpp
    item/ParameterModel.cpp
    item/OperationsView.cpp
    script/ReflectiveScriptClass.cpp
    script/ScriptEvaluator.cpp
    script/ScriptEngine.cpp

    # Widgets
    widgets/TreeView.cpp
    widgets/ConnectionWidget.cpp
    widgets/CustomLayouts.cpp
    widgets/FormWidget.cpp
    widgets/SizeGrip.cpp
    widgets/SortableGroup.cpp
    widgets/StartStopButton.cpp
    widgets/Status.cpp
    widgets/TabWidget.cpp
    widgets/IpAddressLineEdit.cpp
    widgets/MainWindow.cpp
    widgets/ConnectionView.cpp

    # private
    widgets/private/codeeditor.cpp
    widgets/private/qscriptsyntaxhighlighter.cpp
    widgets/private/ScriptEditor.cpp

    reflective/path.cpp
    reflective/qvariant.cpp
    reflective/QVariantHelper.cpp
    reflective/ReflectiveWidget.cpp
    reflective/WidgetFactory.cpp
    reflective/BoolWidget.cpp
    reflective/EnumWidget.cpp
    reflective/FloatWidget.cpp
    reflective/IntegerWidget.cpp
    reflective/SequenceWidget.cpp
    reflective/StringWidget.cpp
    reflective/StructWidget.cpp
    reflective/SendMessageDialog.cpp
    reflective/MessageForm.cpp
    reflective/DragAndDrop.cpp
    reflective/ReflectiveItemDelegate.cpp

    reflective/LSBWidget.cpp
    reflective/FixedPointWidget.cpp
    reflective/BitSetWidget.cpp
    reflective/IPAddressWidget.cpp
    reflective/BigTextWidget.cpp
    reflective/DateTimeWidget.cpp
    reflective/BCDWidget.cpp
    reflective/HexWidget.cpp
    reflective/TableWidget.cpp
    reflective/ReflectiveTreeWidget.cpp

    proc/MessageProcessor.cpp
    proc/MessageProcessorManager.cpp
    proc/HolderEmitter.cpp
    proc/DumpProcessor.cpp
    proc/FileLoaderProcessor.cpp

    tools/Engine.cpp
    tools/Sender.cpp
    tools/AbstractTool.cpp
    tools/AbstractInputTool.cpp
    tools/AbstractSequenceTool.cpp
    tools/SenderSequence.cpp
    tools/ValueViewerTool.cpp
    tools/DumpTool.cpp
    tools/ParametersFromFilesTool.cpp
    tools/DetailedLog.cpp
    tools/StatusTool.cpp
)

set(gsim_qt_LIBS
    ${QT_LIBRARIES}
    ${Boost_THREAD_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}

    gsim
)

foreach(_header ${gsim_qt_EXPORTED_HEADERS})
    get_filename_component(_header_path ${_header} PATH)
    install(FILES ${_header} DESTINATION include/gsim/qt/${_header_path}) 
endforeach()

# NOTE: QT bug: https://bugreports.qt.nokia.com/browse/QTBUG-22829
qt4_wrap_cpp(gsim_qt_MOC_SRCS ${gsim_qt_MOC_HDRS} 
    OPTIONS -DBOOST_TT_HAS_OPERATOR_HPP_INCLUDED)
qt4_add_resources(gsim_qt_RCC gsim_qt.qrc)

add_library(gsim_qt SHARED 
    ${gsim_qt_SRCS} ${gsim_qt_MOC_SRCS} ${gsim_qt_RCC})
add_library(gsim_qt_static STATIC 
    ${gsim_qt_SRCS} ${gsim_qt_MOC_SRCS})
set_target_properties(gsim_qt PROPERTIES 
    VERSION ${${PROJECT_NAME}_VERSION} 
    SOVERSION ${${PROJECT_NAME}_MAJOR_VERSION})
set_target_properties(gsim_qt_static PROPERTIES OUTPUT_NAME gsim_qt)
target_link_libraries(gsim_qt ${gsim_qt_LIBS})

install(TARGETS gsim_qt DESTINATION lib)
install(TARGETS gsim_qt_static DESTINATION lib)

