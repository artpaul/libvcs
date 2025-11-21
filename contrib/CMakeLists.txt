add_subdirectory(flatbuffers)
add_subdirectory(lz4)
add_subdirectory(sha)
add_subdirectory(xxhash)

# Tests.
if(LIBVCS_BUILD_TESTS)
  add_subdirectory(gtest)
endif()
