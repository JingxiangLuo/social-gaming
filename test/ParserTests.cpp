#include "gtest/gtest.h"
#include "JsonParser.h"
#include "GameData.h"
#include "GameRules.h"
#include "GameState.h"

using namespace testing;

/////////////////////////////////////////////////////////////////////////////
// Parser Tests
/////////////////////////////////////////////////////////////////////////////
TEST(ParserTests, parse_validGameSpecFile) {
  // Arrange
  const std::string GAME_SPEC_PATH = "../social-gaming/test/json/gameSpec_addGlblMsg_basic.json";
  const bool EXPECTED_PARSE_VALIDITY = true;
         
  // Act
  const JsonParser::JsonParser parser = JsonParser::JsonParser();
  const GameData::GameData gameData = parser.parseJsonFile_gameSpec(GAME_SPEC_PATH);

  // Assert
  // Note: Currently isValid relies on the game spec formatting within the file being right
  //       - This test passing is not just dependent on the file path being valid.
  EXPECT_EQ(EXPECTED_PARSE_VALIDITY, gameData.isValid);
}

TEST(ParserTests, parse_invalidGameSpecFile) {
  // Arrange
  const std::string INVALID_GAME_SPEC_PATH = "../social-gaming/test/json/notAFilePath.json";
  const bool EXPECTED_PARSE_VALIDITY = false;
         
  // Act
  const JsonParser::JsonParser parser = JsonParser::JsonParser();
  const GameData::GameData gameData = parser.parseJsonFile_gameSpec(INVALID_GAME_SPEC_PATH);

  // Assert
  EXPECT_EQ(EXPECTED_PARSE_VALIDITY, gameData.isValid);
}

TEST(ParserTests, parse_invalidServerConfig) {
  // Arrange
  const std::string INVALID_SERVER_CONFIG =
  R"({
    "configuration": {
      "name": "Zen Game",
      "player count": {"min": 0, "max": 0},
      "audience": false,
      "setup": { }
    },
    "constants": {},
    "variables": {},
    "per-player": {},
    "per-audience": {},
    "rules": []
  })";
  const json EXPECTED_OUTCOME = nullptr;

  // Act
  const JsonParser::JsonParser parser = JsonParser::JsonParser();
  const json result = parser.parseJsonString_serverConfig(INVALID_SERVER_CONFIG);

  // Assert
  EXPECT_EQ(EXPECTED_OUTCOME, result);
}

TEST(ParserTests, parse_validServerConfig) {
  // Arrange
  const std::string VALID_SERVER_CONFIG =
  R"({
    "port": 4000,
    "serverhtml": "../web-socket-networking/webchat.html"
  })";

  const json EXPECTED_OUTCOME = {
    {"port", 4000},
    {"serverhtml", "../web-socket-networking/webchat.html"}
  };

  // Act
  const JsonParser::JsonParser parser = JsonParser::JsonParser();
  const json result = parser.parseJsonString_serverConfig(VALID_SERVER_CONFIG);

  // Assert
  EXPECT_EQ(EXPECTED_OUTCOME, result);
}

TEST(ParserTests, parse_validServerConfigFile) {
  // Arrange
  const std::string VALID_SERVER_CONFIG_FILEPATH =
      "../social-gaming/test/json/testServerConfig.json";

  const json EXPECTED_OUTCOME = {
    {"port", 4000},
    {"serverhtml", "../web-socket-networking/webchat.html"}
  };

  // Act
  const JsonParser::JsonParser parser = JsonParser::JsonParser();
  const json result = parser.parseJsonFile_serverConfig(VALID_SERVER_CONFIG_FILEPATH);

  // Assert
  EXPECT_EQ(EXPECTED_OUTCOME, result);
}

TEST(ParserTests, parse_invalidServerConfigFile) {
  // Arrange
  const std::string INVALID_SERVER_CONFIG_FILEPATH =
      "../social-gaming/test/json/invalid.json";

  const json EXPECTED_OUTCOME = nullptr;

  // Act
  const JsonParser::JsonParser parser = JsonParser::JsonParser();
  const json result = parser.parseJsonFile_serverConfig(INVALID_SERVER_CONFIG_FILEPATH);

  // Assert
  EXPECT_EQ(EXPECTED_OUTCOME, result);
}

// TODO-#57: Fill in this unit test
TEST(ParserTests, parse_gameSpec_inputText) {
  // Arrange
  const std::string GAME_SPEC_PATH = "../social-gaming/test/json/gameSpec_inputOutput_basic.json";
  const bool EXPECTED_PARSE_VALIDITY = true;
  // const bool EXPECTED_DEBUG_TARGET_VALIDITY = true;
  // const GameState::VariableValue EXPECTED_DEBUG_TARGET_VALUE = 1;

  // Act (Game Load)
  const JsonParser::JsonParser parser = JsonParser::JsonParser();
  const GameData::GameData gameData = parser.parseJsonFile_gameSpec(GAME_SPEC_PATH);

  // Assert (Game Load)
  EXPECT_EQ(EXPECTED_PARSE_VALIDITY, gameData.isValid);

  // Act (Game Execute)
  // GameState::GameState gameState = GameState::GameState(gameData.variableMap,
                                                        // {},
                                                        // gameData.perPlayerVariableMap);
  // for (const auto& rule : gameData.topLevelRules) {
  //   ASSERT_EQ(rule->executeRule(gameState), GameRules::RuleExecutionResult::SUCCESS);
  // }

  // Assert (Game Execute)
  // GameState::GetVariableResult result = gameState.getValue("debug_target");
  // EXPECT_EQ(EXPECTED_DEBUG_TARGET_VALIDITY, result.wasSuccessful);
  // EXPECT_EQ(EXPECTED_DEBUG_TARGET_VALUE, result.value);
}
