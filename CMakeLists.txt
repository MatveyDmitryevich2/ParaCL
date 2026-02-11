cmake_minimum_required(VERSION 3.13.4)
project(ParaCL LANGUAGES CXX)

# Включаем тестирование
enable_testing()

# Настройки компилятора
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Предупреждения
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic -Werror=return-type)
endif()

# ========== ПУТИ К ИСХОДНИКАМ ==========
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/source)
set(TESTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tests)

# ========== FLEX & BISON ==========
find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)

# Генерация парсера
bison_target(parser
    ${SOURCE_DIR}/grammar/parser.y
    ${CMAKE_CURRENT_BINARY_DIR}/parser.tab.cpp
    COMPILE_FLAGS "-d --defines=parser.tab.hpp"
    DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/parser.tab.hpp
)

# Генерация лексера
flex_target(scanner
    ${SOURCE_DIR}/grammar/lexer.l
    ${CMAKE_CURRENT_BINARY_DIR}/lexer.cpp
)

# Зависимость лексера от парсера
add_flex_bison_dependency(scanner parser)

# ========== ЯДРО (логика без main.cpp) ==========
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

# ========== ИНТЕРПРЕТАТОР (ядро + main) ==========
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

# ========== ТЕСТЫ (ядро + тесты, БЕЗ main.cpp) ==========
find_package(GTest QUIET)

if(GTest_FOUND)
    message(STATUS "Google Test found: ${GTest_VERSION}")

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

    # Простая регистрация тестов
    add_test(NAME paracl_unit_tests COMMAND paracl_tests)
    set_tests_properties(paracl_unit_tests PROPERTIES TIMEOUT 2)
else()
    message(WARNING "Google Test not found — tests will be skipped")
    message(WARNING "Install libgtest-dev (Debian/Ubuntu) to enable tests")
endif()

# ========== УДОБНЫЕ ЦЕЛИ ==========
add_custom_target(run
    COMMAND ${CMAKE_BINARY_DIR}/bin/ParaCL
    DEPENDS paracl
    COMMENT "Run ParaCL interpreter"
    VERBATIM
)

add_custom_command(TARGET paracl POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_COMMAND} -E echo "✅ ParaCL built successfully!"
    COMMAND ${CMAKE_COMMAND} -E echo "   Run interpreter: ./build/bin/ParaCL"
    COMMAND ${CMAKE_COMMAND} -E echo "   Run tests:       ./build/paracl_tests"
    COMMAND ${CMAKE_COMMAND} -E echo ""
)
