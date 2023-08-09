#pragma once

#include "nlohmann/json.hpp"

#include "GameRules.h"
#include "GameState.h"

#include <vector>

using json = nlohmann::json;

/**
 * This header file specifies the format for how gameData is passed between...
 * ...different intercommunicating modules throughout the project
 */

namespace GameData {



using TopLevelRules = std::vector<GameRules::RulePtr>;

struct GameData {
    bool isValid = false;
    // TODO: configuration
    // TODO: constants (Probably also an unordered_map)
    GameState::VariableMap variableMap = {};

    GameState::VariableMap perPlayerVariableMap = {};

    // TODO: per-audience
    TopLevelRules topLevelRules = {};
};



}
