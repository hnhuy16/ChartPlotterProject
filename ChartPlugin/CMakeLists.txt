# Tắt cảnh báo QTP0001 của Qt 6
qt_policy(SET QTP0001 NEW)

# 1. Khởi tạo đích đến là thư viện Tĩnh (STATIC) để DemoApp có thể link được
qt_add_library(ChartPlugin STATIC)

# 2. Gắn module QML vào thư viện vừa tạo
qt_add_qml_module(ChartPlugin
    URI "ChartPlotter"
    VERSION 1.0
    SOURCES
        ChartPlugin.cpp
        ChartBackend.h
        ChartBackend.cpp
        core/IDataProcessor.h
        core/LttbAlgorithm.h
        core/OfflineDataProcessor.h
        core/OnlineDataProcessor.h
        core/LttbAlgorithm.cpp
        core/OfflineDataProcessor.cpp
        core/OnlineDataProcessor.cpp
)

# 3. Khai báo các thư viện Qt mà Plugin sử dụng
target_link_libraries(ChartPlugin PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
)

# 4. Cấu hình đường dẫn include
target_include_directories(ChartPlugin PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
