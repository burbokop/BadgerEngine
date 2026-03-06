file(MAKE_DIRECTORY ${BIN2H_BINARY_DIR})

execute_process(COMMAND ${CMAKE_COMMAND} -S ${BIN2H_SOURCE_DIR} -B ${BIN2H_BINARY_DIR}
                        -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER} COMMAND_ERROR_IS_FATAL ANY)

execute_process(COMMAND ${CMAKE_COMMAND} --build ${BIN2H_BINARY_DIR} COMMAND_ERROR_IS_FATAL ANY)

if(WIN32)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(BIN2H_EXECUTABLE ${CMAKE_CURRENT_BINARY_DIR}/build_tools/bin2h/Debug/bin2h.exe)
    else()
        set(BIN2H_EXECUTABLE ${CMAKE_CURRENT_BINARY_DIR}/build_tools/bin2h/Release/bin2h.exe)
    endif()
else()
    set(BIN2H_EXECUTABLE ${CMAKE_CURRENT_BINARY_DIR}/build_tools/bin2h/bin2h)
endif()
