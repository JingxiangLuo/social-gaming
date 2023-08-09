add_subdirectory(GameClient)
add_subdirectory(GameData)
add_subdirectory(GameRules)
add_subdirectory(GameServer)
add_subdirectory(GameState)
add_subdirectory(JsonParser)
add_subdirectory(ServerConfig)
add_subdirectory(User)

add_executable(
    socialgaming 
    main.cpp
)

set_target_properties(socialgaming
                      PROPERTIES
                      LINKER_LANGUAGE CXX
                      CXX_STANDARD 20
                      CMAKE_C_COMPILER clang
                      CMAKE_CXX_COMPILER clang++
)

target_link_libraries( socialgaming
    logconfig
    
    gameclient
    gameserver

    networking
    glog::glog
)
