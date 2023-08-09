add_library(gamerules
  GameRules.cpp
)

target_include_directories(gamerules
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(gamerules
PUBLIC
  gamestate
PRIVATE
  glog::glog
)

set_target_properties(gamerules
  PROPERTIES
    LINKER_LANGUAGE CXX
    CXX_STANDARD 20
    CMAKE_C_COMPILER clang
    CMAKE_CXX_COMPILER clang++
)
