add_library(gamestate
  GameState.cpp
)

target_include_directories(gamestate
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(gamestate
PRIVATE
  glog::glog
)

set_target_properties(gamestate
  PROPERTIES
    LINKER_LANGUAGE CXX
    CXX_STANDARD 20
    CMAKE_C_COMPILER clang
    CMAKE_CXX_COMPILER clang++
)
