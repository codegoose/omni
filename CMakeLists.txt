cmake_minimum_required(VERSION 3.10)

project(hello)

set(CMAKE_CXX_STANDARD 17)

# The generators at the end of these paths prevent VS Build Tools
# from just putting where it wants.

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin$<0:>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin$<0:>")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib$<0:>")

set(BUILD_SHARED_LIBS OFF CACHE BOOL "CMake should assume static libs.")
set(BUILD_TESTING OFF CACHE BOOL "Don't build testing programs.")
set(ALSOFT_EXAMPLES OFF CACHE BOOL "Don't build OpenAL examples.")
set(ALSOFT_UTILS OFF CACHE BOOL "Don't build OpenAL utility programs.")
set(ALSOFT_UPDATE_BUILD_VERSION OFF CACHE BOOL "Don't let OpenAL update it's version all the time.")
set(LIBTYPE STATIC CACHE STRING "Tell OpenAL it's a static lib.")

add_executable(
	hello
	src/main.cpp
	src/app.cpp
)

add_library(
	imgui
	STATIC
	extern/imgui/imgui_demo.cpp
	extern/imgui/imgui_draw.cpp
	extern/imgui/imgui_tables.cpp
	extern/imgui/imgui_widgets.cpp
	extern/imgui/imgui.cpp
	extern/imgui/backends/imgui_impl_glfw.cpp
	extern/imgui/backends/imgui_impl_opengl3.cpp
	extern/imgui/misc/freetype/imgui_freetype.cpp
)

if(NOT EMSCRIPTEN)
	target_include_directories(imgui PUBLIC "extern/glfw/include")
endif()

target_include_directories(
	imgui
	PUBLIC
	"extern/freetype2/include"
	"extern/glad/include"
	"extern/imgui"
	"extern/imgui/misc/freetype"
)

target_include_directories(
	hello
	PUBLIC
	"extern/include"
	"extern/json/include"
	"extern/spdlog/include"
)

if(NOT EMSCRIPTEN)
	add_library(
		glad
		STATIC
		extern/glad/src/glad.c
	)
	target_include_directories(glad PUBLIC "extern/glad/include") # GL 4.5
	target_include_directories(hello PUBLIC "extern/glad/include")
	target_link_libraries(hello PUBLIC glad)
endif()

add_subdirectory(extern/freetype2)
add_subdirectory(extern/spdlog)

if(EMSCRIPTEN)
	set(USE_FLAGS "-s USE_GLFW=3 -s")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${USE_FLAGS}")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${USE_FLAGS}")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${USE_FLAGS}")
	set(CMAKE_EXECUTABLE_SUFFIX .html)
else()
	add_subdirectory(extern/glfw)
	add_subdirectory(extern/openal-soft)
	if(WIN32)
		target_link_libraries(hello PUBLIC opengl32)
	else()
		target_link_libraries(hello PUBLIC GL)
	endif()
	target_link_libraries(hello PUBLIC glfw OpenAL)
endif()

target_link_libraries(hello PUBLIC imgui freetype)

install(TARGETS hello RUNTIME DESTINATION "${PROJECT_SOURCE_DIR}/bin")