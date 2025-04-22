# CMakeList.txt : CMake project for cpp_test_test2, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.28)
project(SimpleSwitcher VERSION 1.0 LANGUAGES CXX)

 set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

if (NOT TARGET freetype)
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
	set(SKIP_INSTALL_ALL ON CACHE INTERNAL "" FORCE) # disable Freetype install
	set(FT_DISABLE_HARFBUZZ ON CACHE BOOL "" FORCE)
    set(FT_DISABLE_BROTLI ON CACHE BOOL "" FORCE)
    set(FT_DISABLE_PNG ON CACHE BOOL "" FORCE)
    message(STATUS "Fetching freetype")
    include(FetchContent)
    FetchContent_Declare(freetype GIT_REPOSITORY https://github.com/freetype/freetype GIT_TAG VER-2-13-3)
    FetchContent_MakeAvailable(freetype)
endif()


file(GLOB_RECURSE all_implement_units  
src/*.cpp 
#src/res/myres.rc
src/simpleswitcher.manifest
)

list(FILTER all_implement_units EXCLUDE REGEX ".*binary_to_compressed_c.cpp")
list(FILTER all_implement_units EXCLUDE REGEX ".*/old/.*")

#message(all_implement_units=${all_implement_units})

#file(GLOB_RECURSE all_interface_modules  
#src/*.ixx 
#)


set(CMAKE_CXX_STANDARD 23)

add_executable (SimpleSwitcher WIN32)
target_sources(SimpleSwitcher    PUBLIC    ${all_implement_units})

#target_sources(SimpleSwitcher
#  PUBLIC
#    FILE_SET all_my_modules TYPE CXX_MODULES FILES
#    ${all_interface_modules}
#)

target_precompile_headers(SimpleSwitcher PRIVATE src/sw-base.h )

target_link_libraries(SimpleSwitcher "Imm32.lib" "Psapi.lib" freetype "d3d11.lib" "d3dcompiler.lib" "dxgi.lib")

target_include_directories(SimpleSwitcher PRIVATE "src/libtools")
target_include_directories(SimpleSwitcher PRIVATE "src/extern")
target_include_directories(SimpleSwitcher PRIVATE "src/extern/iamgui")
target_include_directories(SimpleSwitcher PRIVATE "src/extern/iamgui/misc/cpp")
target_include_directories(SimpleSwitcher PRIVATE "src/extern/iamgui/backends")
target_include_directories(SimpleSwitcher PRIVATE "${freetype_SOURCE_DIR}/include")
target_include_directories(SimpleSwitcher PRIVATE "src") 











