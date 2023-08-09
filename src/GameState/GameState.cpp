#include "GameState.h"

#include <glog/logging.h>
#include <unordered_map>
#include <string>
#include <algorithm>



namespace GameState {



/******************************************************************************
 *                   GameState Helper Functions Declarations                  *
 ******************************************************************************/
NestedVariableKey parseRuntimeVariables(const GameState& gameState,
                                        NestedVariableKey rawNestedVariableName);
std::pair<PlayerID, VariableKey>
extractPlayerVariableInfo(const GameState& gameState, NestedVariableKey nestedVariableName);



/******************************************************************************
 *                          GameState Public Methods                          *
 ******************************************************************************/
GameState::GameState(const VariableMap& variableMap,
                     const PlayerIDList& playerIDList,
                     const VariableMap& perPlayerVariableMap)
  : variableMap(variableMap)
  , activeScopeVariables({}) {
    // Initialize and construct perPlayer variables
    // Data will end up looking like:
    /*
        {    playerIDs  :    perPlayerVariableMap
          { "PLAYER_ID1": { "VAR1": 2, "VAR2": -1 } },
          { "PLAYER_ID2": { "VAR1": 2, "VAR2": -1 } },
          { "PLAYER_ID3": { "VAR1": 2, "VAR2": -1 } }
        }
    */
    this->playerVariableMaps = {};
    for (const PlayerID& playerID : playerIDList) {
      this->playerVariableMaps.insert({std::to_string(playerID), perPlayerVariableMap});
    }
}


[[nodiscard]] GetVariableResult
GameState::getValue(NestedVariableKey rawNestedVariableName) const {
  // If we are accessing players, we need to do some special parsing. So, if we are NOT...
  // ...accessing players, we can just directly access the variable's value
  // TODO: This check should change once non-player lists become supported
  if (rawNestedVariableName.find("players") == std::string::npos) {
    return getVariable(rawNestedVariableName); 
  }
  
  // Dynamically evaluate any runtime variables that could not be evaluated at JSON-parse time
  // I.e. "players.$player.input" -> "players.789.input"
  NestedVariableKey nestedVariableName = parseRuntimeVariables(*this, rawNestedVariableName);

  // Extract individual elements from nestedVariableName so we can access the variable using gameState method
  // I.e. "players.789.input" -> 789 and "input"
  const auto [playerID, perPlayerVariableName] = extractPlayerVariableInfo(*this, nestedVariableName);

  return getPlayerVariable(playerID, perPlayerVariableName);
}


[[nodiscard]] SetVariableResult
GameState::setValue(NestedVariableKey rawNestedVariableName, VariableValue newValue) {
  // If we are accessing players, we need to do some special parsing. So, if we are NOT...
  // ...accessing players, we can just directly access the variable's value
  // TODO: This check should change once non-player lists become supported
  if (rawNestedVariableName.find("players") == std::string::npos) {
    return setVariable(rawNestedVariableName, newValue); 
  }
  
  // Dynamically evaluate any runtime variables that could not be evaluated at JSON-parse time
  // I.e. "players.$player.input" -> "players.789.input"
  NestedVariableKey nestedVariableName = parseRuntimeVariables(*this, rawNestedVariableName);

  // Extract individual elements from nestedVariableName so we can access the variable using gameState method
  // I.e. "players.789.input" -> 789 and "input"
  const auto [playerID, perPlayerVariableName] = extractPlayerVariableInfo(*this, nestedVariableName);

  return setPlayerVariable(playerID, perPlayerVariableName, newValue);
}


[[nodiscard]] PlayerIDList GameState::getPlayerIDs() const {
  PlayerIDList playerIDs;
  playerIDs.reserve(this->playerVariableMaps.size());

  for (const auto& [playerID, _] : this->playerVariableMaps) {
    playerIDs.push_back(std::stoul(playerID));
  }

  return playerIDs;
}


////////////////////////////// Scope methods //////////////////////////////
[[nodiscard]] SetVariableResult
GameState::setActiveScopeVariable(VariableKey variableName, PlayerID value) {
  if (this->activeScopeVariables.contains(variableName)) {
    LOG(ERROR) << variableName << " already found in scope variable map - can't set";
    return SetVariableResult::FAILURE;
  }

  this->activeScopeVariables.insert({variableName, value});
  return SetVariableResult::SUCCESS;
}


[[nodiscard]] SetVariableResult
GameState::unsetActiveScopeVariable(VariableKey variableName) {
  if (!this->activeScopeVariables.contains(variableName)) {
    LOG(ERROR) << variableName << " not found in scope variable map - can't remove";
    return SetVariableResult::FAILURE;
  }

  this->activeScopeVariables.erase(variableName);
  return SetVariableResult::SUCCESS;
}


[[nodiscard]] GetScopedVariableResult
GameState::getActiveScopeVariable(VariableKey variableName) const {
  if (!this->activeScopeVariables.contains(variableName)) {
    LOG(ERROR) << "Could not retrieve \"" << variableName << "\" from scope variable map";
    return {};
  }
  
  return {
    .wasSuccessful = true,
    .value = this->activeScopeVariables.at(variableName),
  };
}



/******************************************************************************
 *                          GameState Private Methods                         *
 ******************************************************************************/
[[nodiscard]] GetVariableResult GameState::getVariable(VariableKey variableName) const {
  if (!this->variableMap.contains(variableName)) {
    LOG(ERROR) << "Could not retrieve \"" << variableName << "\" from gameState variable map";
    return {};
  }
  
  return {
    .wasSuccessful = true,
    .value = this->variableMap.at(variableName),
  };
}


[[nodiscard]] SetVariableResult
GameState::setVariable(VariableKey variableName, VariableValue newValue) {
  if (!this->variableMap.contains(variableName)) {
    LOG(ERROR) << variableName << " not found in gameState variable map";
    return SetVariableResult::FAILURE;
  }

  this->variableMap.erase(variableName);
  this->variableMap.insert({variableName, newValue});
  return SetVariableResult::SUCCESS;
}


[[nodiscard]] GetVariableResult
GameState::getPlayerVariable(PlayerID playerID, VariableKey variableName) const {
  if (!this->playerVariableMaps.contains(std::to_string(playerID))) {
    LOG(ERROR) << playerID << " not found in playerVariableMaps";
    return {};
  }

  const VariableMap& playerVariables = this->playerVariableMaps.at(std::to_string(playerID));
  if (!playerVariables.contains(variableName)) {
    LOG(ERROR) << variableName << " not found in " << playerID << "'s variables";
    return {};
  }

  return {
    .wasSuccessful = true,
    .value = playerVariables.at(variableName),
  };
}


[[nodiscard]] SetVariableResult
GameState::setPlayerVariable(PlayerID playerID, VariableKey variableName, VariableValue newValue) {
  if (!this->playerVariableMaps.contains(std::to_string(playerID))) {
    LOG(ERROR) << playerID << " not found in playerVariableMaps";
    return SetVariableResult::FAILURE;
  }

  VariableMap& playerVariables = this->playerVariableMaps.at(std::to_string(playerID));
  if (!playerVariables.contains(variableName)) {
    LOG(ERROR) << variableName << " not found in " << playerID << "'s variables";
    return SetVariableResult::FAILURE;
  }

  playerVariables.erase(variableName);
  playerVariables.insert({variableName, newValue});
  return SetVariableResult::SUCCESS;
}



/******************************************************************************
 *                 GameState Helper Functions Implementations                 *
 ******************************************************************************/
/**
 * Evaluates any runtime variable names dynamically as rules are executing
 * i.e. "players.$player.input" -> "players.789.input" <-- new nestedVariableName
 * It does this by constructing and returning a new nestedVariableName with the evaluated variables
 * @param rawNestedVariableName A string that potentially contains a runtime-evaluate variable
 * TODO: string manipulation is super clunky, we could benefit from another way to perform this mechanism
 * ^ TODO-#58: Refactor to address string manipulation and reduce complexity
 */
NestedVariableKey parseRuntimeVariables(const GameState& gameState,
                                        NestedVariableKey rawNestedVariableName)
{
  // If there is no $ in this nested variable, then this is a static variable which doesn't need...
  // ...runtime parsing
  if (std::find(rawNestedVariableName.begin(), rawNestedVariableName.end(), '$') == rawNestedVariableName.end()) {
    return rawNestedVariableName;
  }

  // From here, we find any $VARIABLE instances within rawNestedVariableName...
  // ...and convert instances of $VARIABLE with the runtime value of $VARIABLE
  // I.e. "players.$player.input" -> "players.789.input"
  // TODO-#53b: string manipulation is super clunky, we need another way to carry this information
  NestedVariableKey newNestedVariableName = "";

  // Identify the start of the first variable with a $
  // i.e. "players.$player.input"
  //               ^
  auto scopedVarStart_it = std::find(rawNestedVariableName.begin(), rawNestedVariableName.end(), '$');

  // Start construction of the new parsed variable name by adding anything before the $
  // i.e. newVariable: "players."
  newNestedVariableName.append(std::string(rawNestedVariableName.begin(), scopedVarStart_it));

  // This loop will continue parsing and creating the new variable name by doing these steps:
  // 1. Select the runtime variable by selecting the start and end
  //    i.e. "players.$player.input"
  //                  ^^^^^^^
  // 2. Parse it into a runtime variable
  //    i.e.   "player" -> 789
  // 3. Add it to the newVariable string
  //    i.e.  "players.789"
  // 4. Add anything between this runtime variable and the next runtime variable into the newVariable
  //    i.e. "players.789.input" (in this case, no other $ exists, so we add contents up until end)
  while (scopedVarStart_it != rawNestedVariableName.end()) {
    // Step 1: Identify runtime variable end boundary and extracting the runtime variable name
    const auto scopedVarEnd_it = std::find(scopedVarStart_it, rawNestedVariableName.end(), '.');
    VariableKey scopedVariableName = std::string(scopedVarStart_it + 1, scopedVarEnd_it);

    // Step 2: Parsing the runtime variable into the runtime value for that variable
    //         - This hooks into values set by rule execution to dynamically get the scoped value
    GetScopedVariableResult scopedVariableResult = gameState.getActiveScopeVariable(scopedVariableName);
    if (scopedVariableResult.wasSuccessful == false) {
      LOG(ERROR) << scopedVariableName << " was not found in active scoped variables";
      return "";
    }

    // Step 3: Continue constructing new variable name using the parsed value
    newNestedVariableName.append(std::to_string(scopedVariableResult.value));

    // Step 1 (again): Identify start of next runtime variable
    scopedVarStart_it = std::find(scopedVarEnd_it, rawNestedVariableName.end(), '$');

    // Step 4: Add variable name info between previous and next runtime variable to the newVariable
    //         -> This is important as we don't want to forget non-runtime variable info
    newNestedVariableName.append(std::string(scopedVarEnd_it, scopedVarStart_it));
  }

  // We've finished parsing the runtime variables, we can return the new string
  // i.e. "players.789.input"
  return newNestedVariableName;
}


/**
 * Extract the identifiers needed to access the perPlayer variable map
 * @param nestedVariableName A variable name containing perPlayer variable info
 *                           - i.e.: "players.789.input" 
 * @return The two individual identifiers required for perPlayer variable access
 *         - i.e.: 789 and "input"
 * TODO: string manipulation is super clunky, we could benefit from another way to perform this mechanism
 * ^ TODO-#58: Refactor to address string manipulation and reduce complexity
 */
std::pair<PlayerID, VariableKey>
extractPlayerVariableInfo(const GameState& gameState, NestedVariableKey nestedVariableName) {
  // TODO: Due to this project being in early stages, we currently know that the only list
  //       we can search through is "players"
  //       For that reason, we only deal with the case of a player list access such as:
  //       "players.789.input"
  //       More general lists are not handled
  
  // Step1: Extract the playerID
  //        i.e. "players.789.input" -> "789"
  auto playerIDStart_it = std::find(nestedVariableName.begin(), nestedVariableName.end(), '.');
  playerIDStart_it++;
  auto playerIDEnd_it = std::find(playerIDStart_it, nestedVariableName.end(), '.');
  PlayerID playerID = std::stoul(std::string(playerIDStart_it, playerIDEnd_it));

  // Step2: Extract the perPlayer variable name
  //        i.e. "players.789.input" -> "input"
  //                      ^  ^
  // curr iters:       start end
  auto perPlayerVariableStart_it = playerIDEnd_it + 1;
  auto perPlayerVariableEnd_it = nestedVariableName.end();
  VariableKey perPlayerVariableName = std::string(perPlayerVariableStart_it, perPlayerVariableEnd_it);

  return {playerID, perPlayerVariableName};
}



} // namespace GameState
