add_library(gameserver GameServer.cpp)

set_target_properties(gameserver
    PROPERTIES 
    CXX_STANDARD 20
    CMAKE_C_COMPILER clang
    CMAKE_CXX_COMPILER clang++
)

target_include_directories(gameserver
PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(gameserver
PRIVATE
    gamedata
    gamerules
    gamestate
    jsonparser
PUBLIC
    serverconfig
    networking
    user
    glog::glog
)