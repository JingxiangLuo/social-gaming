#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>



namespace GameState {


using NestedVariableKey = std::string; // <---  i.e. "players.$player.input"
using VariableKey = std::string;       // <---  i.e. "debug_target"
// TODO-#59?: In the future, variable values might want to be more than just ints
using VariableValue = int;
using VariableMap = std::unordered_map<VariableKey, VariableValue>;

using PlayerID = uintptr_t;
using PlayerIDList = std::vector<PlayerID>;
using PlayerVariableMaps = std::unordered_map<VariableKey, VariableMap>;

// TODO-#59?: Currently only holds playerIDs, we should generalize this to more types
using ActiveScopeVariableMap = std::unordered_map<VariableKey, PlayerID>;

struct GetVariableResult {
  bool wasSuccessful = false;
  VariableValue value = 0;
};
struct GetScopedVariableResult {
  bool wasSuccessful = false;
  PlayerID value = 0;
};

enum class SetVariableResult { SUCCESS, FAILURE };

class GameState {
  public:
    GameState() = delete;
    GameState(const VariableMap& variableMap,
              const PlayerIDList& playerIds,
              const VariableMap& playerVariableMaps);

    [[nodiscard]] GetVariableResult getValue(NestedVariableKey nestedVariableName) const;
    [[nodiscard]] SetVariableResult setValue(NestedVariableKey nestedVariableName, VariableValue newValue);

    [[nodiscard]] PlayerIDList getPlayerIDs() const;

    // Scope methods
    [[nodiscard]] SetVariableResult setActiveScopeVariable(VariableKey variableName, PlayerID value);
    [[nodiscard]] SetVariableResult unsetActiveScopeVariable(VariableKey variableName);
    [[nodiscard]] GetScopedVariableResult getActiveScopeVariable(VariableKey variableName) const;
  private:
    // TODO-#59: Variables can be held in a single data structure if we use something like a tree of sorts
    VariableMap variableMap;
    PlayerVariableMaps playerVariableMaps;

    // Used for forEach iteration
    ActiveScopeVariableMap activeScopeVariables;

    // TODO: Constant handling
    
    // Specialized variable accessers and modifiers
    [[nodiscard]] GetVariableResult getVariable(VariableKey variableName) const;
    [[nodiscard]] SetVariableResult setVariable(VariableKey variableName, VariableValue newValue);

    [[nodiscard]] GetVariableResult getPlayerVariable(PlayerID playerID,
                                                      VariableKey variableName) const;
    [[nodiscard]] SetVariableResult setPlayerVariable(PlayerID playerID,
                                                      VariableKey variableName,
                                                      VariableValue newValue);
};



} // namespace GameState
