# Tắt cảnh báo QTP0001
qt_policy(SET QTP0001 NEW)

# Định nghĩa executable cho app
qt_add_executable(DemoApp
    main.cpp
)

# Cấu hình module QML cho DemoApp
qt_add_qml_module(DemoApp
    URI "DemoApp"
    VERSION 1.0
    QML_FILES 
        main.qml
        ChartView.qml
)

# Liên kết DemoApp với Qt và ChartPlugin
target_link_libraries(DemoApp PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    ChartPlugin # Link trực tiếp với target của Plugin
)
