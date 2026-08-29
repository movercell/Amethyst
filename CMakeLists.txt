cmake_minimum_required(VERSION 3.15...4.0)

project(AMETHYST VERSION 1.0
                  DESCRIPTION "Very nice project"
		  LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Amethyst build configuration.
set(AMETHYST_BUILD_PROFILE OFF)

if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /arch:AVX")
elseif (CMAKE_C_COMPILER_ID MATCHES "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx")
endif()


# Profiling
if (AMETHYST_BUILD_PROFILE)
add_compile_options(-pg -g)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
endif()


# Include the libraries.
set(GLFW_LIBRARY_TYPE STATIC CACHE STRING "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
add_subdirectory(libs/glfw)

add_library(glad STATIC src/one-off_libs/glad.c)
target_include_directories(glad PRIVATE include)

file(GLOB_RECURSE imgui_sources      libs/imgui/*.cpp)
add_library(imgui STATIC ${imgui_sources})
target_include_directories(imgui PRIVATE "libs/imgui")
target_include_directories(imgui PRIVATE "include")
target_link_libraries(imgui glfw)

add_library(assimp SHARED IMPORTED)
if(WIN32)
set_target_properties(assimp PROPERTIES IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/testgame/libassimp.dll")
set_target_properties(assimp PROPERTIES IMPORTED_IMPLIB "${CMAKE_SOURCE_DIR}/build_windows/libassimp.dll.a")
else()
set_target_properties(assimp PROPERTIES IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/testgame/libassimp.so.6")
endif()


# The main engine source code.
add_executable(amethyst
	src/main.cpp
	src/GameDLLHandler.cpp
	src/utils/EngineMisc.cpp
	src/graphics/Rendering.cpp
	src/graphics/opengl/STDGLRenderer.cpp
	src/graphics/opengl/STDGLCamera.cpp
	src/graphics/opengl/STDGLRWorld.cpp
	src/graphics/opengl/STDGLModel.cpp
	src/graphics/opengl/STDGLWindow.cpp
	src/graphics/opengl/STDGLShaderSystem.cpp
	src/graphics/opengl/STDGLLight.cpp
	src/graphics/opengl/GLMisc.cpp
	src/geometry/Model.cpp
	src/geometry/Alloc2D.cpp
	src/Init.cpp
	src/filesystem/Filesystem.cpp
	src/filesystem/ADFParser.cpp
	src/entities/World.cpp
	src/entities/BasicEntities.cpp
	src/entities/MiscEntities.cpp
)

set_target_properties(amethyst PROPERTIES ENABLE_EXPORTS ON)

# Link the libraries.
target_link_libraries(amethyst PRIVATE glad)
target_link_libraries(amethyst PRIVATE glfw)
target_link_libraries(amethyst PRIVATE imgui)
target_link_libraries(amethyst PRIVATE assimp)


target_include_directories(amethyst PRIVATE include)
target_include_directories(amethyst PRIVATE engineheaders)
target_include_directories(amethyst PRIVATE "include")
target_include_directories(amethyst PRIVATE "libs/imgui")



target_compile_definitions(amethyst PRIVATE AMETHYSTENGINESRC)
target_compile_options(amethyst PUBLIC "-freflection")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	target_compile_options(amethyst PRIVATE "-DDEBUG")
endif()

if(UNIX)
	target_link_options(amethyst PRIVATE "-rdynamic" "-Wl,-rpath,./")
	target_compile_options(amethyst PRIVATE "-fvisibility=hidden")
endif()



# testgame
add_library(testgame SHARED
	testgame_src/main.cpp
)
target_include_directories(testgame PRIVATE engineheaders)
target_include_directories(testgame PRIVATE "libs/imgui")
target_link_libraries(testgame amethyst)
target_compile_options(testgame PUBLIC "-freflection")
set_target_properties(testgame PROPERTIES OUTPUT_NAME "game")
if(UNIX)
	target_compile_options(testgame PRIVATE "-fvisibility=hidden")
endif()

# Copy to the game directory.
add_custom_command(TARGET testgame POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        $<TARGET_FILE:testgame>
        "${CMAKE_CURRENT_SOURCE_DIR}/testgame/"
)
