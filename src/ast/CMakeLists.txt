add_library(pcl_ast STATIC)

target_sources(pcl_ast
    PRIVATE
        source/node.cpp
)

target_include_directories(pcl_ast
    PUBLIC
        include
)

target_link_libraries(pcl_ast
    PUBLIC
        pcl-defaults
        pcl_interpreter
        pcl_error
)