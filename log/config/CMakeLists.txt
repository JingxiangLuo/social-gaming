add_library(logconfig logconfig.cpp)

target_include_directories(logconfig
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries( logconfig
    glog::glog
)
