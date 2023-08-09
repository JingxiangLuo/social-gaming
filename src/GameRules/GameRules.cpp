#include "GameRules.h"

#include "GameState.h"

#include <glog/logging.h>
#include <string>
#include <vector>

namespace GameRules {



/******************************************************************************
 *                                  Add Rule                                  *
 ******************************************************************************/
// TODO: For now we only support integers as the value - we should support variable names, and floats in the future
AddRule::AddRule(const GameState::VariableKey targetVariableName, GameState::VariableValue value)
  : addTarget(targetVariableName), value(value) {}

[[nodiscard]] RuleExecutionResult AddRule::executeRuleImpl(GameState::GameState& gameState) {
  const GameState::GetVariableResult getVariableResult = gameState.getValue(this->addTarget);
  if (getVariableResult.wasSuccessful == false) {
    LOG(ERROR) << "Failed to get variable: " << this->addTarget;
    return RuleExecutionResult::FAILURE;
  }

  const GameState::VariableValue newTargetValue = getVariableResult.value + this->value;
  
  if (gameState.setValue(this->addTarget, newTargetValue) == GameState::SetVariableResult::FAILURE) {
    LOG(ERROR) << "Failed to set variable: " << this->addTarget;
    return RuleExecutionResult::FAILURE;
  }

  return RuleExecutionResult::SUCCESS;
}



/******************************************************************************
 *                             Global Message Rule                            *
 ******************************************************************************/
GlobalMessageRule::GlobalMessageRule(const std::string messageValue)
  : messageValue(messageValue) { }

// TODO-51: Parse {variable_name} into value
// TODO: Instead of a server log, this should send message to all clients
[[nodiscard]] RuleExecutionResult GlobalMessageRule::executeRuleImpl(GameState::GameState& gameState) {
  LOG(INFO) << "Msg: " << this->messageValue;
  // TODO: return false if any variables within this message fail to parse into values

  return RuleExecutionResult::SUCCESS;
}



/******************************************************************************
 *                                  Loop Rule                                 *
 ******************************************************************************/
LoopRule::LoopRule(std::string stopCondition, Rules rulesToExecuteEachIteration)
  : stopCondition(stopCondition)
  , rulesToExecuteEachIteration(std::move(rulesToExecuteEachIteration)) {}

[[nodiscard]] RuleExecutionResult LoopRule::executeRuleImpl(GameState::GameState& gameState) {
  if (gameState.getValue("debug_target").wasSuccessful == false) {
        return RuleExecutionResult::FAILURE;
  }

  // TODO-#50: Parse the conditional rather than hard-coding it
  while (gameState.getValue("debug_target").value != 10) {
    // Every iteration of this loopRule, execute each rule that is contained within this loopRule
    for (const auto& rule : this->rulesToExecuteEachIteration) {
      if (rule->executeRule(gameState) == RuleExecutionResult::FAILURE) {
        LOG(ERROR) << "Rule within loop failed to execute";
        return RuleExecutionResult::FAILURE;
      }
    }
  }

  return RuleExecutionResult::SUCCESS;
}



/******************************************************************************
 *                               Input Text Rule                              *
 ******************************************************************************/
InputTextRule::InputTextRule(const std::string targettedUser,
                             const std::string inputPrompt,
                             const std::string resultVariable)
  : targettedUser(targettedUser)
  , inputPrompt(inputPrompt)
  , resultVariable(resultVariable) {}

// TODO: Add handling for timeout variable
[[nodiscard]] RuleExecutionResult
InputTextRule::executeRuleImpl(GameState::GameState& gameState) {
  const GameState::GetVariableResult getTargetResult = gameState.getValue(this->targettedUser);
  if (getTargetResult.wasSuccessful == false) {
    LOG(ERROR) << "Failed to get target user: " << this->targettedUser;
    return RuleExecutionResult::FAILURE;
  }

  // TODO-#57:
  // call function which gets input from user
  // -> if user input not number, print error and failure
  // -> try to ensure user enters a number
      // maybe try passing in a lambda function upon creation of gameState
      // this lambda function could maybe be able to (blocking) get input from user
      // blocking is okay because we dont have any inparallel yet

  // TODO-#57: Get the value from user above
  const GameState::VariableValue resultValue = 123456789;
  if (gameState.setValue(this->resultVariable, resultValue) == GameState::SetVariableResult::FAILURE) {
    LOG(ERROR) << "Failed to set variable: " << this->resultVariable;
    return RuleExecutionResult::FAILURE;
  }

  // TODO-#57: uncomment when we have a working implementation
  // return RuleExecutionResult::SUCCESS;

  /* TODO-#57: Initial thoughts
      - Select specified user
      - Somehow indicate to the outside that we should pause and wait for input from user
          - Do we take input from in here?
          - Rules are on the server, we must make the server fetch the user input from clients
          - Can we return? It a little hard:
              - that would end this function, do we return a callback then?
              - That's not even easy, this rule is very likely not a topLevelRule
              - Tricky....
      - Save the input to a variable within the user?
  */
  return RuleExecutionResult::FAILURE;
}



/******************************************************************************
 *                                For Each Rule                               *
 ******************************************************************************/
ForEachRule::ForEachRule(const GameState::VariableKey& listName,
                         const GameState::VariableKey& listElementName,
                         Rules rulesToExecuteEachElement)
  : listName(listName)
  , listElementName(listElementName)
  , rulesToExecuteEachElement(std::move(rulesToExecuteEachElement)) {}

[[nodiscard]] RuleExecutionResult
ForEachRule::executeRuleImpl(GameState::GameState& gameState) {
  // TODO: Currently only iterating through lists of players is supported
  if (this->listName != "players") {
    LOG(ERROR) << "Only iterating through \"players\" list is supported";
    return RuleExecutionResult::FAILURE;
  }

  // TODO: Support more lists than just the player list
  for (const GameState::PlayerID& playerID : gameState.getPlayerIDs()) {
    // Put forEach element variable in scope for any rules that execute after this
    if (gameState.setActiveScopeVariable(this->listElementName, playerID) == GameState::SetVariableResult::FAILURE) {
      return RuleExecutionResult::FAILURE;
    }

    // Execute all the rules within this loop for each player
    for (const auto& rule : this->rulesToExecuteEachElement) {
      if (rule->executeRule(gameState) == RuleExecutionResult::FAILURE) {
        LOG(ERROR) << "Rule within forEach failed to execute";
        return RuleExecutionResult::FAILURE;
      }
    }
    
    // Take forEach element variable out of scope, future rule executions can't see forEach element
    if (gameState.unsetActiveScopeVariable(this->listElementName) == GameState::SetVariableResult::FAILURE) {
      return RuleExecutionResult::FAILURE;
    }
  }
  
  return RuleExecutionResult::SUCCESS;
}



}
