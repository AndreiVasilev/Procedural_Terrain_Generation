cmake_minimum_required(VERSION 3.6)
project(Terrain)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# includes cmake/FindSDL2.cmake
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake_modules)
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})

set(SOURCE_FILES src/main.cpp src/Screen.cpp include/Screen.h src/Terrain.cpp include/Terrain.h)
add_executable(Terrain ${SOURCE_FILES})
target_link_libraries(Terrain ${SDL2_LIBRARY})