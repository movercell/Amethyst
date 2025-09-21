cmake_minimum_required(VERSION 3.15...4.0)

project(AMETHYST VERSION 1.0
                  DESCRIPTION "Very nice project"
		  LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)

include_directories(include)
include_directories(engineheaders)

# include the libraries
add_subdirectory(libs/glfw)
add_library(glad STATIC src/one-off_libs/glad.c)
include_directories("libs/imgui")
file(GLOB_RECURSE imgui_sources      libs/imgui/*.cpp)
add_library(imgui ${imgui_sources})
	
add_executable(amethyst
	src/main.cpp
	src/rendering/Window.cpp
	src/rendering/Context.cpp
	src/rendering/opengl/GLContext.cpp
	)

target_link_libraries(amethyst glad)
target_link_libraries(amethyst glfw)
target_link_libraries(amethyst imgui)

