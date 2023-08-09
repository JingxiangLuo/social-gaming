#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "GameState.h"

namespace GameRules {



class Rule;
using RulePtr = std::unique_ptr<Rule>;
using Rules = std::vector<RulePtr>;

enum class RuleExecutionResult {
  SUCCESS,
  FAILURE,
};

class Rule {
public:
  Rule() = default;

  [[nodiscard]] RuleExecutionResult executeRule(GameState::GameState& gameState) {
    return executeRuleImpl(gameState);
  }
private:
  [[nodiscard]] virtual RuleExecutionResult executeRuleImpl(GameState::GameState& gameState) = 0;
};


class AddRule : public Rule {
public:
  // TODO: For now we only support integers as the value - we should support variable names, and floats in the future
  AddRule(const GameState::VariableKey targetVariableName, GameState::VariableValue value);
private:
  const GameState::VariableKey addTarget;  // Variable name of an integer to add to
  const GameState::VariableValue value;  // Constant containing the value to add

  [[nodiscard]] RuleExecutionResult executeRuleImpl(GameState::GameState& gameState);
};


class GlobalMessageRule : public Rule {
public:
  GlobalMessageRule(const std::string messageValue);
private:
  std::string messageValue; // Value of message to send  // TODO: const?

  [[nodiscard]] RuleExecutionResult executeRuleImpl(GameState::GameState& gameState);
};


class LoopRule : public Rule {
public:
  LoopRule(std::string stopCondition, Rules rulesToExecuteEachIteration);
private:
  std::string stopCondition; // Condition that may fail
  Rules rulesToExecuteEachIteration;

  [[nodiscard]] RuleExecutionResult executeRuleImpl(GameState::GameState& gameState);
};


class InputTextRule : public Rule {
public:
  // TODO: Add handling for audience members
  InputTextRule(const std::string targettedUser,
                const std::string inputPrompt,
                const std::string resultVariable);
private:
  const std::string targettedUser; // TODO-#57: Alias
  const std::string inputPrompt; // TODO-#57: Alias
  const std::string resultVariable; // TODO-#57: alias

  [[nodiscard]] RuleExecutionResult executeRuleImpl(GameState::GameState& gameState);
};


class ForEachRule : public Rule {
public:
  // TODO: Add handling for non-user lists
  ForEachRule(const GameState::VariableKey& listName,
              const GameState::VariableKey& listElementName,
              Rules rulesToExecuteEachElement);
private:
  const GameState::VariableKey listName;
  const GameState::VariableKey listElementName;
  Rules rulesToExecuteEachElement;

  [[nodiscard]] RuleExecutionResult executeRuleImpl(GameState::GameState& gameState);
};



} // namespace GameRules
