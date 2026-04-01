cmake_minimum_required (VERSION 3.31.5)
project(SimpleSwitcher VERSION 1.0 LANGUAGES CXX)

# Глобальные настройки

if(WIN32)
add_definitions(-D_WIN32_WINNT=0x0601)
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

if(MSVC)
	add_compile_options(/utf-8)

    # 1) Dead Code Elimination (Удаление мертвого кода)
    # /Gy — разделяет функции на сегменты, чтобы линковщик мог их удалить
    # /OPT:REF — удаляет функции, на которые нет ссылок
    add_compile_options($<$<CONFIG:Release>:/Gy>)
    add_link_options($<$<CONFIG:Release>:/OPT:REF>)
    # add_link_options($<$<CONFIG:Release>:/OPT:ICF>)
	# add_link_options($<$<CONFIG:Release>:/INCREMENTAL:NO>) # Оптимизации выше (REF/ICF) НЕ работают, если включена инкрементальная линковка.
	add_compile_options(/GF)
endif()

# Проверка и включение IPO только после объявления project
include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_supported OUTPUT ipo_error)
if(ipo_supported)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
	message("IPO ENABLED")
else()
    message(WARNING "IPO is NOT supported: ${ipo_error}")
endif()


# Project
add_executable (${CMAKE_PROJECT_NAME} WIN32)
target_sources(${CMAKE_PROJECT_NAME}  PRIVATE src/manifest.manifest src/res/res.rc)
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE "src/libtools")
target_precompile_headers(${CMAKE_PROJECT_NAME} PRIVATE src/precompl.h)
#target_sources(${CMAKE_PROJECT_NAME} PRIVATE FILE_SET CXX_MODULE_HEADER_UNITS  FILES src/precompl.h)
block()
	file(GLOB_RECURSE src CONFIGURE_DEPENDS "src/*.cpp" "src/*.h" ) # CONFIGURE_DEPENDS - искать новые файлы при билде.
	list(FILTER src EXCLUDE REGEX ".*/old/.*")
	target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${src})
	#file(GLOB_RECURSE src_modules CONFIGURE_DEPENDS  "src/*.cppm" "src/*.ixx" )
	#if(src_modules)
	#	 target_sources(${CMAKE_PROJECT_NAME} PRIVATE FILE_SET CXX_MODULES FILES ${src_modules})
	#	 set_source_files_properties(${src_modules} PROPERTIES SKIP_PRECOMPILE_HEADERS ON)
	#endif()
	target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE "src") 
endblock()



# ImguiLib
block()
	add_library(ImguiLib OBJECT)
	file(GLOB_RECURSE src  imgui/*.cpp )
	list(FILTER src EXCLUDE REGEX ".*binary_to_compressed_c.cpp")
	list(FILTER src EXCLUDE REGEX ".*/backends/.*")
	list(FILTER src EXCLUDE REGEX ".*/examples/.*")
	list(APPEND src imgui/backends/imgui_impl_dx11.cpp)
	list(APPEND src imgui/backends/imgui_impl_win32.cpp)
	target_sources(ImguiLib PRIVATE ${src})
	target_include_directories(ImguiLib PUBLIC "imgui") # PUBLIC - автоматически прокидываем дальше
	# подключаем freetype
	block()
		set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
		set(SKIP_INSTALL_ALL ON CACHE INTERNAL "" FORCE) 
		set(FT_DISABLE_HARFBUZZ ON CACHE BOOL "" FORCE)
		set(FT_DISABLE_BROTLI ON CACHE BOOL "" FORCE)
		set(FT_DISABLE_PNG ON CACHE BOOL "" FORCE)
		message(STATUS "Fetching freetype")
		include(FetchContent)
		FetchContent_Declare(freetype GIT_REPOSITORY https://github.com/freetype/freetype GIT_TAG VER-2-13-3)
		FetchContent_MakeAvailable(freetype)
		target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE freetype)
		target_include_directories(ImguiLib PRIVATE "${freetype_SOURCE_DIR}/include")
	endblock()
	target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE ImguiLib)
	target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE "d3d11.lib"  "dxgi.lib")
endblock()

# ExternLib
block()
	file(GLOB_RECURSE src  extern/*.cpp )
	if(src)
		add_library(ExternLib OBJECT)
		target_sources(ExternLib PRIVATE ${src})
		target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE ExternLib)
	endif()
	target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE "extern")
endblock()

# LibUnits
block()
	file(GLOB_RECURSE src1 CONFIGURE_DEPENDS "lib_units/*.cpp" )
	file(GLOB_RECURSE headers CONFIGURE_DEPENDS "lib_units/*.h" )
	file(GLOB_RECURSE src2 CONFIGURE_DEPENDS  "lib_units/*.cppm" )
	if(src1 OR src2)
		add_library(LibUnits OBJECT)
		target_sources(LibUnits PRIVATE ${src1})
		target_sources(LibUnits PRIVATE  FILE_SET CXX_MODULES FILES ${src2})
	else()
		add_library(LibUnits INTERFACE)
	endif()
	target_sources(LibUnits INTERFACE ${headers})
	target_include_directories(LibUnits INTERFACE "lib_units")
	target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE LibUnits)
endblock()

















