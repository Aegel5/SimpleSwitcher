cmake_minimum_required (VERSION 3.31.5)
project(SimpleSwitcher VERSION 1.0 LANGUAGES CXX)

# Глобальные настройки

if(WIN7_COMPAT)
	add_compile_definitions(_WIN32_WINNT=0x0601)
	add_compile_definitions(SS_WIN_7_COMPAT)
endif()

set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
#set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

if(MSVC)
	add_compile_options(/utf-8)
	add_compile_options(/std:c++latest)
	add_compile_options(/we5030)
	add_compile_options(/GF) # String Pooling

    # 1) Dead Code Elimination (Удаление мертвого кода)
    add_compile_options($<$<CONFIG:Release>:/Gy>)     # /Gy — разделяет функции на сегменты, чтобы линковщик мог их удалить
    add_link_options($<$<CONFIG:Release>:/OPT:REF>)     # /OPT:REF — удаляет функции, на которые нет ссылок


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
	file(GLOB_RECURSE src  "src/*.cpp" "src/*.h" ) # CONFIGURE_DEPENDS - искать новые файлы при билде.
	list(FILTER src EXCLUDE REGEX ".*/old/.*")
	target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${src})
	#file(GLOB_RECURSE src_modules   "src/*.cppm" "src/*.ixx" )
	#if(src_modules)
	#	 target_sources(${CMAKE_PROJECT_NAME} PRIVATE FILE_SET CXX_MODULES FILES ${src_modules})
	#	 set_source_files_properties(${src_modules} PROPERTIES SKIP_PRECOMPILE_HEADERS ON)
	#endif()
	target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE "src") 
endblock()


# SHA DEFINE
block()
	# 1. Берем хэш из окружения GitHub (GITHUB_SHA есть в рантайме экшена по умолчанию)
	set(TMP_GIT_HASH $ENV{GITHUB_SHA})
	# 2. Если пусто (локальная сборка), ставим заглушку "unknown" или пустую строку ""
	if(NOT TMP_GIT_HASH)
		set(TMP_GIT_HASH "")
	endif()
	# 3. Пробрасываем в C++ (CMake сам добавит кавычки, если значение внутри кавычек в set)
	target_compile_definitions(${PROJECT_NAME} PRIVATE GIT_COMMIT_HASH="${TMP_GIT_HASH}")
endblock()


# ImguiLib
block()
	add_library(ImguiLib OBJECT)
	file(GLOB src 
		"imgui_tiny_app/*.cpp"
		"imgui_tiny_app/misc/cpp/imgui_stdlib.cpp"
	)
	if(WIN32)
		list(APPEND src imgui_tiny_app/backends/imgui_impl_dx11.cpp)
		list(APPEND src imgui_tiny_app/backends/imgui_impl_win32.cpp)
	endif()
	target_sources(ImguiLib PRIVATE ${src})
	target_include_directories(ImguiLib PUBLIC "imgui_tiny_app") # PUBLIC - автоматически прокидываем дальше
	add_compile_definitions(IMGUI_DISABLE_OBSOLETE_FUNCTIONS)
	add_compile_definitions(IMGUI_IMPL_WIN32_DISABLE_GAMEPAD)
	add_compile_definitions("$<$<CONFIG:Release>:IMGUI_DISABLE_DEMO_WINDOWS;IMGUI_DISABLE_DEBUG_TOOLS>")
	target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE ImguiLib)
	block()
		target_sources(ImguiLib PRIVATE imgui_tiny_app/misc/freetype/imgui_freetype.cpp)
		target_compile_definitions(ImguiLib PRIVATE IMGUI_ENABLE_FREETYPE) # включаем поддержку в imgui
		set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
		set(SKIP_INSTALL_ALL ON CACHE INTERNAL "" FORCE) 
		set(FT_DISABLE_HARFBUZZ ON CACHE BOOL "" FORCE)
		set(FT_DISABLE_BROTLI ON CACHE BOOL "" FORCE)
		set(FT_DISABLE_PNG ON CACHE BOOL "" FORCE)
		message(STATUS "Fetching freetype")
		include(FetchContent)
		FetchContent_Declare(
			freetype GIT_REPOSITORY https://github.com/freetype/freetype GIT_TAG VER-2-14-3
        GIT_SHALLOW    TRUE   # <--- ВОТ ЭТА ШТУКА (аналог --depth 1)
        GIT_PROGRESS   TRUE   # Чтобы видеть проценты в консоли, а не сидеть в тишине
		)
		FetchContent_MakeAvailable(freetype)
		target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE freetype)
		target_include_directories(ImguiLib PRIVATE "${freetype_SOURCE_DIR}/include")
	endblock()
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
	file(GLOB_RECURSE src1  "lib_units/*.cpp" )
	file(GLOB_RECURSE headers  "lib_units/*.h" )
	file(GLOB_RECURSE src2   "lib_units/*.cppm" )
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

















