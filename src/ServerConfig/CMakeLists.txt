add_library(serverconfig ServerConfig.cpp)

set_target_properties(serverconfig
  PROPERTIES 
    CXX_STANDARD 20
    CMAKE_C_COMPILER clang
    CMAKE_CXX_COMPILER clang++
)

target_include_directories(serverconfig
PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries( serverconfig
  PUBLIC
    jsonparser
  PRIVATE
    glog::glog
)
