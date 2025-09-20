cmake_minimum_required(VERSION 3.14...3.22)

# ---- Project ----

# Note: update this to your new project's name and version
project(
  AetherionEngine
  VERSION 1.0
  LANGUAGES CXX
)

# ---- Include guards ----

if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
  message(
    FATAL_ERROR
      "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there."
  )
endif()

# ---- Vulkan ----

# Vulkan SDK is absolutely needed.
find_package(Vulkan REQUIRED FATAL_ERROR)

# ---- Add dependencies via CPM ----
# see https://github.com/TheLartians/CPM.cmake for more info

include(cmake/CPM.cmake)

# PackageProject.cmake will be used to make our target installable
CPMAddPackage("gh:TheLartians/PackageProject.cmake@1.8.0")

# OpenGL

find_package(OpenGL REQUIRED)

# FMT

CPMAddPackage(
  NAME fmt
  GIT_TAG 11.1.1
  GITHUB_REPOSITORY fmtlib/fmt
)

# GLM

CPMAddPackage(
  NAME glm
  GITHUB_REPOSITORY g-truc/glm
  GIT_TAG 1.0.1
  OPTIONS
    "GLM_BUILD_TESTS OFF"
    "GLM_BUILD_INSTALL ON"
)

# GLFW

CPMAddPackage(
  NAME glfw
  GITHUB_REPOSITORY glfw/glfw
  GIT_TAG 3.3.2
  OPTIONS
    "GLFW_BUILD_TESTS OFF"
    "GLFW_BUILD_EXAMPLES OFF"
    "GLFW_BULID_DOCS OFF"
)

# VMA

CPMAddPackage(
  NAME VulkanMemoryAllocator
  GITHUB_REPOSITORY GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator
  GIT_TAG v3.2.1
  OPTIONS
    "VMA_ENABLE_INSTALL true"
)

CPMAddPackage(
  NAME VulkanMemoryAllocator-Hpp
  GITHUB_REPOSITORY YaaZ/VulkanMemoryAllocator-Hpp
  GIT_TAG v3.2.1
  OPTIONS
    "VMA_HPP_ENABLE_INSTALL true"
)

# VKB

set(VK_BOOTSTRAP_INSTALL ON CACHE BOOL "Enable vk-bootstrap install" FORCE)
CPMAddPackage(
  NAME vk-bootstrap
  GIT_REPOSITORY https://github.com/charles-lunarg/vk-bootstrap
  GIT_TAG v1.4.307
)

# EnTT

CPMAddPackage(
  NAME EnTT
  VERSION 3.14.0
  GITHUB_REPOSITORY skypjack/entt
  # EnTT's CMakeLists screws with configuration options
  DOWNLOAD_ONLY True
)

if (EnTT_ADDED)
  add_library(EnTT INTERFACE IMPORTED)
  target_include_directories(EnTT SYSTEM INTERFACE ${EnTT_SOURCE_DIR}/src)
endif()

# tinygltf

CPMAddPackage(
  NAME tinygltf
  VERSION 2.9.5
  GITHUB_REPOSITORY syoyo/tinygltf
  OPTIONS
    "TINYGLTF_HEADER_ONLY true"
    "TINYGLTF_USE_CPP14 true"
    "TINYGLTF_NO_INCLUDE_STB_IMAGE true"
    "TINYGLTF_NO_INCLUDE_STB_IMAGE_WRITE true"
    "TINYGLTF_BUILD_LOADER_EXAMPLE off"
)

# stb

CPMAddPackage(
  NAME STB
  GITHUB_REPOSITORY nothings/stb
  GIT_TAG 5c205738c191bcb0abc65c4febfa9bd25ff35234
  # No CMakeLists
  DOWNLOAD_ONLY True
)

add_library(STB INTERFACE IMPORTED)
target_include_directories(STB SYSTEM INTERFACE ${STB_SOURCE_DIR})

# FastNoiseLite

CPMAddPackage(
  NAME FastNoiseLite
  VERSION 1.1.1
  GITHUB_REPOSITORY Auburn/FastNoiseLite
  DOWNLOAD_ONLY
)

if (FastNoiseLite_ADDED)
  # Single-header file

  add_library(FastNoiseLite INTERFACE IMPORTED)
  target_include_directories(FastNoiseLite SYSTEM INTERFACE ${FastNoiseLite_SOURCE_DIR}/Cpp)
endif()

# Freetype

CPMAddPackage(
  NAME freetype
  GIT_REPOSITORY https://github.com/aseprite/freetype2.git
  GIT_TAG VER-2-10-0
  VERSION 2.10.0
)

if (freetype_ADDED)
  add_library(Freetype::Freetype ALIAS freetype)
endif()

# ImGui

CPMAddPackage(
  NAME imgui
  VERSION 1.91.1
  GITHUB_REPOSITORY ocornut/imgui
  DOWNLOAD_ONLY TRUE
)

# CMakeLists.txt from https://gist.githubusercontent.com/rokups/f771217b2d530d170db5cb1e08e9a8f4
file(
  DOWNLOAD
  "https://gist.githubusercontent.com/rokups/f771217b2d530d170db5cb1e08e9a8f4/raw/4c2c14374ab878ca2f45daabfed4c156468e4e27/CMakeLists.txt"
  "${imgui_SOURCE_DIR}/CMakeLists.txt"
  EXPECTED_HASH SHA256=fd62f69364ce13a4f7633a9b50ae6672c466bcc44be60c69c45c0c6e225bb086
)

