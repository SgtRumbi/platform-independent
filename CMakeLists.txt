cmake_minimum_required(VERSION 3.0.2)
project(pfind)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES linux_main.cpp)
add_executable(pfind ${SOURCE_FILES})

target_link_libraries(pfind GL X11)
