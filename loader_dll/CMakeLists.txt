# CMakeList.txt : CMake project for cpp_test_test2, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.18)
project(SimpleSwitcherDll VERSION 1.0 LANGUAGES CXX)

file(GLOB src_exe src/main_exe.cpp )
file(GLOB src_dll src/main_dll.cpp )

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

add_executable(loader WIN32 ${src_exe})
add_library(SimpleSwitcherDll SHARED ${src_dll})











