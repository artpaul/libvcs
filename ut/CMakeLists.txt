add_executable(libvcs3-ut)

target_link_libraries(libvcs3-ut
  contrib-gtest
  libvcs3
)

target_sources(libvcs3-ut PRIVATE
  "main.cc"
)

# Unit tests.
target_sources(libvcs3-ut PRIVATE
  "ut_hashid.cc"
  "ut_hex.cc"
)
