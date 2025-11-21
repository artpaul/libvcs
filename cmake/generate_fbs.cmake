function(generate_fbs name)
  get_filename_component(FLATBUFFERS_DIR "${name}" DIRECTORY)

  file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${FLATBUFFERS_DIR}")

  add_custom_command(
    OUTPUT
      "${CMAKE_CURRENT_BINARY_DIR}/${name}.fb.h"
    COMMAND
      "${CMAKE_BINARY_DIR}/contrib/flatbuffers/flatc"
    ARGS
      -o "${CMAKE_CURRENT_BINARY_DIR}/${FLATBUFFERS_DIR}"
      --cpp
      --cpp-ptr-type std::unique_ptr
      --cpp-std c++17
      --filename-suffix .fb
      --gen-nullable
      --scoped-enums
      "${CMAKE_CURRENT_SOURCE_DIR}/${name}.fbs"
    DEPENDS
      "${CMAKE_CURRENT_SOURCE_DIR}/${name}.fbs"
    WORKING_DIRECTORY
      "${CMAKE_CURRENT_SOURCE_DIR}/${FLATBUFFERS_DIR}"
  )
endfunction(generate_fbs name)
