cmake_minimum_required(VERSION 2.8)
project(SOA)

SET(CMAKE_MODULE_PATH ${SOA_SOURCE_DIR}/cmake)

set(CMAKE_CXX_FLAGS "-Wno-deprecated-declarations -std=c++11")

add_subdirectory(examples)
