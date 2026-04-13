function (badger_engine_target_pedantic_diagnostic TARGET)
    if (ENABLE_BADGER_ENGINE_PEDANTIC_DIAGNOSTIC)
        if (MSVC)
            target_compile_options(
                ${TARGET}
                PRIVATE /Wall
                        /WX
                        /wd4464
                        /wd4820
                        /wd4514
                        /wd5045
                        /wd4996
                        /wd4191
                        /wd4355
                        /wd4623
                        /wd5204
                        /wd4868
                        /wd5246
                        /wd4625
                        /wd4626
                        /wd4686
                        /wd5027
                        /wd4711
                        /wd4710)
        elseif ((CMAKE_CXX_COMPILER_ID STREQUAL "Clang") OR (CMAKE_CXX_COMPILER_ID STREQUAL "GNU"))
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

        if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
            target_compile_options(
                ${TARGET}
                PRIVATE -Weverything
                        -Wno-c++98-compat
                        -Wno-c++98-compat-pedantic
                        -Wno-padded
                        -Wno-switch-default
                        -Wno-unsafe-buffer-usage
                        -Wno-exit-time-destructors
                        -Wno-global-constructors
                        -Wno-weak-vtables
                        -Wno-inconsistent-missing-destructor-override
                        -Wno-suggest-destructor-override
                        -Wno-ctad-maybe-unsupported
                        -Wno-cast-function-type-strict)
        endif ()
    endif ()
endfunction ()
