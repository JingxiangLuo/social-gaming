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
// GameState Tests
/////////////////////////////////////////////////////////////////////////////
TEST(GameStateTests, variableGet_valid) {
  // Arrange
  const std::string GAME_SPEC_PATH = "../social-gaming/test/json/gameSpec_gameStateVariables_basic.json";
  const bool EXPECTED_GET_RESULT = true;
  const GameState::VariableValue EXPECTED_ORIG_DEBUG_TARGET_VALUE = -40;
         
  // Arrange (Assembling GameState)
  const JsonParser::JsonParser parser = JsonParser::JsonParser();
  const GameData::GameData gameData = parser.parseJsonFile_gameSpec(GAME_SPEC_PATH);
  GameState::GameState gameState = GameState::GameState(gameData.variableMap,
                                                        {},
                                                        gameData.perPlayerVariableMap);

  // Act (Get)
  GameState::GetVariableResult getResult = gameState.getValue("debug_target");

  // Assert (Get)
  EXPECT_EQ(EXPECTED_GET_RESULT, getResult.wasSuccessful);
  EXPECT_EQ(EXPECTED_ORIG_DEBUG_TARGET_VALUE, getResult.value);
}

TEST(GameStateTests, variableSet_valid) {
  // Arrange
  const std::string GAME_SPEC_PATH = "../social-gaming/test/json/gameSpec_gameStateVariables_basic.json";
  const bool EXPECTED_GET_RESULT = true;
  const GameState::SetVariableResult EXPECTED_SET_RESULT = GameState::SetVariableResult::SUCCESS;
  const GameState::VariableValue EXPECTED_ORIG_DEBUG_TARGET_VALUE = -40;
  const GameState::VariableValue EXPECTED_NEW_DEBUG_TARGET_VALUE = 20;
         
  // Arrange (Assembling GameState)
  const JsonParser::JsonParser parser = JsonParser::JsonParser();
  const GameData::GameData gameData = parser.parseJsonFile_gameSpec(GAME_SPEC_PATH);
  GameState::GameState gameState = GameState::GameState(gameData.variableMap,
                                                        {},
                                                        gameData.perPlayerVariableMap);

  // Act (Get)
  GameState::GetVariableResult getResult = gameState.getValue("debug_target");

  // Assert (Get)
  EXPECT_EQ(EXPECTED_GET_RESULT, getResult.wasSuccessful);
  EXPECT_EQ(EXPECTED_ORIG_DEBUG_TARGET_VALUE, getResult.value);

  // Act (Set)
  const GameState::SetVariableResult setResult = gameState.setValue("debug_target", 20);
  getResult = gameState.getValue("debug_target");

  // Assert (Set)
  EXPECT_EQ(EXPECTED_GET_RESULT, getResult.wasSuccessful);
  EXPECT_EQ(EXPECTED_SET_RESULT, setResult);
  EXPECT_EQ(EXPECTED_NEW_DEBUG_TARGET_VALUE, getResult.value);
}

TEST(GameStateTests, getPlayers_empty) {
  // Arrange
  const std::string GAME_SPEC_PATH = "../social-gaming/test/json/gameSpec_gameStateVariables_basic.json";
  const GameState::PlayerIDList EXPECTED_PLAYER_IDS = {};
         
  // Act
  const JsonParser::JsonParser parser = JsonParser::JsonParser();
  const GameData::GameData gameData = parser.parseJsonFile_gameSpec(GAME_SPEC_PATH);
  GameState::GameState gameState = GameState::GameState(gameData.variableMap,
                                                        {},
                                                        gameData.perPlayerVariableMap);

  // Assert
  EXPECT_EQ(EXPECTED_PLAYER_IDS, gameState.getPlayerIDs());
}

