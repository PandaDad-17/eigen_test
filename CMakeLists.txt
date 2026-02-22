cmake_minimum_required(VERSION 3.24) # fetch content with args

include(FetchContent)

project(
  test
  LANGUAGES CXX
  VERSION 0.0.1
)

# --------------------------------------------------
# Compiler settings

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(NOT DEFINED CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()          

# Link shared libraries with executable.
set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

if(LINUX)
  set(CMAKE_INSTALL_RPATH "$ORIGIN")
endif()

# --------------------------------------------------
# Fetch Dependencies

# SDL3
set(SDL_TESTS OFF CACHE BOOL "" FORCE)
set(SDL_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SDL_SHARED ON CACHE BOOL "" FORCE)
set(SDL_STATIC OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
  SDL3
  GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
  GIT_TAG release-3.4.x
  FIND_PACKAGE_ARGS NAMES SDL3
)

# Eigen
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(EIGEN_BUILD_PKGCONFIG OFF CACHE BOOL "" FORCE)
set(EIGEN_BUILD_DOC OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
  eigen
  GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
  GIT_TAG 3.4.0
  FIND_PACKAGE_ARGS NAMES Eigen3
)

FetchContent_MakeAvailable(SDL3 eigen)

# --------------------------------------------------
# Project Setup

set(ROOT_DIR ${CMAKE_CURRENT_LIST_DIR})
set(MAIN "${ROOT_DIR}/src/main.cpp")

set(INCLUDE_DIRS "${ROOT_DIR}/src")

file(GLOB_RECURSE SOURCE_FILES "${ROOT_DIR}/src/*.cpp")
list(REMOVE_ITEM SOURCE_FILES "${MAIN}")

add_executable(${PROJECT_NAME} ${MAIN})

# --------------------------------------------------
# Target Configuration

target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE_DIRS})

target_sources(${PROJECT_NAME} PUBLIC ${MAIN} ${SOURCE_FILES})

# Link against the namespaced targets created by FetchContent
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3 Eigen3::Eigen)

# Linux-specific math linkage for SDL
if(LINUX)
    find_library(HAVE_LIBM m)
    if(HAVE_LIBM)
      target_link_libraries(${PROJECT_NAME} PRIVATE m)
    endif()
endif()

# --------------------------------------------------
# Post build

# Windows can't use RPATH and therefore has to copy the shared libraries to the executable directory.
if(WIN32)
  add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:SDL3::SDL3>
    $<TARGET_FILE_DIR:${PROJECT_NAME}>)
endif()