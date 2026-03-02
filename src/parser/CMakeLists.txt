add_library(pcl_parser STATIC)

find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)

bison_target(parser
    parser.y
    ${CMAKE_CURRENT_BINARY_DIR}/parser.tab.cpp
    DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/parser.tab.hpp
    COMPILE_FLAGS "-Wall --color=yes -Wcex"
)

flex_target(scanner
    lexer.l
    ${CMAKE_CURRENT_BINARY_DIR}/lexer.cpp
)

add_flex_bison_dependency(scanner parser)

target_sources(pcl_parser
    PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}/parser.tab.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/lexer.cpp
)

target_include_directories(pcl_parser
    PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(pcl_parser
    pcl_ast
)