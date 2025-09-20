file(GLOB_RECURSE Triangle_includes CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/include/*.h" "${CMAKE_CURRENT_LIST_DIR}/include/*.hpp")
file(GLOB_RECURSE Triangle_sources CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/source/*.cpp" "${CMAKE_CURRENT_LIST_DIR}/source/*.cppm")
file(GLOB_RECURSE Triangle_shaders CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/shader/*.vert" "${CMAKE_CURRENT_LIST_DIR}/shader/*.frag"
"${CMAKE_CURRENT_LIST_DIR}/shader/*.glsl")

example_game(
    NAME Triangle 
    SOURCES "${Triangle_sources}"
    SHADERS "${Triangle_shaders}"
    DEPENDENCIES "")