TEST(GameStateTests, getPlayers_nonEmpty) {
  // Arrange
  const std::string GAME_SPEC_PATH = "../social-gaming/test/json/gameSpec_gameStateVariables_basic.json";
  const GameState::PlayerIDList EXPECTED_PLAYER_IDS = {123, 456, 789};

  // Act
  const JsonParser::JsonParser parser = JsonParser::JsonParser();
  const GameData::GameData gameData = parser.parseJsonFile_gameSpec(GAME_SPEC_PATH);
  GameState::GameState gameState = GameState::GameState(gameData.variableMap,
                                                        {123, 456, 789},
                                                        gameData.perPlayerVariableMap);

  // Assert
  // Multiset since the order of playerIDs returned from GameState is nondeterministic
  const GameState::PlayerIDList playerIDs = gameState.getPlayerIDs();
  EXPECT_EQ(std::multiset<GameState::PlayerID>(EXPECTED_PLAYER_IDS.begin(), EXPECTED_PLAYER_IDS.end()),
            std::multiset<GameState::PlayerID>(playerIDs.begin(), playerIDs.end()));
}

TEST(GameStateTests, variableGet_invalid) {
  // Arrange
  const std::string GAME_SPEC_PATH = "../social-gaming/test/json/gameSpec_gameStateVariables_basic.json";
  const bool EXPECTED_GET_RESULT = false;
         
  // Arrange (Assembling GameState)
  const JsonParser::JsonParser parser = JsonParser::JsonParser();
  const GameData::GameData gameData = parser.parseJsonFile_gameSpec(GAME_SPEC_PATH);
  GameState::GameState gameState = GameState::GameState(gameData.variableMap,
                                                        {},
                                                        gameData.perPlayerVariableMap);

  // Act
  GameState::GetVariableResult getResult = gameState.getValue("nonexistent_variable");

  // Assert
  EXPECT_EQ(EXPECTED_GET_RESULT, getResult.wasSuccessful);
}

TEST(GameStateTests, variableSet_invalid) {
  // Arrange
  const std::string GAME_SPEC_PATH = "../social-gaming/test/json/gameSpec_gameStateVariables_basic.json";;
  const GameState::SetVariableResult EXPECTED_SET_RESULT = GameState::SetVariableResult::FAILURE;
         
  // Arrange (Assembling GameState)
  const JsonParser::JsonParser parser = JsonParser::JsonParser();
  const GameData::GameData gameData = parser.parseJsonFile_gameSpec(GAME_SPEC_PATH);
  GameState::GameState gameState = GameState::GameState(gameData.variableMap,
                                                        {},
                                                        gameData.perPlayerVariableMap);

  // Act
  const GameState::SetVariableResult setResult = gameState.setValue("nonexistent_variable", 20);

  // Assert
  EXPECT_EQ(EXPECTED_SET_RESULT, setResult);
}

TEST(GameStateTests, perPlayerGet_valid) {
  // Arrange
  const std::string GAME_SPEC_PATH = "../social-gaming/test/json/gameSpec_gameStateVariables_basic.json";
  const GameState::PlayerIDList playerIDs = {123, 456, 789};
  const bool EXPECTED_GET_RESULT = true;
  const GameState::VariableValue EXPECTED_PLAYER_VALUE = 13;
         
  // Arrange (Assembling GameState)
  const JsonParser::JsonParser parser = JsonParser::JsonParser();
  const GameData::GameData gameData = parser.parseJsonFile_gameSpec(GAME_SPEC_PATH);
  GameState::GameState gameState = GameState::GameState(gameData.variableMap,
                                                        playerIDs,
                                                        gameData.perPlayerVariableMap);

  // Act&Assert for each player
  // Using raw for-each loop because otherwise we'd need to capture like 3+ variables :(
  for (const GameState::PlayerID& playerID : playerIDs) {
    GameState::GetVariableResult getResult = gameState.getValue("players." + std::to_string(playerID) + ".input");
    EXPECT_EQ(EXPECTED_GET_RESULT, getResult.wasSuccessful);
    EXPECT_EQ(EXPECTED_PLAYER_VALUE, getResult.value);
  }
}

