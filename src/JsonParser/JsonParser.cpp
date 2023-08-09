#include "JsonParser.h"

#include "nlohmann/json.hpp"

#include "RuleParser.h"
#include "VariableParser.h"
#include "GameData.h"
#include "GameRules.h"
#include "GameState.h"

#include <fstream>
#include <glog/logging.h>
#include <memory>

using json = nlohmann::json;

// Basic examples for nlohmann's json parsing can be found in the docs:
// https://github.com/nlohmann/json#examples
namespace JsonParser {



////////////////////////////////////////////////////////////////////////////////
//                                  Public                                    
////////////////////////////////////////////////////////////////////////////////
/**
 * No data members to initialize - using default constructor
 */
JsonParser::JsonParser() = default;


/**
 * Parses and validates the server configuration JSON string
 *
 * @param jsonString A string containing JSON data
 * @return A C++ JSON object which represents the JSON data input
 *         - (nullptr for invalid input)
 */
json
JsonParser::parseJsonString_serverConfig(const std::string& jsonString) const
{
  return parseJson(jsonString, false, FileType::SERVER_CONFIG);
}


/**
 * Parses and validates the server configuration JSON data located at the
 * specified filepath
 *
 * @param jsonFilepath A path to a file containing JSON data
 * @return A C++ JSON object which represents the JSON data input
 *         - (nullptr for invalid input)
 */
json
JsonParser::parseJsonFile_serverConfig(const std::string& jsonFilepath) const
{
  return parseJson(jsonFilepath, true, FileType::SERVER_CONFIG);
}


/**
 * Parses the game specification JSON at the specified filepath. Also validates 
 * JSON to ensure that game logic and variables follow the expected format of a
 * game specification file.
 * 
 * @param jsonFilepath A path to a file containing JSON data for the game spec
 * @return Data for game variables and rules which correspond to the game spec's
 *         JSON data
 *         - (invalid data is returned when game spec JSON is invalid)
 */
GameData::GameData
JsonParser::parseJsonFile_gameSpec(const std::string& jsonFilepath) const
{
  json parseResult = parseJson(jsonFilepath, true, FileType::GAME_SPEC);

  if (parseResult == nullptr) {
    return {};
  }
  
  const json variableJson = parseResult["variables"];
  const VariableParser variableParser = VariableParser();
  const GameState::VariableMap variableMap = variableParser.parseVariables(variableJson);
  
  const json perPlayerJson = parseResult["per-player"];
  const GameState::VariableMap perPlayerVariableMap = variableParser.parseVariables(perPlayerJson);

  const json rulesJson = parseResult["rules"];
  RuleParser ruleParser = RuleParser();
  GameData::TopLevelRules topLevelRules = ruleParser.parseRules(rulesJson);

  if (variableMap.size() == 0 || topLevelRules.size() == 0) {
    return {};
  }

  return {
    .isValid = true,
    .variableMap = variableMap,
    .perPlayerVariableMap = perPlayerVariableMap,
    .topLevelRules = std::move(topLevelRules),
  };
}



////////////////////////////////////////////////////////////////////////////////
//                                  Private                                    
////////////////////////////////////////////////////////////////////////////////
/**
 * Parses and validates the JSON data from the specified source
 *
 * @param jsonSource The JSON data in string form, or a filepath to JSON data
 * @param isFile Indicates whether to treat jsonSource as a filepath/JSON string
 * @param fileType The expected format of the JSON data
 * @return A C++ JSON object which represents the JSON data input
 *         - (nullptr for invalid input)
 */
json
JsonParser::parseJson(const std::string& jsonSource, 
                      const bool isFile, 
                      const JsonParser::FileType fileType) const
{
  json jsonObject = safeParse(jsonSource, isFile);
  if (jsonObject == nullptr) {
    LOG(ERROR) << "Failed to read JSON file or JSON data was not properly "
               << "formatted: returning nullptr";
    return nullptr;
  }

  if (!validateJsonObject(jsonObject, fileType)) {
    LOG(ERROR) << "Failed to validate JSON object: returning nullptr";
    return nullptr;
  }

  return jsonObject;
}


/**
 * Validates the contents of the C++ JSON object passed in
 * 
 * @param jsonObject The JSON object to validate
 *                   - i.e: {"pi": 3.141, "happy": true}
 * @param fileType The intended file type of the JSON input, used for property
 *                 validation
 * @return True if the JSON format is valid for the specified filetype, 
 *         false otherwise
 */
bool
JsonParser::validateJsonObject(const json& jsonObject,
                               const JsonParser::FileType& fileType) const
{
  // Validate that json object matches defined structure
  switch (fileType) {
  case FileType::GAME_SPEC:
    if (!validateJsonContent_gameSpec(jsonObject)) {
      LOG(ERROR) << "JSON contents do not match game specification file "
                 << "contents: returning nullptr";
      return false;
    }
    break;

  case FileType::SERVER_CONFIG:
    if (!validateJsonContent_serverConfig(jsonObject)) {
      LOG(ERROR) << "JSON contents do not match server configuration file "
                 << "contents: returning nullptr";
      return false;
    }
    break;

  default:
    LOG(ERROR) << "Unknown fileType: returning nullptr";
    return false;
  }

  return true;
}


/**
 * Validates the format of a game specification JSON object
 * 
 * @param jsonObject The JSON object containing the game specification data
 * @return True for valid format, false otherwise
 */
bool
JsonParser::validateJsonContent_gameSpec(const json& jsonObject) const
{
  // As defined in milestone 1 project spec:
  jsonRootElemProperties GS_ROOT_ELEMS = {
    std::pair{"configuration", json::value_t::object},
    std::pair{"constants", json::value_t::object},
    std::pair{"variables", json::value_t::object},
    std::pair{"per-player", json::value_t::object},
    std::pair{"per-audience", json::value_t::object},
    std::pair{"rules", json::value_t::array}
  };

  return validateJsonContent_rootLevelElements(jsonObject, GS_ROOT_ELEMS);
}


/**
 * Validates the format of a server configuration JSON object
 * 
 * @param jsonObject The JSON object containing the server configuration data
 * @return True for valid format, false otherwise
 */
bool
JsonParser::validateJsonContent_serverConfig(const json& jsonObject) const
{
  jsonRootElemProperties SC_ROOT_ELEMS = {
    std::pair{"port", json::value_t::number_unsigned},
    std::pair{"serverhtml", json::value_t::string}
  };

  return validateJsonContent_rootLevelElements(jsonObject, SC_ROOT_ELEMS);
}


/**
 * Validates the format of a JSON object based on number of root elements,
 * name of root elements, and type of root element values
 * 
 * @param jsonObject The JSON object to be validated
 * @param rootElemProperties The desired root-level elements and their types
 * @return True for valid format, false otherwise
 */
bool
JsonParser::validateJsonContent_rootLevelElements(const json& jsonObject,
                                                  const jsonRootElemProperties& rootElemProperties) const
{
  // Validate # of elements
  if (jsonObject.size() != rootElemProperties.size()) {
    return false;
  }

  // Validate element names
  if (std::any_of(rootElemProperties.begin(), rootElemProperties.end(),
                  [jsonObject](auto rootLevelKey) {
                    return !jsonObject.contains(rootLevelKey.first);
                  })) {
    return false;
  }

  // Validate element value types
  if (std::any_of(rootElemProperties.begin(), rootElemProperties.end(),
                  [jsonObject](auto rootLevelKey) {
                    json::value_t actual_type =
                        jsonObject[rootLevelKey.first].type();
                    json::value_t expected_type = rootLevelKey.second;
                    return actual_type != expected_type;
                  })) {
    return false;
  }

  return true;
}


/**
 * Wraps Nlohmann's parse() so that exceptions are handled gracefully
 * 
 * @param jsonSource Either the JSON string itself, or a path to the file
 *                   containing JSON data
 * @param isFile Indicates whether jsonSource is to be treated as JSON, or as a 
 *               filepath
 * @return The C++ JSON object containing the JSON data
 *         - (or nullptr if JSON was invalid)
 */
json
JsonParser::safeParse(const std::string& jsonSource, const bool isFile) const
{
  json result;
  try {
    if (isFile) {
      result = json::parse(std::ifstream(jsonSource));
    } else {
      result = json::parse(jsonSource);
    }
  } catch(const json::parse_error& ex) {
    LOG(ERROR) << "Invalid JSON format passed in or invalid filepath: "
               << "returning nullptr.\n"
               << "\tParse error at byte: " << ex.byte;
    return nullptr;
  }
  return result;
}



} // namespace JsonParser
