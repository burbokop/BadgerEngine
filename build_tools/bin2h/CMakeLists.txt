cmake_minimum_required(VERSION 3.27.2)

project(BadgerEngineBin2H LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(bin2h bin2h.cpp)