TEST(GameStateTests, perPlayerSet_valid) {
  // Arrange
  const std::string GAME_SPEC_PATH = "../social-gaming/test/json/gameSpec_gameStateVariables_basic.json";
  const GameState::PlayerIDList playerIDs = {123, 456, 789};
  const bool EXPECTED_GET_RESULT = true;
  const GameState::SetVariableResult EXPECTED_SET_RESULT = GameState::SetVariableResult::SUCCESS;
  const GameState::VariableValue EXPECTED_ORIG_PLAYER_VALUE = 13;
  const std::vector<GameState::VariableValue> EXPECTED_VALUES = {45, 55, 10};
         
  // Arrange (Assembling GameState)
  const JsonParser::JsonParser parser = JsonParser::JsonParser();
  const GameData::GameData gameData = parser.parseJsonFile_gameSpec(GAME_SPEC_PATH);
  GameState::GameState gameState = GameState::GameState(gameData.variableMap,
                                                        playerIDs,
                                                        gameData.perPlayerVariableMap);

  // Act&Assert for each player to check that variables exist
  for (const GameState::PlayerID& playerID : playerIDs) {
    GameState::GetVariableResult getResult = gameState.getValue("players." + std::to_string(playerID) + ".input");
    EXPECT_EQ(EXPECTED_GET_RESULT, getResult.wasSuccessful);
    EXPECT_EQ(EXPECTED_ORIG_PLAYER_VALUE, getResult.value);
  }

  // Act (Set variables)
  // Raw loop since looping through two data structures at once
  for (unsigned int i = 0; i < EXPECTED_VALUES.size(); i++) {
    GameState::SetVariableResult setResult = gameState.setValue("players." + std::to_string(playerIDs.at(i)) + ".input",
                                                                EXPECTED_VALUES.at(i));
    EXPECT_EQ(EXPECTED_SET_RESULT, setResult);
  }

  // Assert (Check set results)
  // Raw loop since looping through two data structures at once
  for (unsigned int i = 0; i < EXPECTED_VALUES.size(); i++) {
    GameState::GetVariableResult getResult = gameState.getValue("players." + std::to_string(playerIDs.at(i)) + ".input");
    EXPECT_EQ(EXPECTED_GET_RESULT, getResult.wasSuccessful);
    EXPECT_EQ(EXPECTED_VALUES.at(i), getResult.value);
  }
}

TEST(GameStateTests, perPlayerGet_invalidPlayerID) {
  // Arrange
  const std::string GAME_SPEC_PATH = "../social-gaming/test/json/gameSpec_gameStateVariables_basic.json";
  const GameState::PlayerIDList playerIDs = {123, 456, 789};
  const bool EXPECTED_GET_RESULT = false;
         
  // Arrange (Assembling GameState)
  const JsonParser::JsonParser parser = JsonParser::JsonParser();
  const GameData::GameData gameData = parser.parseJsonFile_gameSpec(GAME_SPEC_PATH);
  GameState::GameState gameState = GameState::GameState(gameData.variableMap,
                                                        playerIDs,
                                                        gameData.perPlayerVariableMap);

  // Act
  GameState::GetVariableResult getResult = gameState.getValue("players.123456789.input");

  // Assert
  EXPECT_EQ(EXPECTED_GET_RESULT, getResult.wasSuccessful);
}

TEST(GameStateTests, perPlayerGet_invalidVariableName) {
  // Arrange
  const std::string GAME_SPEC_PATH = "../social-gaming/test/json/gameSpec_gameStateVariables_basic.json";
  const GameState::PlayerIDList playerIDs = {123, 456, 789};
  const bool EXPECTED_GET_RESULT = false;
         
  // Arrange (Assembling GameState)
  const JsonParser::JsonParser parser = JsonParser::JsonParser();
  const GameData::GameData gameData = parser.parseJsonFile_gameSpec(GAME_SPEC_PATH);
  GameState::GameState gameState = GameState::GameState(gameData.variableMap,
                                                        playerIDs,
                                                        gameData.perPlayerVariableMap);

  // Act
  GameState::GetVariableResult getResult = gameState.getValue("players." + std::to_string(playerIDs.at(0)) + ".nonexistent_variable");

  // Assert
  EXPECT_EQ(EXPECTED_GET_RESULT, getResult.wasSuccessful);
}

TEST(GameStateTests, perPlayerSet_invalidPlayerID) {
  // Arrange
  const std::string GAME_SPEC_PATH = "../social-gaming/test/json/gameSpec_gameStateVariables_basic.json";
  const GameState::PlayerIDList playerIDs = {123, 456, 789};
  const GameState::SetVariableResult EXPECTED_SET_RESULT = GameState::SetVariableResult::FAILURE;
         
  // Arrange (Assembling GameState)
  const JsonParser::JsonParser parser = JsonParser::JsonParser();
  const GameData::GameData gameData = parser.parseJsonFile_gameSpec(GAME_SPEC_PATH);
  GameState::GameState gameState = GameState::GameState(gameData.variableMap,
                                                        playerIDs,
                                                        gameData.perPlayerVariableMap);

  // Act
  GameState::SetVariableResult setResult = gameState.setValue("players.123456789.input", 13);

  // Assert
  EXPECT_EQ(EXPECTED_SET_RESULT, setResult);
}

