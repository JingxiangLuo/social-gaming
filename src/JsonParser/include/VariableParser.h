#pragma once
#include "nlohmann/json.hpp"

#include "GameState.h"

#include <string>
#include <unordered_map>

using json = nlohmann::json;

namespace JsonParser {



class VariableParser {
public:
  VariableParser() {}

  GameState::VariableMap parseVariables(json variableJson) const;
private:
  // Any helper functions should be defined here
};



} // namespace JsonParser
