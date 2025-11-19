cmake_minimum_required(VERSION 3.13.4)
project(ParaCL)

add_subdirectory(flags)

find_package(FLEX REQUIRED)

flex_target(scanner
    source/grammar/lexer.l
    ${CMAKE_CURRENT_BINARY_DIR}/lexer.cc
)

add_executable(${PROJECT_NAME} 
    source/main.cpp
    ${FLEX_scanner_OUTPUTS}
)

target_include_directories(${PROJECT_NAME} 
    PRIVATE 
        ${CMAKE_CURRENT_BINARY_DIR}
)

target_include_directories(${PROJECT_NAME} 
    PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}
)