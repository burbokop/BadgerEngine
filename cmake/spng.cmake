ExternalProject_Add(
    SPNGProject
    GIT_REPOSITORY "https://github.com/randy408/libspng.git"
    GIT_TAG "v0.7.4"
    GIT_SHALLOW ON
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
               -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
               -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
               -DBUILD_EXAMPLES=OFF
               -DSPNG_SHARED=OFF)

ExternalProject_Get_Property(SPNGProject INSTALL_DIR)

set(SPNG_THIRDPARTY_INCLUDE_DIR ${INSTALL_DIR}/include)
set(SPNG_THIRDPARTY_LIBRARY_DIR ${INSTALL_DIR}/lib)

file(MAKE_DIRECTORY ${SPNG_THIRDPARTY_INCLUDE_DIR})
file(MAKE_DIRECTORY ${SPNG_THIRDPARTY_LIBRARY_DIR})

add_library(spng::spng INTERFACE IMPORTED)
add_dependencies(spng::spng SPNGProject)

target_include_directories(spng::spng INTERFACE ${SPNG_THIRDPARTY_INCLUDE_DIR})
target_link_directories(spng::spng INTERFACE ${SPNG_THIRDPARTY_LIBRARY_DIR})
target_link_libraries(spng::spng INTERFACE spng_static)
