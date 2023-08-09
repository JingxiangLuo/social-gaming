#include "VariableParser.h"

#include <glog/logging.h>

#include "GameState.h"

#include <string>
#include <unordered_map>

using json = nlohmann::json;

namespace JsonParser {



GameState::VariableMap
VariableParser::parseVariables(json variableJson) const
{
  // If there is more than one variable defined, this is represented as additional...
  // ...keys within the object
  if (variableJson.type() != json::value_t::object) {
    LOG(ERROR) << "Variables are not stored in game spec as attributes of an object";
    return {};
  }

  GameState::VariableMap gameVariableMap = {};
  for (const auto& variable : variableJson.items()) {
    try {
      gameVariableMap.insert({variable.key(), variable.value()});
    } catch (const std::exception& e) {
      LOG(ERROR) << "Key or value were not of expected type";
      return {};
    }
  }
  
  return gameVariableMap;
}



} // namespace JsonParser
