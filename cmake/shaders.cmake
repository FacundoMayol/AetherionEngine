# TODO: this only handles glsl to SPIR-V.
function(add_shaders TARGET_NAME)
  set(SHADER_SOURCE_FILES ${ARGN}) # The rest of arguments to this function will be assigned as shader source files
  
  # Validate that source files have been passed
  list(LENGTH SHADER_SOURCE_FILES FILE_COUNT)
  if(FILE_COUNT EQUAL 0)
    return()
  endif()

  set(SHADER_COMMANDS)
  set(SHADER_PRODUCTS)

  foreach(SHADER_SOURCE IN LISTS SHADER_SOURCE_FILES)
    cmake_path(ABSOLUTE_PATH SHADER_SOURCE NORMALIZE)
    cmake_path(GET SHADER_SOURCE FILENAME SHADER_NAME)
    
    # Build command
    set(SHADER_COMMAND COMMAND ${Vulkan_GLSLC_EXECUTABLE} ${SHADER_SOURCE} -o ${CMAKE_CURRENT_BINARY_DIR}/${SHADER_NAME}.spv)
    list(APPEND SHADER_COMMANDS ${SHADER_COMMAND})

    # Add product
    list(APPEND SHADER_PRODUCTS "${CMAKE_CURRENT_BINARY_DIR}/${SHADER_NAME}.spv")

  endforeach()

  add_custom_target(${TARGET_NAME}_shaders ALL
    ${SHADER_COMMANDS}
    COMMENT "Compiling Shaders [${SHADER_SOURCE_FILES}]"
    SOURCES "${SHADER_SOURCE_FILES}"
    BYPRODUCTS "${SHADER_PRODUCTS}"
    VERBATIM
  )

  add_dependencies("${TARGET_NAME}" "${TARGET_NAME}_shaders")
endfunction()