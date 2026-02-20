cmake_minimum_required(VERSION 3.16)
project(AtariSTEToolkit LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define the core library with both Disk and Filesystem logic
add_library(ste_core STATIC 
    src/libste/disk/DiskHandler.cpp
    src/libste/fs/Fat12Driver.cpp
)

# Set the global include path for the library and all tools
target_include_directories(ste_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Tool 1: Create a blank disk
add_executable(st-mkdisk src/tools/st-mkdisk/main.cpp)
target_link_libraries(st-mkdisk ste_core)

# Tool 2: Verify boot sector checksum
add_executable(st-check src/tools/st-check/main.cpp)
target_link_libraries(st-check ste_core)

# Tool 3: List files on a disk image
add_executable(st-dir src/tools/st-dir/main.cpp)
target_link_libraries(st-dir ste_core)

# Tool 4: Inject a local file into a disk image
add_executable(st-inject src/tools/st-inject/main.cpp)
target_link_libraries(st-inject ste_core)

# Tool 5: Extract a file from a disk image back to Linux
add_executable(st-extract src/tools/st-extract/main.cpp)
target_link_libraries(st-extract ste_core)

add_executable(ste-palette src/tools/ste-palette/main.cpp)
add_executable(st-planar src/tools/st-planar/main.cpp)
add_executable(ste-dma-snd src/tools/ste-dma-snd/main.cpp)
