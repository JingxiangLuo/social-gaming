add_subdirectory(gtest)
enable_testing()

add_executable(runAllTests
  main.cpp
  ParserTests.cpp
  GameRuleTests.cpp
  GameStateTests.cpp
)

target_link_libraries(runAllTests
  PRIVATE
    gmock
    gtest
    gtest_main
    jsonparser
    glog::glog
    logconfig
    gamedata
    gamestate
    gamerules
)

add_test(NAME AllTests COMMAND runAllTests)

