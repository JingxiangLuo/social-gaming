# Automatically download a release as a dependency at configure time
# Explained in https://github.com/nlohmann/json#embedded-fetchcontent
include(FetchContent)
FetchContent_Declare(json
  URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz
)
FetchContent_MakeAvailable(json)

add_library(jsonparser
  JsonParser.cpp
  RuleParser.cpp
  VariableParser.cpp
)

target_include_directories(jsonparser
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(jsonparser
  PUBLIC
    gamedata
    gamerules
    gamestate

    nlohmann_json::nlohmann_json
  PRIVATE
    glog::glog
)

set_target_properties(jsonparser
  PROPERTIES
    LINKER_LANGUAGE CXX
    CXX_STANDARD 20
    CMAKE_C_COMPILER clang
    CMAKE_CXX_COMPILER clang++
)
