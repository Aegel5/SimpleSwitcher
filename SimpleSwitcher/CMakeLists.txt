# CMakeList.txt : CMake project for cpp_test_test2, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.18)
project(SimpleSwitcher VERSION 1.0 LANGUAGES CXX)

set(wxUSE_STC OFF)
set(wxBUILD_SHARED OFF)
set(wxBUILD_USE_STATIC_RUNTIME ON)

file(GLOB_RECURSE TARGET_SOURCES  src/*.cpp src/*.cc)

#file(GLOB_RECURSE TARGET_PRIVATE_HEADERS     src/*.hpp     src/*.h)

add_subdirectory(../wxWidgets wxWidgets)

set(CMAKE_CXX_STANDARD 17)
add_executable (SimpleSwitcher WIN32 ${TARGET_SOURCES})
target_link_libraries(SimpleSwitcher wx::core wx::base "Crypt32.lib")
target_include_directories(SimpleSwitcher PRIVATE "../libtools")











