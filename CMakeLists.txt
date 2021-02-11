cmake_minimum_required(VERSION 3.10)

project(hello)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin$<0:>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin$<0:>")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib$<0:>")
set(BUILD_SHARED_LIBS OFF CACHE BOOL "CMake should assume static libs.")
set(BUILD_TESTING OFF CACHE BOOL "Don't build testing programs.")
set(ALSOFT_EXAMPLES OFF CACHE BOOL "Don't build OpenAL examples.")
set(ALSOFT_UTILS OFF CACHE BOOL "Don't build OpenAL utility programs.")
set(LIBTYPE STATIC CACHE STRING "Tell OpenAL it's a static lib.")

add_executable(
	hello
	main.cpp
)

add_subdirectory(extern/freetype2)

if(EMSCRIPTEN)
	set(CMAKE_INVOKE emcmake cmake)
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
	target_link_libraries(hello PUBLIC freetype glfw OpenAL)
endif()

install(TARGETS hello RUNTIME DESTINATION "${PROJECT_SOURCE_DIR}/bin")