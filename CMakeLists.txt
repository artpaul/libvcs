cmake_minimum_required(VERSION 3.25)

option(LIBVCS_BUILD_TESTS "Build tests" ON)

project(libvcs LANGUAGES C CXX)

LIST(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

find_package(Threads REQUIRED)

set(BUILD_SHARED_LIBS OFF)
set(CMAKE_CXX_STANDARD 17)

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  # Colored output.
  set(CMAKE_COLOR_DIAGNOSTICS ON)
  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()

if(UNIX AND NOT APPLE)
  set(LINUX TRUE)
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++ -lc++abi")
endif()

# Third parties come with their own setup of
# C++ standard and compiler options.
add_subdirectory(contrib EXCLUDE_FROM_ALL)

# Determine whether this is a standalone project or
# included by other projects.
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  # Setup C++ standard for the project.
  set(CMAKE_CXX_STANDARD 23)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_CXX_EXTENSIONS OFF)

  set(LIBVCS_BUILD_TESTS ON)
elseif(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 23)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

set(LIBVCS_PUBLIC_INCLUDE_DIR "include/libvcs")

add_library(libvcs3 STATIC)

add_dependencies(libvcs3 flatc)

target_include_directories(libvcs3 PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

include(generate_fbs)

generate_fbs(${LIBVCS_PUBLIC_INCLUDE_DIR}/fbs/blob)
generate_fbs(${LIBVCS_PUBLIC_INCLUDE_DIR}/fbs/commit)
generate_fbs(${LIBVCS_PUBLIC_INCLUDE_DIR}/fbs/renames)
generate_fbs(${LIBVCS_PUBLIC_INCLUDE_DIR}/fbs/tag)
generate_fbs(${LIBVCS_PUBLIC_INCLUDE_DIR}/fbs/tree)
generate_fbs(${LIBVCS_PUBLIC_INCLUDE_DIR}/fbs/types)

target_include_directories(libvcs3
  SYSTEM PUBLIC
    # Flatbuffers includes.
    "${CMAKE_BINARY_DIR}"
  PUBLIC
    # Third party includes.
    "contrib"
)

target_sources(libvcs3 PRIVATE
  "src/hashid.cc"
)

target_sources(libvcs3 PRIVATE
  "${CMAKE_CURRENT_BINARY_DIR}/${LIBVCS_PUBLIC_INCLUDE_DIR}/fbs/blob.fb.h"
  "${CMAKE_CURRENT_BINARY_DIR}/${LIBVCS_PUBLIC_INCLUDE_DIR}/fbs/commit.fb.h"
  "${CMAKE_CURRENT_BINARY_DIR}/${LIBVCS_PUBLIC_INCLUDE_DIR}/fbs/renames.fb.h"
  "${CMAKE_CURRENT_BINARY_DIR}/${LIBVCS_PUBLIC_INCLUDE_DIR}/fbs/types.fb.h"
  "${CMAKE_CURRENT_BINARY_DIR}/${LIBVCS_PUBLIC_INCLUDE_DIR}/fbs/tree.fb.h"
  "${CMAKE_CURRENT_BINARY_DIR}/${LIBVCS_PUBLIC_INCLUDE_DIR}/fbs/types.fb.h"
)

target_link_libraries(libvcs3 PUBLIC
  contrib-flatbuffers
  contrib-sha
)

# Tests.
if(LIBVCS_BUILD_TESTS)
  enable_testing()
  add_subdirectory(ut)
endif()
