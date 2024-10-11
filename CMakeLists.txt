cmake_minimum_required(VERSION 3.30)
project(edenjson)

set(CMAKE_CXX_STANDARD 20)

add_library(edenjson STATIC src/edenjson.cxx)

target_include_directories(edenjson PUBLIC include)
target_include_directories(edenjson PUBLIC src)

add_subdirectory(test)

enable_testing()
