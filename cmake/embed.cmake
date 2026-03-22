function (badger_engine_add_embedded_target target)
    cmake_parse_arguments(
        PARSE_ARGV
        1
        arg
        ""
        "OUTPUT_DIR;SUBDIR"
        "SOURCES")

    if (NOT DEFINED arg_OUTPUT_DIR)
        set(arg_OUTPUT_DIR
            "${CMAKE_CURRENT_BINARY_DIR}/badger_engine_add_embedded_target_output/${target}")
    endif ()

    if (DEFINED arg_SUBDIR)
        get_filename_component(output_dir ${arg_OUTPUT_DIR}/${arg_SUBDIR} ABSOLUTE)
    else ()
        set(output_dir ${arg_OUTPUT_DIR})
    endif ()

    set(output_file_paths)
    foreach (source_file_path ${arg_SOURCES})

        get_filename_component(source_file_name ${source_file_path} NAME)
        get_filename_component(output_file_path ${output_dir}/${source_file_name}.h ABSOLUTE)
        get_filename_component(output_file_path_cpp ${output_dir}/${source_file_name}.cpp ABSOLUTE)

        string(REPLACE "." "_" variable_file_name ${source_file_name})

        if (NOT DEFINED BIN2H_EXECUTABLE)
            message(FATAL_ERROR "BIN2H_EXECUTABLE is not set")
        endif ()

        add_custom_command(
            DEPENDS ${source_file_path}
            COMMAND ${BIN2H_EXECUTABLE} ${source_file_path} --name ${variable_file_name} --extern
                    --pragma-once > ${output_file_path}
            OUTPUT ${output_file_path})

        add_custom_command(
            DEPENDS ${source_file_path}
            COMMAND ${BIN2H_EXECUTABLE} ${source_file_path} --name ${variable_file_name} --include
                    ${output_file_path} > ${output_file_path_cpp}
            OUTPUT ${output_file_path_cpp})

        list(APPEND output_file_paths ${output_file_path} ${output_file_path_cpp})
    endforeach ()

    add_library(${target}_impl SHARED ${output_file_paths})

    add_library(${target} INTERFACE)
    target_sources(${target} PRIVATE ${arg_SOURCES})

    # add_dependencies()
    target_link_libraries(${target} INTERFACE ${target}_impl)
    target_include_directories(${target} INTERFACE ${arg_OUTPUT_DIR})
endfunction ()
