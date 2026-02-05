cmake_minimum_required(VERSION 3.13.4)
project(ParaCL)

add_subdirectory(flags)

find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)


bison_target(parser
    source/grammar/parser.y
    ${CMAKE_CURRENT_BINARY_DIR}/parser.tab.cpp

)


flex_target(scanner
    source/grammar/lexer.l
    ${CMAKE_CURRENT_BINARY_DIR}/lexer.cpp
)


add_executable(${PROJECT_NAME}
    source/main.cpp
    ${BISON_parser_OUTPUTS}
    ${FLEX_scanner_OUTPUTS}
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_CXX_STANDARD 14)
