cmake_minimum_required(VERSION 3.20)

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

# Tests.
if(LIBVCS_BUILD_TESTS)
  enable_testing()
endif()