TEST(GameStateTests, perPlayerSet_invalidVariableName) {
  // Arrange
  const std::string GAME_SPEC_PATH = "../social-gaming/test/json/gameSpec_gameStateVariables_basic.json";
  const GameState::PlayerIDList playerIDs = {123, 456, 789};
  const GameState::SetVariableResult EXPECTED_SET_RESULT = GameState::SetVariableResult::FAILURE;
         
  // Arrange (Assembling GameState)
  const JsonParser::JsonParser parser = JsonParser::JsonParser();
  const GameData::GameData gameData = parser.parseJsonFile_gameSpec(GAME_SPEC_PATH);
  GameState::GameState gameState = GameState::GameState(gameData.variableMap,
                                                        playerIDs,
                                                        gameData.perPlayerVariableMap);

  // Act
  GameState::SetVariableResult setResult = gameState.setValue("players." + std::to_string(playerIDs.at(0)) + ".nonexistent_variable",
                                                              13);

  // Assert
  EXPECT_EQ(EXPECTED_SET_RESULT, setResult);
}

TEST(GameStateTests, valueGet_normal_valid) {
  // Arrange
  const std::string GAME_SPEC_PATH = "../social-gaming/test/json/gameSpec_gameStateVariables_basic.json";
  const bool EXPECTED_GET_RESULT = true;
  const GameState::VariableValue EXPECTED_ORIG_DEBUG_TARGET_VALUE = -40;
         
  // Arrange (Assembling GameState)
  const JsonParser::JsonParser parser = JsonParser::JsonParser();
  const GameData::GameData gameData = parser.parseJsonFile_gameSpec(GAME_SPEC_PATH);
  GameState::GameState gameState = GameState::GameState(gameData.variableMap,
                                                        {},
                                                        gameData.perPlayerVariableMap);

  // Act (Get)
  GameState::GetVariableResult getResult = gameState.getValue("debug_target");

  // Assert (Get)
  EXPECT_EQ(EXPECTED_GET_RESULT, getResult.wasSuccessful);
  EXPECT_EQ(EXPECTED_ORIG_DEBUG_TARGET_VALUE, getResult.value);
}

TEST(GameStateTests, valueGet_perPlayer_valid) {
  // Arrange
  const std::string GAME_SPEC_PATH = "../social-gaming/test/json/gameSpec_gameStateVariables_basic.json";
  const bool EXPECTED_GET_RESULT = true;
  const GameState::VariableValue EXPECTED_ORIG_DEBUG_TARGET_VALUE = 13;
         
  // Arrange (Assembling GameState)
  const JsonParser::JsonParser parser = JsonParser::JsonParser();
  const GameData::GameData gameData = parser.parseJsonFile_gameSpec(GAME_SPEC_PATH);
  GameState::GameState gameState = GameState::GameState(gameData.variableMap,
                                                        {123, 456, 789},
                                                        gameData.perPlayerVariableMap);

  // Act&Assert (Get Player 1)
  GameState::GetVariableResult getResult = gameState.getValue("players.123.input");
  EXPECT_EQ(EXPECTED_GET_RESULT, getResult.wasSuccessful);
  EXPECT_EQ(EXPECTED_ORIG_DEBUG_TARGET_VALUE, getResult.value);

  // Act&Assert (Get Player 2)
  getResult = gameState.getValue("players.456.input");
  EXPECT_EQ(EXPECTED_GET_RESULT, getResult.wasSuccessful);
  EXPECT_EQ(EXPECTED_ORIG_DEBUG_TARGET_VALUE, getResult.value);
  
  // Act&Assert (Get Player 3)
  getResult = gameState.getValue("players.789.input");
  EXPECT_EQ(EXPECTED_GET_RESULT, getResult.wasSuccessful);
  EXPECT_EQ(EXPECTED_ORIG_DEBUG_TARGET_VALUE, getResult.value);
}

