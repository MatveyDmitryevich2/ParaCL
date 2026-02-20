cmake_minimum_required(VERSION 3.13.4)
project(ParaCL LANGUAGES CXX)

enable_testing()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic -Werror=return-type)
endif()

set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/source)
set(TESTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tests)

# ========== FLEX & BISON ==========
find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)

bison_target(parser
    ${SOURCE_DIR}/grammar/parser.y
    ${CMAKE_CURRENT_BINARY_DIR}/parser.tab.cpp
    DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/parser.tab.hpp
)

flex_target(scanner
    ${SOURCE_DIR}/grammar/lexer.l
    ${CMAKE_CURRENT_BINARY_DIR}/lexer.cpp
)

add_flex_bison_dependency(scanner parser)

set(PARA_CORE_SOURCES
    ${SOURCE_DIR}/node.cpp
    ${BISON_parser_OUTPUTS}
    ${FLEX_scanner_OUTPUTS}
)

add_library(paracl_core OBJECT ${PARA_CORE_SOURCES})

target_include_directories(paracl_core PUBLIC
    ${INCLUDE_DIR}
    ${SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)


add_executable(paracl
    ${SOURCE_DIR}/main.cpp
    $<TARGET_OBJECTS:paracl_core>
)

target_include_directories(paracl PRIVATE
    ${INCLUDE_DIR}
    ${SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

set_target_properties(paracl PROPERTIES
    OUTPUT_NAME "ParaCL"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)


find_package(GTest QUIET)

if(GTest_FOUND)
    add_executable(paracl_tests
        ${TESTS_DIR}/parser_tests.cpp
        $<TARGET_OBJECTS:paracl_core>
    )

    target_include_directories(paracl_tests PRIVATE
        ${INCLUDE_DIR}
        ${SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
    )

    target_link_libraries(paracl_tests PRIVATE
        GTest::gtest
        GTest::gtest_main
    )

    add_test(NAME paracl_unit_tests COMMAND paracl_tests)

     add_executable(paracl_interpreter_tests
        ${TESTS_DIR}/interpreter_tests.cpp
        $<TARGET_OBJECTS:paracl_core>
    )

    target_include_directories(paracl_interpreter_tests PRIVATE
        ${INCLUDE_DIR}
        ${SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
    )

    target_link_libraries(paracl_interpreter_tests PRIVATE
        GTest::gtest
        GTest::gtest_main
    )

    add_test(NAME interpreter_tests COMMAND paracl_interpreter_tests)
endif()


add_custom_target(run
    COMMAND ${CMAKE_BINARY_DIR}/bin/ParaCL
    DEPENDS paracl
    COMMENT "Run ParaCL interpreter"
    VERBATIM
)
