# CMakeList.txt : CMake project for cpp_test_test2, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.18)
project(SimpleSwitcher VERSION 1.0 LANGUAGES CXX)

set(wxBUILD_SHARED OFF)
set(wxBUILD_USE_STATIC_RUNTIME ON)

set(wxUSE_STC OFF)
#set(wxUSE_XRC OFF)
#set(wxUSE_XML OFF)
#set(wxUSE_WEBVIEW OFF)
#set(wxUSE_RIGHTEXT OFF)
#set(wxUSE_RIBBON OFF)
##set(wxUSE_QA OFF)
##set(wxUSE_PROPGRID OFF)
##set(wxUSE_NET OFF)
set(wxUSE_MEDIA OFF)
#set(wxUSE_HTML OFF)
#set(wxUSE_GL OFF)
#set(wxUSE_AUI OFF)

#set(wxUSE_LIBJPEG OFF)


file(GLOB_RECURSE TARGET_SOURCES  
src/*.cpp 
src/*.cc
src/myres.rc)

#file(GLOB_RECURSE TARGET_PRIVATE_HEADERS     src/*.hpp     src/*.h)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

add_subdirectory(../wxWidgets wxWidgets)
#set(CMAKE_CXX20_STANDARD_COMPILE_OPTION "-std:c++latest")

set(CMAKE_CXX_STANDARD 17)
add_executable (SimpleSwitcher WIN32 ${TARGET_SOURCES})
target_precompile_headers(SimpleSwitcher PRIVATE src/stdafx.h )
target_link_libraries(SimpleSwitcher wx::core wx::base "Crypt32.lib")
target_include_directories(SimpleSwitcher PRIVATE "../libtools")
target_include_directories(SimpleSwitcher PRIVATE "src")











