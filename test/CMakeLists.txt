find_package(Catch2 REQUIRED)

include(CTest)
include(Catch)

enable_testing()

set(SRC ${CMAKE_SOURCE_DIR}/src)

add_executable(edenjson_test
    json_parser.cxx

    ${SRC}/edenjson.cxx
)

target_include_directories(edenjson_test PRIVATE ${CATCH2_SOURCE_DIR}/include)
target_include_directories(edenjson_test PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_include_directories(edenjson_test PRIVATE ${CMAKE_SOURCE_DIR}/test)

target_link_libraries(edenjson_test PRIVATE edenjson Catch2::Catch2WithMain)

catch_discover_tests(edenjson_test)
