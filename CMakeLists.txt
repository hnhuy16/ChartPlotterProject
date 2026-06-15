cmake_minimum_required(VERSION 3.16)
project(ChartPlotterProject VERSION 1.0.0 LANGUAGES CXX)

# Thiết lập chuẩn C++17 (hoặc C++20 nếu cần dùng các tính năng mới)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Tự động chạy MOC, RCC, UIC của Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Tìm các gói Qt6 cơ bản cần thiết cho cả plugin và app
find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick)

# Thêm các thư mục con
add_subdirectory(ChartPlugin)
add_subdirectory(DemoApp)