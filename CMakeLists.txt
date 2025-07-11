cmake_minimum_required(VERSION 3.5) # CMake install : https://cmake.org/download/
project(MNIC LANGUAGES CXX)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# 改为你的静态库 Qt 安装路径
# set(CMAKE_PREFIX_PATH "d:/Qt/6.6.3/build_static_msvc") 

# 改为你的动态库 Qt 安装路径
set(CMAKE_PREFIX_PATH "D:/Qt/6.6.3/msvc2019_64")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# set(CMAKE_FIND_LIBRARY_SUFFIXES ".lib")#指定寻找静态库
# set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")#指定静态连接
# set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")#指定静态连接

find_package(Qt6 COMPONENTS Widgets REQUIRED) # Qt COMPONENTS
aux_source_directory(./src srcs)

# Specify MSVC UTF-8 encoding   
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

add_executable(${PROJECT_NAME}
    WIN32 # If you need a terminal for debug, please comment this statement 
    ${srcs} 
) 
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets) # Qt5 Shared Library