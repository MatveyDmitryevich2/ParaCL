add_library(pcl_interpreter INTERFACE)

target_include_directories(pcl_interpreter
    INTERFACE
        include
)

target_link_libraries(pcl_interpreter
    INTERFACE
        pcl-defaults
        pcl_ast
)