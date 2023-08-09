#include "RuleParser.h"

#include <glog/logging.h>

#include "GameRules.h"
#include "GameData.h"

#include <memory>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>

using json = nlohmann::json;

namespace JsonParser {



/******************************************************************************
 *                         Scoped Variable Management                         *
 ******************************************************************************/
// Convert variableName to one with context if possible
// TODO-#58: Refactor to address string manipulation and reduce complexity
VariableName
RuleParser::processScopedVariables(VariableName variableName) const {
  // Looping is necessary because variableName could be
  // "XYZ.player.input" (where "player" is scoped variable)
  // In which case, we only want to replace this part:
  // "XYZ.player.input" -> "XYZ.players.$player.input"
  //      ^^^^^^^               ^^^^^^^^^^^^^^^^  
  //      |     |
  //     Start  End
  VariableName newVariableName = "";

  auto variableComponentStart_it = variableName.begin();
  auto variableComponentEnd_it = variableName.begin();
  while (variableComponentEnd_it != variableName.end()) {
    variableComponentEnd_it = std::find(variableComponentStart_it,
                                        variableName.end(),
                                        '.');
    VariableName variableComponent = std::string(variableComponentStart_it, variableComponentEnd_it);
    if (this->activeScopedVariableAliases.count(variableComponent) == 1) {
      newVariableName.append(this->activeScopedVariableAliases.at(variableComponent));
    } else {
      newVariableName.append(variableComponent);
    }
    if (variableComponentEnd_it != variableName.end()) {
      newVariableName.append(".");
    }
    variableComponentStart_it = variableComponentEnd_it + 1;
  }

  return newVariableName;
}



/******************************************************************************
 *                             Public Rule Parser                             *
 ******************************************************************************/
RuleList
RuleParser::parseRules(json ruleListJson)
{
  RuleList orderedRuleList = {};

  if (ruleListJson.type() != json::value_t::array) {
      LOG(ERROR) << "Game rules are not an array at its highest level";
  }

  for (const auto& ruleJson : ruleListJson) {
    if (!ruleJson.contains("rule")) {
      LOG(ERROR) << "Rule json object does not specify the type of rule (\"rule\" missing)";
      return {};
    }
    
    // TODO-#60: This should probably be implemented as a map of {ruleName->ruleParseImplementations}
    GameRules::RulePtr ruleObject;
    if (ruleJson["rule"] == "add") {
      ruleObject = parseAddRule(ruleJson);
    } else if (ruleJson["rule"] == "global-message") {
      ruleObject = parseGlobalMessageRule(ruleJson);
    } else if (ruleJson["rule"] == "loop") {
      // TODO-#50: The tricky part of implementing loop is handling the conditional (when to stop)
      // ruleObject = parseLoopRule(ruleJson);
    } else if (ruleJson["rule"] == "foreach") {
      ruleObject = parseForEachRule(ruleJson);
    } else if (ruleJson["rule"] == "input-text") {
      ruleObject = parseInputTextRule(ruleJson);
    } else {
      LOG(ERROR) << "Unrecognized type of rule";
      return {};
    }

    if (ruleObject == nullptr) {
      LOG(ERROR) << "Rule failed to parse";
      return {};
    }
    
    orderedRuleList.push_back(std::move(ruleObject));
  }


  return orderedRuleList;
}


/******************************************************************************
 *                           Individual Rule Parsers                          *
 ******************************************************************************/
GameRules::RulePtr
RuleParser::parseAddRule(json addRuleJson) const
{
  if (!addRuleJson.contains("to") || !addRuleJson.contains("value")) {
    LOG(ERROR) << "Add rule is missing properties";
    return nullptr;
  }

  if (addRuleJson.size() > 3) {
    LOG(ERROR) << "Add rule has too many properties";
    return nullptr;
  }

  std::unique_ptr<GameRules::AddRule> addRule = nullptr;
  try {
    addRule = std::make_unique<GameRules::AddRule>(processScopedVariables(addRuleJson["to"]),
                                                   addRuleJson["value"]);
  } catch (const std::exception& e) {
    LOG(ERROR) << "Add rule properties \"to\" or \"value\" was of invalid type";
    return nullptr;
  }

  return std::move(addRule);
}

GameRules::RulePtr
RuleParser::parseGlobalMessageRule(json globalMessageRuleJson) const
{
  if (!globalMessageRuleJson.contains("value")) {
    LOG(ERROR) << "Global message rule is missing value property";
    return nullptr;
  }

  if (globalMessageRuleJson.size() > 2) {
    LOG(ERROR) << "Global message rule has too many properties";
    return nullptr;
  }

  try {
    // TODO-#51: we need to somehow access variables from here?
    return std::make_unique<GameRules::GlobalMessageRule>(globalMessageRuleJson["value"]);
  } catch (const std::exception& e) {
    LOG(ERROR) << "Global message rule property \"value\" was not string";
    return nullptr;
  }
}

GameRules::RulePtr
RuleParser::parseForEachRule(json forEachRuleJson)
{
  if (!forEachRuleJson.contains("list") || !forEachRuleJson.contains("element")
                                    || !forEachRuleJson.contains("rules")) {
    LOG(ERROR) << "For-each rule is missing properties";
    return nullptr;
  }

  if (forEachRuleJson.size() > 4) {
    LOG(ERROR) << "For-each rule has too many properties";
    return nullptr;
  }

  std::unique_ptr<GameRules::ForEachRule> forEachRule = nullptr;
  try {
    if (std::string(forEachRuleJson["list"]) != "players") {
      LOG(ERROR) << "List found that is not named players - currently only \"players\" list is supported";
      return nullptr;
    }

    // Convert any references of `element` into "`list`.$`element`" within the child rules
    // The ($) will let us identify which elements need to be dynamically evaluated during runtime
    VariableName listElemVariableName{forEachRuleJson["element"]};
    VariableName contextedElemVariableName{forEachRuleJson["list"]};
    contextedElemVariableName.append(".$");
    contextedElemVariableName.append(forEachRuleJson["element"]);

    // Parsing forEach, any references to "element" should be converted to "list.$element"
    this->activeScopedVariableAliases.insert({listElemVariableName, contextedElemVariableName});

    forEachRule = std::make_unique<GameRules::ForEachRule>(forEachRuleJson["list"],
                                                           forEachRuleJson["element"],
                                                           parseRules(forEachRuleJson["rules"]));

    // Done parsing forEach, no longer need to convert "element" -> "list.$element"
    this->activeScopedVariableAliases.erase(listElemVariableName);
  } catch (const std::exception& e) {
    LOG(ERROR) << "For-each rule properties were of invalid type";
    return nullptr;
  }

  return std::move(forEachRule);
}

// TODO: We should probably have some way of identifying and asserting that "to" is a player
// TODO: Add hanlding for timeout variable
GameRules::RulePtr
RuleParser::parseInputTextRule(json inputTextRuleJson) const
{
  if (!inputTextRuleJson.contains("to") || !inputTextRuleJson.contains("prompt")
                                        || !inputTextRuleJson.contains("result")) {
    LOG(ERROR) << "Input-text rule is missing properties";
    return nullptr;
  }

  if (inputTextRuleJson.size() > 4) {
    LOG(ERROR) << "Input-text rule has too many properties";
    return nullptr;
  }

  std::unique_ptr<GameRules::InputTextRule> inputTextRule = nullptr;
  try {
    inputTextRule = std::make_unique<GameRules::InputTextRule>(processScopedVariables(inputTextRuleJson["to"]),
                                                               inputTextRuleJson["prompt"],
                                                               processScopedVariables(inputTextRuleJson["result"]));
  } catch (const std::exception& e) {
    LOG(ERROR) << "Input-text rule properties were of invalid type";
    return nullptr;
  }

  return std::move(inputTextRule);
}



} // namespace JsonParser
