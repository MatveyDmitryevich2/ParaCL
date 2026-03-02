enable_testing()

add_executable(pcl_tests)

find_package(GTest REQUIRED)

target_sources(pcl_tests
    PRIVATE
        interpreter_tests.cpp
        parser_tests.cpp
)

target_link_libraries(pcl_tests
    PRIVATE
        pcl_parser
        pcl-defaults
        pcl_ast
        pcl_interpreter
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(pcl_tests)