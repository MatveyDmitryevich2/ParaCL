add_library(pcl_error STATIC)

target_sources(pcl_error
    PRIVATE
        source/error_print.cpp
)

target_include_directories(pcl_error
    PUBLIC
        include
)

target_link_libraries(pcl_error
    PRIVATE
        pcl-defaults
)