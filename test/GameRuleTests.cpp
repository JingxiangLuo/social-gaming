#include "gtest/gtest.h"
#include <glog/logging.h>
#include "GameData.h"
#include "GameRules.h"
#include "GameState.h"
#include "JsonParser.h"
#include <string>
#include <vector>

using namespace testing;

/////////////////////////////////////////////////////////////////////////////
// GameRule Tests
/////////////////////////////////////////////////////////////////////////////
TEST(GameRuleTests, basicAddTest) {
  // Arrange
  const std::string GAME_SPEC_PATH = "../social-gaming/test/json/gameSpec_addGlblMsg_basic.json";
  const bool EXPECTED_PARSE_VALIDITY = true;
  const bool EXPECTED_DEBUG_TARGET_VALIDITY = true;
  const GameState::VariableValue EXPECTED_DEBUG_TARGET_VALUE = 1;
         
  // Act (Game Load)
  const JsonParser::JsonParser parser = JsonParser::JsonParser();
  const GameData::GameData gameData = parser.parseJsonFile_gameSpec(GAME_SPEC_PATH);

  // Assert (Game Load)
  EXPECT_EQ(EXPECTED_PARSE_VALIDITY, gameData.isValid);

  // Act (Game Execute)
  GameState::GameState gameState = GameState::GameState(gameData.variableMap,
                                                        {},
                                                        gameData.perPlayerVariableMap);
  for (const auto& rule : gameData.topLevelRules) {
    ASSERT_EQ(rule->executeRule(gameState), GameRules::RuleExecutionResult::SUCCESS);
  }

  // Assert (Game Execute)
  GameState::GetVariableResult result = gameState.getValue("debug_target");
  EXPECT_EQ(EXPECTED_DEBUG_TARGET_VALIDITY, result.wasSuccessful);
  EXPECT_EQ(EXPECTED_DEBUG_TARGET_VALUE, result.value);
}

TEST(GameRuleTests, manyAddGlobalMsgRules) {
  // Arrange
  const std::string GAME_SPEC_PATH = "../social-gaming/test/json/gameSpec_addGlblMsg_advanced.json";
  const bool EXPECTED_PARSE_VALIDITY = true;
  const bool EXPECTED_GET_VARIABLE_VALIDITY = true;
  const GameState::VariableValue EXPECTED_DIFFERENTLY_NAMED_VARIABLE = -90;
  const GameState::VariableValue EXPECTED_ANOTHER_VARIABLE = 100;
  const GameState::VariableValue EXPECTED_LARGE_VARIABLE = 246912;
         
  // Act (Game Load)
  const JsonParser::JsonParser parser = JsonParser::JsonParser();
  const GameData::GameData gameData = parser.parseJsonFile_gameSpec(GAME_SPEC_PATH);

  // Assert (Game Load)
  EXPECT_EQ(EXPECTED_PARSE_VALIDITY, gameData.isValid);

  // Act (Game Execute)
  GameState::GameState gameState = GameState::GameState(gameData.variableMap,
                                                        {},
                                                        gameData.perPlayerVariableMap);
  for (const auto& rule : gameData.topLevelRules) {
    ASSERT_EQ(rule->executeRule(gameState), GameRules::RuleExecutionResult::SUCCESS);
  }

  // Assert (Game Execute)
  GameState::GetVariableResult result1 = gameState.getValue("differently_named_variable");
  GameState::GetVariableResult result2 = gameState.getValue("another_variable");
  GameState::GetVariableResult result3 = gameState.getValue("largeVariable");
  EXPECT_EQ(EXPECTED_GET_VARIABLE_VALIDITY, result1.wasSuccessful);
  EXPECT_EQ(EXPECTED_GET_VARIABLE_VALIDITY, result2.wasSuccessful);
  EXPECT_EQ(EXPECTED_GET_VARIABLE_VALIDITY, result3.wasSuccessful);
  EXPECT_EQ(EXPECTED_DIFFERENTLY_NAMED_VARIABLE, result1.value);
  EXPECT_EQ(EXPECTED_ANOTHER_VARIABLE, result2.value);
  EXPECT_EQ(EXPECTED_LARGE_VARIABLE, result3.value);
}

TEST(GameRuleTests, invalidAddGlobalMsgRules) {
  // Arrange
  const std::string GAME_SPEC_PATH = "../social-gaming/test/json/gameSpec_addGlblMsg_invalid.json";
  const bool EXPECTED_PARSE_VALIDITY = false;
         
  // Act (Game Load)
  const JsonParser::JsonParser parser = JsonParser::JsonParser();
  const GameData::GameData gameData = parser.parseJsonFile_gameSpec(GAME_SPEC_PATH);

  // Assert (Game Load)
  EXPECT_EQ(EXPECTED_PARSE_VALIDITY, gameData.isValid);
}

// TODO-#61: Add more tests for various ways you could create an invalid json file using add+glblmsg

