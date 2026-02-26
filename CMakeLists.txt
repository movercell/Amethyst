cmake_minimum_required(VERSION 3.15...4.0)

project(AMETHYST VERSION 1.0
                  DESCRIPTION "Very nice project"
		  LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /arch:AVX")
elseif (CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx")
endif()


# Profiling stuff
# add_compile_options(-pg -g)
# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")



include_directories(include)
include_directories(engineheaders)

# include the libraries
add_subdirectory(libs/glfw)
add_library(glad STATIC src/one-off_libs/glad.c)

include_directories("libs/imgui")
file(GLOB_RECURSE imgui_sources      libs/imgui/*.cpp)
add_library(imgui ${imgui_sources})

file(GLOB_RECURSE vdfparser_sources      libs/vdfparser/*.cpp)
add_library(vdfparser ${vdfparser_sources})
add_library(assimp SHARED IMPORTED)
if(WIN32)
set_target_properties(assimp PROPERTIES IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/testgame/libassimp.dll")
set_target_properties(assimp PROPERTIES IMPORTED_IMPLIB "${CMAKE_SOURCE_DIR}/build_windows/libassimp.dll.a")
else()
set_target_properties(assimp PROPERTIES IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/testgame/libassimp.so.6")
endif()
	
add_executable(amethyst
	src/main.cpp
	
	src/graphics/opengl/STDGLRenderer.cpp
	src/graphics/opengl/STDGLCamera.cpp
	src/graphics/opengl/STDGLRWorld.cpp
	src/graphics/opengl/STDGLModel.cpp
	src/graphics/opengl/STDGLWindow.cpp
	src/graphics/opengl/GLMisc.cpp
	src/geometry/Model.cpp
	)

target_link_libraries(amethyst glad)
target_link_libraries(amethyst glfw)
target_link_libraries(amethyst imgui)
target_link_libraries(amethyst vdfparser)
target_link_libraries(amethyst assimp)


target_compile_definitions(amethyst PUBLIC AMETHYSTENGINESRC)

target_link_options(amethyst PUBLIC "-rdynamic" "-Wl,-rpath,./")
target_compile_options(amethyst PUBLIC "-fvisibility=hidden")

