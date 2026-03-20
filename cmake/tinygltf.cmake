message("CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")

set(TINYGLTF_CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
set(TINYGLTF_CMAKE_C_FLAGS ${CMAKE_C_FLAGS})

ExternalProject_Add(
    TinyGLTFProject
    GIT_REPOSITORY "https://github.com/syoyo/tinygltf.git"
    GIT_TAG "v2.9.7"
    GIT_SHALLOW ON
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR> -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
               -DCMAKE_CXX_FLAGS=${TINYGLTF_CMAKE_CXX_FLAGS} -DCMAKE_C_FLAGS=${TINYGLTF_C_FLAGS})

ExternalProject_Get_Property(TinyGLTFProject INSTALL_DIR)

message("tinygltf INSTALL_DIR: ${INSTALL_DIR}")

set(TINYGLTF_THIRDPARTY_INCLUDE_DIR ${INSTALL_DIR}/include)
set(TINYGLTF_THIRDPARTY_LIBRARY_DIR ${INSTALL_DIR}/lib)

file(MAKE_DIRECTORY ${TINYGLTF_THIRDPARTY_INCLUDE_DIR})
file(MAKE_DIRECTORY ${TINYGLTF_THIRDPARTY_LIBRARY_DIR})

add_library(tinygltf::tinygltf INTERFACE IMPORTED)
add_dependencies(tinygltf::tinygltf TinyGLTFProject)

target_include_directories(tinygltf::tinygltf INTERFACE ${TINYGLTF_THIRDPARTY_INCLUDE_DIR})
target_link_directories(tinygltf::tinygltf INTERFACE ${TINYGLTF_THIRDPARTY_LIBRARY_DIR})
target_link_libraries(tinygltf::tinygltf INTERFACE tinygltf)
