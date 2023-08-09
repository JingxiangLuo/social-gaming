add_library(user User.cpp)

set_target_properties(user
    PROPERTIES 
    CXX_STANDARD 20
    CMAKE_C_COMPILER clang
    CMAKE_CXX_COMPILER clang++
)

target_include_directories(user
PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(user
    networking
    serverconfig
)
