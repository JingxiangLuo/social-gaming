# Library with no .cpp file: https://stackoverflow.com/a/60604878/13085998
add_library(gamedata OBJECT include/GameData.h
)

target_include_directories(gamedata
PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(gamedata
  glog::glog
  gamerules
)