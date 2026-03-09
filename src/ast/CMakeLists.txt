add_library(pcl_ast STATIC
    source/node.cpp
)

target_include_directories(pcl_ast
    PUBLIC
        include
        ${CMAKE_SOURCE_DIR}/src/interpreter/include
)

target_link_libraries(pcl_ast
    PUBLIC
        pcl-defaults
        pcl_error
        pcl_visitor
        pcl_interpreter
)
