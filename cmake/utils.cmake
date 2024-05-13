function(badger_engine_target_discover_tests TARGET)
  get_target_property(TARGET_SOURCES ${TARGET} SOURCES)

  foreach(SOURCE_FILE ${TARGET_SOURCES})
    file(READ ${SOURCE_FILE} FILE_CONTENT)
    string(REGEX REPLACE ";" "\\\\;" FILE_CONTENT "${FILE_CONTENT}")
    string(REGEX REPLACE "\n" ";" FILE_CONTENT "${FILE_CONTENT}")

    foreach(LINE ${FILE_CONTENT})
      string(REGEX MATCH "badger_engine_test\\((.*),[ ]*(.*)\\)" _ ${LINE})
      if(CMAKE_MATCH_1 AND CMAKE_MATCH_2)
        set(TEST_NAME "${CMAKE_MATCH_1}.${CMAKE_MATCH_2}")
        add_test(
          NAME "${TARGET}.${TEST_NAME}"
          COMMAND ${TARGET} one ${TEST_NAME}
          WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
      endif()
    endforeach()
  endforeach()
endfunction()

function(badger_engine_lint_target TARGET)
  get_target_property(TARGET_SOURCES ${TARGET} SOURCES)
  foreach(SOURCE_FILE ${TARGET_SOURCES})
    add_custom_command(
      TARGET ${TARGET}
      POST_BUILD
      COMMAND cpplint --output=eclipse ${SOURCE_FILE}
      COMMENT "Linting ${SOURCE_FILE}"
      VERBATIM)
  endforeach()
endfunction()

function(badger_engine_copy_target TARGET DST_DIR)
  get_target_property(TARGET_SOURCES ${TARGET} SOURCES)
  get_target_property(SOURCE_DIR ${TARGET} SOURCE_DIR)

  foreach(SRC_FILE_PATH ${TARGET_SOURCES})
    file(RELATIVE_PATH REL_SRC_PATH ${SOURCE_DIR} ${SRC_FILE_PATH})
    file(REAL_PATH ${REL_SRC_PATH} DST_FILE_PATH BASE_DIRECTORY ${DST_DIR})

    add_custom_command(
      TARGET ${TARGET}
      POST_BUILD
      BYPRODUCTS "${DST_FILE_PATH}"
      COMMAND cmake -E copy "${SRC_FILE_PATH}" "${DST_FILE_PATH}" DEPENDS
              "${SRC_FILE_PATH}"
      COMMENT "Copiyng ${SRC_FILE_PATH} -> ${DST_FILE_PATH}"
      VERBATIM)
  endforeach()
endfunction()

function(badger_engine_add_ext_proj_source_target TARGET EXT_PROJ_NAME)
  ExternalProject_Get_Property(${EXT_PROJ_NAME} SOURCE_DIR)
  file(GLOB_RECURSE SOURCE_FILES "${SOURCE_DIR}/*.cpp" "${SOURCE_DIR}/*.h")
  add_custom_target(${TARGET} ALL SOURCES ${SOURCE_FILES})
endfunction()

function(badger_engine_target_build_inteface_headers RESULT TARGET)
  get_target_property(TARGET_SOURCES ${TARGET} SOURCES)
  foreach(SRC_FILE_PATH ${TARGET_SOURCES})
    string(REGEX MATCH "\\$<BUILD_INTERFACE:(.*)>" _ ${SRC_FILE_PATH})
    if(CMAKE_MATCH_1)
      list(APPEND RESULT_LST ${CMAKE_MATCH_1})
    endif()
  endforeach()
  set(${RESULT}
      ${RESULT_LST}
      PARENT_SCOPE)
endfunction()

function(badger_engine_install_public_headers TARGET SOURCE_DIR DST_DIR)
  badger_engine_target_build_inteface_headers(TARGET_SOURCES ${TARGET})

  foreach(SRC_FILE_PATH ${TARGET_SOURCES})
    if(IS_ABSOLUTE ${SRC_FILE_PATH})
      get_filename_component(FILE_EXT ${SRC_FILE_PATH} EXT)
      file(RELATIVE_PATH REL_SRC_PATH ${SOURCE_DIR} ${SRC_FILE_PATH})
    else()
      file(REAL_PATH ${SRC_FILE_PATH} SRC_REAL_FILE_PATH BASE_DIRECTORY
           ${SOURCE_DIR})

      get_filename_component(FILE_EXT ${SRC_REAL_FILE_PATH} EXT)
      set(REL_SRC_PATH ${SRC_FILE_PATH})
    endif()
    file(REAL_PATH ${REL_SRC_PATH} DST_FILE_PATH BASE_DIRECTORY ${DST_DIR})

    if((${FILE_EXT} STREQUAL ".h") OR (${FILE_EXT} STREQUAL ".hpp"))
      get_filename_component(FILE_DIR ${DST_FILE_PATH} DIRECTORY)
      file(MAKE_DIRECTORY ${FILE_DIR})
      file(WRITE ${DST_FILE_PATH} "#include <${SRC_FILE_PATH}>")
    endif()
  endforeach()
endfunction()

function(badger_engine_add_resources OUTPUT_VAR)
  set(result)
  foreach(in_f ${ARGN})

    cmake_path(IS_PREFIX CMAKE_CURRENT_SOURCE_DIR ${in_f} NORMALIZE
               IS_IN_CURRENT_SOURCE_DIR)
    cmake_path(IS_PREFIX CMAKE_CURRENT_BINARY_DIR ${in_f} NORMALIZE
               IS_IN_CURRENT_BINARY_DIR)
    if(IS_IN_CURRENT_SOURCE_DIR)
      set(REL_DIR ${CMAKE_CURRENT_SOURCE_DIR})
    elseif(IS_IN_CURRENT_BINARY_DIR)
      set(REL_DIR ${CMAKE_CURRENT_BINARY_DIR})
    else()
      message(
        FATAL_ERROR
          "Resource file must be containing currnt source or binary dir" ...)
    endif()

    file(RELATIVE_PATH rel ${REL_DIR} ${in_f})
    set(out_f "${CMAKE_CURRENT_BINARY_DIR}/${rel}.o")
    get_filename_component(out_dir ${out_f} DIRECTORY)
    file(MAKE_DIRECTORY ${out_dir})
    add_custom_command(
      OUTPUT ${out_f}
      COMMAND ld -r -b binary -o "${out_f}" "${rel}"
      DEPENDS ${in_f}
      WORKING_DIRECTORY ${REL_DIR}
      COMMENT "Packing resource ${in_f} -> ${out_f}"
      VERBATIM)
    list(APPEND result ${out_f})
  endforeach()
  set(${OUTPUT_VAR}
      "${result}"
      PARENT_SCOPE)
endfunction()
