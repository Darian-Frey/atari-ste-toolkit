cmake_minimum_required(VERSION 3.16)
project(AtariSTEToolkit LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(include)

# Define the core library
add_library(ste_core STATIC 
    src/libste/disk/DiskHandler.cpp
)

# Define the st-mkdisk tool
add_executable(st-mkdisk src/tools/st-mkdisk/main.cpp)
target_link_libraries(st-mkdisk ste_core)
