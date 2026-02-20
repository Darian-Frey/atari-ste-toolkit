cmake_minimum_required(VERSION 3.16)
project(AtariSTEToolkit LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(ste_core STATIC 
    src/libste/disk/DiskHandler.cpp
    src/libste/fs/Fat12Driver.cpp
)

target_include_directories(ste_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(st-mkdisk src/tools/st-mkdisk/main.cpp)
target_link_libraries(st-mkdisk ste_core)

add_executable(st-check src/tools/st-check/main.cpp)
target_link_libraries(st-check ste_core)

add_executable(st-dir src/tools/st-dir/main.cpp)
target_link_libraries(st-dir ste_core)

add_executable(st-inject src/tools/st-inject/main.cpp)
target_link_libraries(st-inject ste_core)
