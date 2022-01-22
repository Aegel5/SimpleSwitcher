# CMakeList.txt : CMake project for cpp_test_test2, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.18)
project(SimpleSwitcher VERSION 1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE TARGET_SOURCES
     src/*.cpp)

file(GLOB_RECURSE TARGET_PRIVATE_HEADERS
     src/*.hpp
     src/*.h)

add_subdirectory(extern/wxWidgets)
add_executable (SimpleSwitcher ${TARGET_SOURCES})
target_link_libraries(SimpleSwitcher wx::net wx::core wx::base "Crypt32.lib")
target_include_directories(SimpleSwitcher PRIVATE "../libtools")











