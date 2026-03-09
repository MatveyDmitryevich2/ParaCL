
add_library(pcl_visitor STATIC
    source/semantic_visitor.cpp

)

target_include_directories(pcl_visitor
    PUBLIC
        include
)

target_link_libraries(pcl_visitor
    PUBLIC
        pcl-defaults
        pcl_ast
        pcl_error
)

