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
src/simpleswitcher.manifest
)
#message(all_implement_units=${all_implement_units})
list(FILTER all_implement_units EXCLUDE REGEX ".*/old/.*")


#set_source_files_properties(src/res/res.rc PROPERTIES LANGUAGE RC)


file(GLOB_RECURSE extern_files  extern/*.cpp )
list(FILTER extern_files EXCLUDE REGEX ".*binary_to_compressed_c.cpp")
list(FILTER extern_files EXCLUDE REGEX ".*/backends/.*")
list(FILTER extern_files EXCLUDE REGEX ".*/examples/.*")
list(APPEND extern_files extern/imgui/backends/imgui_impl_dx11.cpp)
list(APPEND extern_files extern/imgui/backends/imgui_impl_win32.cpp)
set_source_files_properties(${extern_files} PROPERTIES SKIP_PRECOMPILE_HEADERS ON)

#file(GLOB_RECURSE all_interface_modules  
#src/*.ixx 
#)
#target_sources(SimpleSwitcher
#  PUBLIC
#    FILE_SET all_my_modules TYPE CXX_MODULES FILES
#    ${all_interface_modules}
#)

set(CMAKE_CXX_SCAN_FOR_MODULES off)
set(CMAKE_CXX_STANDARD 23)

IF(PUBLIC_RELEASE)
    ADD_DEFINITIONS(-DPUBLIC_RELEASE)
ENDIF(PUBLIC_RELEASE)

add_executable (SimpleSwitcher WIN32)
target_sources(SimpleSwitcher    PRIVATE    ${extern_files}  ${all_implement_units} src/res/res.rc)
target_precompile_headers(SimpleSwitcher PRIVATE src/sw-base.h)


target_link_libraries(SimpleSwitcher PRIVATE "Imm32.lib" "Psapi.lib" freetype "d3d11.lib" "d3dcompiler.lib" "dxgi.lib")

target_include_directories(SimpleSwitcher PRIVATE "src/libtools")
target_include_directories(SimpleSwitcher PRIVATE "extern")
target_include_directories(SimpleSwitcher PRIVATE "extern/imgui")
target_include_directories(SimpleSwitcher PRIVATE "extern/imgui/misc/cpp")
target_include_directories(SimpleSwitcher PRIVATE "extern/imgui/backends")
target_include_directories(SimpleSwitcher PRIVATE "src") 
target_include_directories(SimpleSwitcher PRIVATE "${freetype_SOURCE_DIR}/include")












