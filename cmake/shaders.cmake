function(badger_engine_add_shader_target target)
  cmake_parse_arguments(PARSE_ARGV 1 arg "" "OUTPUT_DIR" "SOURCES")

  if(NOT DEFINED arg_OUTPUT_DIR)
    message("error" FATAL_ERROR)
  endif()

  file(MAKE_DIRECTORY ${arg_OUTPUT_DIR})
  set(ext spv)

  set(output_file_paths)
  foreach(source_file_path ${arg_SOURCES})

    get_filename_component(source_file_name ${source_file_path} NAME)
    get_filename_component(
      output_file_path ${arg_OUTPUT_DIR}/${source_file_name}.${ext} ABSOLUTE)

    message(
      "source_file_path: ${source_file_path}, output_file_path: ${output_file_path}"
    )

    add_custom_command(
      DEPENDS ${source_file_path}
      COMMAND glslc ${source_file_path} -o ${output_file_path}
      OUTPUT ${output_file_path})

    list(APPEND output_file_paths ${output_file_path})

  endforeach()

  message("output_file_paths: ${output_file_paths}")

  add_custom_target(
    ${target}
    SOURCES ${arg_SOURCES}
    DEPENDS ${output_file_paths})

endfunction()

function(badger_engine_add_embedded_shader_target target)
  cmake_parse_arguments(PARSE_ARGV 1 arg "" "OUTPUT_DIR" "SOURCES")

  if(NOT DEFINED arg_OUTPUT_DIR)
    message("error" FATAL_ERROR)
  endif()

  get_filename_component(intermediate_dir ${arg_OUTPUT_DIR}/intermediate
                         ABSOLUTE)

  file(MAKE_DIRECTORY ${intermediate_dir})
  set(ext spv)

  set(output_file_paths)
  foreach(source_file_path ${arg_SOURCES})

    get_filename_component(source_file_name ${source_file_path} NAME)
    get_filename_component(
      intermediate_file_path ${intermediate_dir}/${source_file_name}.${ext}
      ABSOLUTE)

    get_filename_component(
      output_file_path ${arg_OUTPUT_DIR}/${source_file_name}.${ext}.h ABSOLUTE)

    string(REPLACE "." "_" variable_file_name ${source_file_name})

    message(
      "source_file_path: ${source_file_path}, intermediate_file_path: ${intermediate_file_path}"
    )

    add_custom_command(
      DEPENDS ${source_file_path}
      COMMAND glslc ${source_file_path} -o ${intermediate_file_path}
      OUTPUT ${intermediate_file_path})

    add_custom_command(
      DEPENDS ${intermediate_file_path}
      COMMAND bin2c ${intermediate_file_path} --name ${variable_file_name}
              --const > ${output_file_path}
      OUTPUT ${output_file_path})

    list(APPEND output_file_paths ${output_file_path})

  endforeach()

  message("output_file_paths: ${output_file_paths}")

  add_custom_target(
    ${target}
    SOURCES ${arg_SOURCES}
    DEPENDS ${output_file_paths})

endfunction()
