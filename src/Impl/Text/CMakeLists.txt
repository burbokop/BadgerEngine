cmake_minimum_required(VERSION 3.27.2)

set(INSTALLDIR ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}/Impl/Text)

target_sources(BadgerEngine PRIVATE # cmake-format: sort
                                    Font.cpp Font.h Typeface.cpp Typeface.h)

if (ENABLE_BADGER_ENGINE_FREETYPE)
    target_sources(BadgerEngine PRIVATE # cmake-format: sort
                                        TypefaceImpl.h)
endif ()