set(IMGUI_EXAMPLES FALSE)
set(IMGUI_DEMO FALSE)
set(IMGUI_ENABLE_STDLIB_SUPPORT TRUE)
# FreeType (https://github.com/cpm-cmake/CPM.cmake/wiki/More-Snippets#freetype)
set(FREETYPE_FOUND TRUE)
set(FREETYPE_INCLUDE_DIRS "")
set(FREETYPE_LIBRARIES Freetype::Freetype)

add_subdirectory(${imgui_SOURCE_DIR} EXCLUDE_FROM_ALL SYSTEM)

add_library(imgui_imported INTERFACE IMPORTED)
set_property(TARGET imgui_imported PROPERTY INTERFACE_INCLUDE_DIRECTORIES
    $<TARGET_PROPERTY:imgui,INTERFACE_INCLUDE_DIRECTORIES>)
set_property(TARGET imgui_imported PROPERTY INTERFACE_LINK_LIBRARIES
    $<TARGET_PROPERTY:imgui,INTERFACE_LINK_LIBRARIES>)
    
add_library(imgui-glfw_imported INTERFACE IMPORTED)
set_property(TARGET imgui-glfw_imported PROPERTY INTERFACE_INCLUDE_DIRECTORIES
    $<TARGET_PROPERTY:imgui-glfw,INTERFACE_INCLUDE_DIRECTORIES>)
set_property(TARGET imgui-glfw_imported PROPERTY INTERFACE_LINK_LIBRARIES
    $<TARGET_PROPERTY:imgui-glfw,INTERFACE_LINK_LIBRARIES>)
    
add_library(imgui-vulkan_imported INTERFACE IMPORTED)
set_property(TARGET imgui-vulkan_imported PROPERTY INTERFACE_INCLUDE_DIRECTORIES
    $<TARGET_PROPERTY:imgui-vulkan,INTERFACE_INCLUDE_DIRECTORIES>)
set_property(TARGET imgui-vulkan_imported PROPERTY INTERFACE_LINK_LIBRARIES
    $<TARGET_PROPERTY:imgui-vulkan,INTERFACE_LINK_LIBRARIES>)

# ---- Custom functions ----

include(cmake/shaders.cmake)

# ---- Add source files ----

# Note: globbing sources is considered bad practice as CMake's generators may not detect new files
# automatically. Keep that in mind when changing files, or explicitly mention them here.
file(GLOB_RECURSE headers CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp")
file(GLOB_RECURSE sources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/source/*.cppm")
file(GLOB_RECURSE shaders CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/shader/*.vert" "${CMAKE_CURRENT_SOURCE_DIR}/shader/*.frag" "${CMAKE_CURRENT_SOURCE_DIR}/shader/*.glsl")

# ---- Create library ----

# Note: for header-only libraries change all PUBLIC flags to INTERFACE and create an interface
# target: add_library(${PROJECT_NAME} INTERFACE)
add_library(${PROJECT_NAME} ${headers} ${sources})
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 20)

# being a cross-platform target, we enforce standards conformance on MSVC
target_compile_options(${PROJECT_NAME} PUBLIC "$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/permissive->")

# Link dependencies
target_link_libraries(${PROJECT_NAME} 
  Vulkan::Vulkan 
  vk-bootstrap
  GPUOpen::VulkanMemoryAllocator 
  VulkanMemoryAllocator-Hpp::VulkanMemoryAllocator-Hpp 
  glm::glm 
  glfw 
  EnTT
  tinygltf
  fmt::fmt 
  STB
  FastNoiseLite
  Freetype::Freetype
  imgui_imported imgui-glfw_imported imgui-vulkan_imported)

# Add shaders
add_shaders(${PROJECT_NAME} ${shaders})

target_include_directories(
  ${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
                         $<INSTALL_INTERFACE:include/${PROJECT_NAME}-${PROJECT_VERSION}>
)

# ---- Create an installable target ----
# this allows users to install and find the library via `find_package()`.

# the location where the project's version header will be placed should match the project's regular
# header paths
string(TOLOWER ${PROJECT_NAME}/version.h VERSION_HEADER_LOCATION)

packageProject(
  NAME ${PROJECT_NAME}
  VERSION ${PROJECT_VERSION}
  NAMESPACE ${PROJECT_NAME}
  BINARY_DIR ${PROJECT_BINARY_DIR}
  INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include
  INCLUDE_DESTINATION include/${PROJECT_NAME}-${PROJECT_VERSION}
  VERSION_HEADER "${VERSION_HEADER_LOCATION}"
  COMPATIBILITY SameMajorVersion
  DEPENDENCIES 
    "fmt 11.1.1"
    "glm 1.0.1"
    "glfw 3.3.2"
    "VulkanMemoryAllocator 3.2.1"
    "VulkanMemoryAllocator-Hpp 3.2.1"
    "vk-bootstrap 1.4.307"
    "EnTT 3.14.0"
    "stb 5c205738c191bcb0abc65c4febfa9bd25ff35234"
    "FastNoiseLite 1.1.1"
    "tinygltf 2.9.5"
    "freetype 2.10.0"
    "imgui 1.91.1"
)