TEST(GameRuleTests, basicForEach) {
  // Arrange
  const std::string GAME_SPEC_PATH = "../social-gaming/test/json/gameSpec_forEach_basic.json";
  const GameState::PlayerIDList playerIDs = {123, 456, 789};
  const bool EXPECTED_PARSE_VALIDITY = true;
  const bool EXPECTED_GET_VALIDITY = true;
  const GameState::VariableValue EXPECTED_ORIG_DEBUG_TARGET_VALUE = 0;
  const GameState::VariableValue EXPECTED_NEW_DEBUG_TARGET_VALUE = 5;
  const GameState::VariableValue EXPECTED_ORIG_PERPLAYER_VALUE = 0;
  const GameState::VariableValue EXPECTED_NEW_PERPLAYER_VALUE = 1;
         
  // Act (Game Load)
  const JsonParser::JsonParser parser = JsonParser::JsonParser();
  const GameData::GameData gameData = parser.parseJsonFile_gameSpec(GAME_SPEC_PATH);
  GameState::GameState gameState = GameState::GameState(gameData.variableMap,
                                                        playerIDs,
                                                        gameData.perPlayerVariableMap);

  // Assert (Game Load)
  EXPECT_EQ(EXPECTED_PARSE_VALIDITY, gameData.isValid);
  GameState::GetVariableResult origGetResult = gameState.getValue("debug_target");
  EXPECT_EQ(EXPECTED_GET_VALIDITY, origGetResult.wasSuccessful);
  EXPECT_EQ(EXPECTED_ORIG_DEBUG_TARGET_VALUE, origGetResult.value);
  for (const GameState::PlayerID& playerID : playerIDs) {
    GameState::GetVariableResult getResult = gameState.getValue("players." + std::to_string(playerID) + ".input");
    EXPECT_EQ(EXPECTED_GET_VALIDITY, getResult.wasSuccessful);
    EXPECT_EQ(EXPECTED_ORIG_PERPLAYER_VALUE, getResult.value);
  }

  // Act (Game Execute)
  for (const auto& rule : gameData.topLevelRules) {
    ASSERT_EQ(rule->executeRule(gameState), GameRules::RuleExecutionResult::SUCCESS);
  }

  // Assert (Game Execute)
  GameState::GetVariableResult newGetResult = gameState.getValue("debug_target");
  EXPECT_EQ(EXPECTED_GET_VALIDITY, newGetResult.wasSuccessful);
  EXPECT_EQ(EXPECTED_NEW_DEBUG_TARGET_VALUE, newGetResult.value);
  for (const GameState::PlayerID& playerID : playerIDs) {
    GameState::GetVariableResult getResult = gameState.getValue("players." + std::to_string(playerID) + ".input");
    EXPECT_EQ(EXPECTED_GET_VALIDITY, getResult.wasSuccessful);
    EXPECT_EQ(EXPECTED_NEW_PERPLAYER_VALUE, getResult.value);
  }
}

TEST(GameRuleTests, basicForEach_differentElemName) {
  // Arrange
  const std::string GAME_SPEC_PATH = "../social-gaming/test/json/gameSpec_forEach_differentElemName.json";
  const GameState::PlayerIDList playerIDs = {123, 456, 789};
  const bool EXPECTED_PARSE_VALIDITY = true;
  const bool EXPECTED_GET_VALIDITY = true;
  const GameState::VariableValue EXPECTED_ORIG_DEBUG_TARGET_VALUE = 0;
  const GameState::VariableValue EXPECTED_NEW_DEBUG_TARGET_VALUE = 5;
  const GameState::VariableValue EXPECTED_ORIG_PERPLAYER_VALUE = 100;
  const GameState::VariableValue EXPECTED_NEW_PERPLAYER_VALUE = 101;
         
  // Act (Game Load)
  const JsonParser::JsonParser parser = JsonParser::JsonParser();
  const GameData::GameData gameData = parser.parseJsonFile_gameSpec(GAME_SPEC_PATH);
  GameState::GameState gameState = GameState::GameState(gameData.variableMap,
                                                        playerIDs,
                                                        gameData.perPlayerVariableMap);

  // Assert (Game Load)
  EXPECT_EQ(EXPECTED_PARSE_VALIDITY, gameData.isValid);
  GameState::GetVariableResult origGetResult = gameState.getValue("debug_target");
  EXPECT_EQ(EXPECTED_GET_VALIDITY, origGetResult.wasSuccessful);
  EXPECT_EQ(EXPECTED_ORIG_DEBUG_TARGET_VALUE, origGetResult.value);
  for (const GameState::PlayerID& playerID : playerIDs) {
    GameState::GetVariableResult getResult = gameState.getValue("players." + std::to_string(playerID) + ".Money");
    EXPECT_EQ(EXPECTED_GET_VALIDITY, getResult.wasSuccessful);
    EXPECT_EQ(EXPECTED_ORIG_PERPLAYER_VALUE, getResult.value);
  }

  // Act (Game Execute)
  for (const auto& rule : gameData.topLevelRules) {
    ASSERT_EQ(rule->executeRule(gameState), GameRules::RuleExecutionResult::SUCCESS);
  }

  // Assert (Game Execute)
  GameState::GetVariableResult newGetResult = gameState.getValue("debug_target");
  EXPECT_EQ(EXPECTED_GET_VALIDITY, newGetResult.wasSuccessful);
  EXPECT_EQ(EXPECTED_NEW_DEBUG_TARGET_VALUE, newGetResult.value);
  for (const GameState::PlayerID& playerID : playerIDs) {
    GameState::GetVariableResult getResult = gameState.getValue("players." + std::to_string(playerID) + ".Money");
    EXPECT_EQ(EXPECTED_GET_VALIDITY, getResult.wasSuccessful);
    EXPECT_EQ(EXPECTED_NEW_PERPLAYER_VALUE, getResult.value);
  }
}
