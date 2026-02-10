cmake_minimum_required(VERSION 3.13.4)
project(ParaCL LANGUAGES CXX)

# Включаем тестирование
enable_testing()

# Настройки компилятора
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Предупреждения
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# ========== FLEX & BISON ==========
find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)

# Генерация парсера
bison_target(parser
    source/grammar/parser.y
    ${CMAKE_CURRENT_BINARY_DIR}/parser.tab.cpp
    COMPILE_FLAGS "-d"  # Генерировать .hpp файл
)

# Генерация лексера
flex_target(scanner
    source/grammar/lexer.l
    ${CMAKE_CURRENT_BINARY_DIR}/lexer.cpp
)

# Зависимость лексера от парсера
add_flex_bison_dependency(scanner parser)

# ========== OBJECT БИБЛИОТЕКА (общий код) ==========
add_library(paracl_objects OBJECT
    ${BISON_parser_OUTPUTS}    # parser.tab.cpp
    ${FLEX_scanner_OUTPUTS}    # lexer.cpp
)

# ВАЖНО: include директории
target_include_directories(paracl_objects PUBLIC
    ${CMAKE_CURRENT_BINARY_DIR}  # Для parser.tab.hpp
)

# ========== ОСНОВНАЯ ПРОГРАММА ==========
add_executable(paracl
    source/main.cpp
    $<TARGET_OBJECTS:paracl_objects>
)

# Можно задать имя
set_target_properties(paracl PROPERTIES OUTPUT_NAME "ParaCL")

# ========== ТЕСТЫ ==========
# ВАЖНО: Ищем GTest ДО создания тестового таргета
find_package(GTest REQUIRED)

if(GTest_FOUND)
    message(STATUS "Google Test found: ${GTest_VERSION}")

    # Тестовый исполняемый файл
    add_executable(paracl_tests
        tests/parser_tests.cpp
        $<TARGET_OBJECTS:paracl_objects>  # Те же объектные файлы
    )

    # ВАЖНО: правильная линковка с GTest
    target_link_libraries(paracl_tests PRIVATE
        GTest::gtest
        GTest::gtest_main
    )

    # Include директории для тестов
    target_include_directories(paracl_tests PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_SOURCE_DIR}/source
    )

    # Добавляем в CTest
    include(GoogleTest)
    gtest_discover_tests(paracl_tests)

else()
    message(WARNING "GTest not found, skipping tests")
endif()
