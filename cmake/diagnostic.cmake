function (badger_engine_target_pedantic_diagnostic TARGET)
    if (ENABLE_BADGER_ENGINE_PEDANTIC_DIAGNOSTIC)
        if ((CMAKE_CXX_COMPILER_ID STREQUAL "Clang") OR (CMAKE_CXX_COMPILER_ID STREQUAL "GNU"))
            target_compile_options(
                ${TARGET}
                PRIVATE -Wall
                        -Wextra
                        -Wpedantic
                        -Wconversion
                        -Wshadow
                        -Werror
                        -Wno-error=deprecated
                        -Wno-error=deprecated-declarations)
        endif ()
    endif ()
endfunction ()
