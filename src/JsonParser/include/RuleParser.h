#pragma once
#include "nlohmann/json.hpp"

#include "GameRules.h"
#include "GameData.h"

#include <memory>
#include <vector>
#include <unordered_map>

using json = nlohmann::json;

namespace JsonParser {

using RuleList = std::vector<GameRules::RulePtr>;
using VariableName = std::string;
using VariableAliasMap = std::unordered_map<VariableName, VariableName>;


class RuleParser {
public:
  RuleParser() = default;

  RuleList parseRules(json ruleListJson);
private:
  // Scoped variable management for rules such as forEach
  VariableAliasMap activeScopedVariableAliases = {};
  VariableName processScopedVariables(VariableName variableName) const;

  // Rule Parsers
  GameRules::RulePtr parseAddRule(json addRuleJson) const;
  GameRules::RulePtr parseGlobalMessageRule(json globalMessageRuleJson) const;
  GameRules::RulePtr parseForEachRule(json forEachRuleJson);
  GameRules::RulePtr parseInputTextRule(json inputTextRuleJson) const;
};



} // namespace JsonParser
