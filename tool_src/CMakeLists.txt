add_library(basetool
    "${CMAKE_SOURCE_DIR}/src/utils/EngineMisc.cpp"
    "${CMAKE_SOURCE_DIR}/src/filesystem/ADFParser.cpp"
    "basetool/tool_misc.cpp"
    "basetool/Filesystem.cpp"
)
target_include_directories(basetool PUBLIC 
    "${CMAKE_SOURCE_DIR}/src/"
    "${CMAKE_SOURCE_DIR}/engineheaders"
    "${CMAKE_SOURCE_DIR}/include"
    "${CMAKE_SOURCE_DIR}/libs/xxhash"
    "basetool/"
)
target_compile_options(basetool PUBLIC "-freflection")
target_link_libraries(basetool PUBLIC xxhash)


add_executable(atex
	atex/atex.cpp
)
target_link_libraries(atex basetool)

add_custom_command(TARGET atex POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        $<TARGET_FILE:atex>
        "${CMAKE_SOURCE_DIR}/testgame/bin/"
)


add_executable(amdl
	amdl/amdl.cpp
)
target_link_libraries(amdl basetool assimp)

add_custom_command(TARGET amdl POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        $<TARGET_FILE:amdl>
        "${CMAKE_SOURCE_DIR}/testgame/bin/"
)