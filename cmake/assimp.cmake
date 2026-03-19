
ExternalProject_Add(
    AssimpProject
    GIT_REPOSITORY "https://github.com/assimp/assimp.git"
    GIT_TAG "v6.0.4"
    GIT_SHALLOW ON
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
)

ExternalProject_Get_property(AssimpProject INSTALL_DIR)

message("assimp INSTALL_DIR: ${INSTALL_DIR}")

set(ASSIMP_THIRDPARTY_INCLUDE_DIR ${INSTALL_DIR}/include)
set(ASSIMP_THIRDPARTY_LIBRARY_DIR ${INSTALL_DIR}/lib)

file(MAKE_DIRECTORY ${ASSIMP_THIRDPARTY_INCLUDE_DIR})
file(MAKE_DIRECTORY ${ASSIMP_THIRDPARTY_LIBRARY_DIR})

add_library(assimp::assimp INTERFACE IMPORTED)
add_dependencies(assimp::assimp AssimpProject)

target_include_directories(assimp::assimp INTERFACE ${ASSIMP_THIRDPARTY_INCLUDE_DIR})
target_link_directories(assimp::assimp INTERFACE ${ASSIMP_THIRDPARTY_LIBRARY_DIR})
target_link_libraries(assimp::assimp INTERFACE assimp)
