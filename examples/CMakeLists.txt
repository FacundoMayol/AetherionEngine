cmake_minimum_required(VERSION 3.14...3.22)

project(AetherionEngineExamples LANGUAGES CXX)

# --- Import tools ----

include(../cmake/tools.cmake)

# ---- Dependencies ----

include(../cmake/CPM.cmake)

CPMAddPackage(NAME AetherionEngine SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/..)

# ---- Custom functions ----

include(../cmake/shaders.cmake)

# ---- Example game executable creation function ----

function(example_game)
    cmake_parse_arguments(arg "" "NAME" "SOURCES;SHADERS;DEPENDENCIES" ${ARGN})
    
    message("Configuring '${arg_NAME}' example game...")

    if (NOT arg_SOURCES)
        message("No files found. Done.")
        return()
    endif()

    set(GAME_TARGET "${arg_NAME}_game")

    add_executable(${GAME_TARGET} ${arg_SOURCES})
    set_target_properties(${GAME_TARGET} PROPERTIES CXX_STANDARD 20 OUTPUT_NAME "${arg_NAME}")
    target_link_libraries(${GAME_TARGET} PRIVATE AetherionEngine::AetherionEngine ${arg_DEPENDENCIES})

    add_shaders(${GAME_TARGET} ${arg_SHADERS})

    message("Done.")
endfunction()

# ---- Example games inclusion ----

add_subdirectory(triangle)
#add_subdirectory(triangle_buffers)