TEST(GameStateTests, valueSet_normal_valid) {
  // Arrange
  const std::string GAME_SPEC_PATH = "../social-gaming/test/json/gameSpec_gameStateVariables_basic.json";
  const bool EXPECTED_GET_RESULT = true;
  const GameState::SetVariableResult EXPECTED_SET_RESULT = GameState::SetVariableResult::SUCCESS;
  const GameState::VariableValue EXPECTED_ORIG_DEBUG_TARGET_VALUE = -40;
  const GameState::VariableValue EXPECTED_NEW_DEBUG_TARGET_VALUE = 20;
         
  // Arrange (Assembling GameState)
  const JsonParser::JsonParser parser = JsonParser::JsonParser();
  const GameData::GameData gameData = parser.parseJsonFile_gameSpec(GAME_SPEC_PATH);
  GameState::GameState gameState = GameState::GameState(gameData.variableMap,
                                                        {},
                                                        gameData.perPlayerVariableMap);

  // Act (Get)
  GameState::GetVariableResult getResult = gameState.getValue("debug_target");

  // Assert (Get)
  EXPECT_EQ(EXPECTED_GET_RESULT, getResult.wasSuccessful);
  EXPECT_EQ(EXPECTED_ORIG_DEBUG_TARGET_VALUE, getResult.value);

  // Act (Set)
  const GameState::SetVariableResult setResult = gameState.setValue("debug_target", 20);
  getResult = gameState.getValue("debug_target");

  // Assert (Set)
  EXPECT_EQ(EXPECTED_GET_RESULT, getResult.wasSuccessful);
  EXPECT_EQ(EXPECTED_SET_RESULT, setResult);
  EXPECT_EQ(EXPECTED_NEW_DEBUG_TARGET_VALUE, getResult.value);
}

TEST(GameStateTests, valueSet_perPlayer_valid) {
  // Arrange
  const std::string GAME_SPEC_PATH = "../social-gaming/test/json/gameSpec_gameStateVariables_basic.json";
  const GameState::PlayerIDList playerIDs = {123, 94195760792208, 789};
  const bool EXPECTED_GET_RESULT = true;
  const GameState::SetVariableResult EXPECTED_SET_RESULT = GameState::SetVariableResult::SUCCESS;
  const GameState::VariableValue EXPECTED_ORIG_PLAYER_VALUE = 13;
  const std::vector<GameState::VariableValue> EXPECTED_VALUES = {45, 55, 10};
         
  // Arrange (Assembling GameState)
  const JsonParser::JsonParser parser = JsonParser::JsonParser();
  const GameData::GameData gameData = parser.parseJsonFile_gameSpec(GAME_SPEC_PATH);
  GameState::GameState gameState = GameState::GameState(gameData.variableMap,
                                                        playerIDs,
                                                        gameData.perPlayerVariableMap);

  // Act&Assert for each player to check that variables exist
  for (const GameState::PlayerID& playerID : playerIDs) {
    GameState::GetVariableResult getResult = gameState.getValue("players." + std::to_string(playerID) + ".input");
    EXPECT_EQ(EXPECTED_GET_RESULT, getResult.wasSuccessful);
    EXPECT_EQ(EXPECTED_ORIG_PLAYER_VALUE, getResult.value);
  }

  // Act (Set variables)
  // Raw loop since looping through two data structures at once
  for (unsigned int i = 0; i < EXPECTED_VALUES.size(); i++) {
    GameState::SetVariableResult setResult = gameState.setValue("players." + std::to_string(playerIDs.at(i)) + ".input",
                                                                EXPECTED_VALUES.at(i));
    EXPECT_EQ(EXPECTED_SET_RESULT, setResult);
  }

  // Assert (Check set results)
  // Raw loop since looping through two data structures at once
  for (unsigned int i = 0; i < EXPECTED_VALUES.size(); i++) {
    GameState::GetVariableResult getResult = gameState.getValue("players." + std::to_string(playerIDs.at(i)) + ".input");
    EXPECT_EQ(EXPECTED_GET_RESULT, getResult.wasSuccessful);
    EXPECT_EQ(EXPECTED_VALUES.at(i), getResult.value);
  }
}
