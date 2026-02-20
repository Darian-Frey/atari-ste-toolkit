cmake_minimum_required(VERSION 3.16)
project(AtariSTEToolkit LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define the core library
add_library(ste_core STATIC 
    src/libste/disk/DiskHandler.cpp
    src/libste/fs/Fat12Driver.cpp
)

# Use include_directories so ALL executables find the headers automatically
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Toolkit Utilities
add_executable(st-mkdisk src/tools/st-mkdisk/main.cpp)
target_link_libraries(st-mkdisk ste_core)

add_executable(st-check src/tools/st-check/main.cpp)
target_link_libraries(st-check ste_core)

add_executable(st-dir src/tools/st-dir/main.cpp)
target_link_libraries(st-dir ste_core)

add_executable(st-inject src/tools/st-inject/main.cpp)
target_link_libraries(st-inject ste_core)

add_executable(st-extract src/tools/st-extract/main.cpp)
target_link_libraries(st-extract ste_core)

add_executable(ste-palette src/tools/ste-palette/main.cpp)

add_executable(st-planar src/tools/st-planar/main.cpp)

add_executable(ste-dma-snd src/tools/ste-dma-snd/main.cpp)

add_executable(st-bin2rsx src/tools/st-bin2rsx/main.cpp)

add_executable(pi1-to-png src/tools/pi1-to-png/main.cpp)

add_executable(ste-snd-wav src/tools/ste-snd-wav/main.cpp)
add_executable(st-disasm src/tools/st-disasm/main.cpp)
