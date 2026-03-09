cmake_minimum_required(VERSION 3.13.4)
project(ParaCL LANGUAGES CXX)

add_subdirectory(flags)

add_subdirectory(src/error)
add_subdirectory(src/interpreter)
add_subdirectory(src/visitor)
add_subdirectory(src/ast)

add_subdirectory(src/parser)

add_executable(ParaCL
    src/main.cpp
)

target_link_libraries(ParaCL
    PRIVATE
        pcl-defaults
        pcl_ast
        pcl_interpreter
        pcl_parser
        pcl_visitor

)

add_subdirectory(tests)
