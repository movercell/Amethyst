cmake_minimum_required(VERSION 3.15...4.0)

project(AMETHYST VERSION 1.0
                  DESCRIPTION "Very nice project"
		  LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /arch:AVX")
elseif (CMAKE_C_COMPILER_ID MATCHES "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx")
endif()


# Profiling stuff
# add_compile_options(-pg -g)
# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")


# include the libraries
add_subdirectory(libs/glfw)
add_library(glad STATIC src/one-off_libs/glad.c)
target_include_directories(glad PRIVATE include)

file(GLOB_RECURSE imgui_sources      libs/imgui/*.cpp)
add_library(imgui ${imgui_sources})
target_include_directories(imgui PRIVATE "libs/imgui")
target_include_directories(imgui PRIVATE "include")

add_library(assimp SHARED IMPORTED)
if(WIN32)
set_target_properties(assimp PROPERTIES IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/testgame/libassimp.dll")
set_target_properties(assimp PROPERTIES IMPORTED_IMPLIB "${CMAKE_SOURCE_DIR}/build_windows/libassimp.dll.a")
else()
set_target_properties(assimp PROPERTIES IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/testgame/libassimp.so.6")
endif()
	
add_executable(amethyst
	src/main.cpp
	
	src/utils/EngineMisc.cpp
	src/graphics/Rendering.cpp
	src/graphics/opengl/STDGLRenderer.cpp
	src/graphics/opengl/STDGLCamera.cpp
	src/graphics/opengl/STDGLRWorld.cpp
	src/graphics/opengl/STDGLModel.cpp
	src/graphics/opengl/STDGLWindow.cpp
	src/graphics/opengl/STDGLShaderSystem.cpp
	src/graphics/opengl/GLMisc.cpp
	src/geometry/Model.cpp
	src/Init.cpp
	src/filesystem/Filesystem.cpp
	src/filesystem/ADFParser.cpp
	src/entities/World.cpp
	src/entities/BasicEntities.cpp
	)

target_link_libraries(amethyst glad)
target_link_libraries(amethyst glfw)
target_link_libraries(amethyst imgui)
target_link_libraries(amethyst assimp)

target_include_directories(amethyst PRIVATE include)
target_include_directories(amethyst PRIVATE engineheaders)
target_include_directories(amethyst PRIVATE "include")
target_include_directories(amethyst PRIVATE "libs/imgui")



target_compile_definitions(amethyst PRIVATE AMETHYSTENGINESRC)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	target_compile_options(amethyst PRIVATE "-DDEBUG")
endif()

if(UNIX)
	target_link_options(amethyst PRIVATE "-rdynamic" "-Wl,-rpath,./")
	target_compile_options(amethyst PRIVATE "-fvisibility=hidden")
endif()

if(WIN32)
	set_target_properties(amethyst PROPERTIES ENABLE_EXPORTS ON)
    target_link_options(amethyst PRIVATE "-Wl,--out-implib,${CMAKE_CURRENT_BINARY_DIR}/libamethyst.dll.a")
endif()
