ExternalProject_Add(
    AssimpProject
    GIT_REPOSITORY "https://github.com/assimp/assimp.git"
    GIT_TAG "v6.0.4"
    GIT_SHALLOW ON
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
               -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
               -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
               -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
               -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
               -DASSIMP_BUILD_TESTS=OFF
               -DASSIMP_WARNINGS_AS_ERRORS=OFF
               -DASSIMP_INJECT_DEBUG_POSTFIX=OFF)

ExternalProject_Get_Property(AssimpProject INSTALL_DIR)

if( MSVC )
  # in order to prevent DLL hell, each of the DLLs have to be suffixed with the major version and msvc prefix
  # CMake 3.12 added a variable for this
  if(MSVC_TOOLSET_VERSION)
    set(MSVC_PREFIX "vc${MSVC_TOOLSET_VERSION}")
  else()
    if( MSVC70 OR MSVC71 )
      set(MSVC_PREFIX "vc70")
    elseif( MSVC80 )
      set(MSVC_PREFIX "vc80")
    elseif( MSVC90 )
      set(MSVC_PREFIX "vc90")
    elseif( MSVC10 )
      set(MSVC_PREFIX "vc100")
    elseif( MSVC11 )
      set(MSVC_PREFIX "vc110")
    elseif( MSVC12 )
      set(MSVC_PREFIX "vc120")
    elseif( MSVC_VERSION LESS 1910)
      set(MSVC_PREFIX "vc140")
    elseif( MSVC_VERSION LESS 1920)
      set(MSVC_PREFIX "vc141")
    elseif( MSVC_VERSION LESS 1930)
      set(MSVC_PREFIX "vc142")
    else()
      MESSAGE(WARNING "unknown msvc version ${MSVC_VERSION}")
      set(MSVC_PREFIX "vc150")
    endif()
  endif()
  set(ASSIMP_LIBRARY_SUFFIX "-${MSVC_PREFIX}-mt" CACHE STRING "the suffix for the assimp windows library")
endif()

set(ASSIMP_THIRDPARTY_INCLUDE_DIR ${INSTALL_DIR}/include)
set(ASSIMP_THIRDPARTY_LIBRARY_DIR ${INSTALL_DIR}/lib)

file(MAKE_DIRECTORY ${ASSIMP_THIRDPARTY_INCLUDE_DIR})
file(MAKE_DIRECTORY ${ASSIMP_THIRDPARTY_LIBRARY_DIR})

add_library(assimp::assimp INTERFACE IMPORTED)
add_dependencies(assimp::assimp AssimpProject)

target_include_directories(assimp::assimp INTERFACE ${ASSIMP_THIRDPARTY_INCLUDE_DIR})
target_link_directories(assimp::assimp INTERFACE ${ASSIMP_THIRDPARTY_LIBRARY_DIR})
target_link_libraries(assimp::assimp INTERFACE assimp${ASSIMP_LIBRARY_SUFFIX})
