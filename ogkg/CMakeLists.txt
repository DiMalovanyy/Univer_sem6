cmake_minimum_required(VERSION 3.5)
project(OGTG_Lab LANGUAGES C CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

set(target_name var_ao2)
find_package(wxWidgets REQUIRED)
include(${wxWidgets_USE_FILE})

set(SOURCES
		solution.cpp
		compute.cpp)


add_executable(${target_name} ${SOURCES})

target_link_libraries(${target_name} ${wxWidgets_LIBRARIES})

