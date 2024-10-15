find_package(Catch2 REQUIRED)

include(CTest)
include(Catch)

enable_testing()

add_executable(edenjson_test
    json_parser.cxx
    json_value.cxx

    ${EDENJSON_SOURCE_DIR}/edenjson.cxx
)

target_include_directories(edenjson_test PRIVATE ${CATCH2_SOURCE_DIR}/../include)
target_include_directories(edenjson_test PRIVATE ${EDENJSON_SOURCE_DIR})
target_include_directories(edenjson_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(edenjson_test PRIVATE edenjson Catch2::Catch2WithMain)

catch_discover_tests(edenjson_test)
