#pragma once
#include "nlohmann/json.hpp"

#include "GameData.h"

using json = nlohmann::json;

namespace JsonParser {



class JsonParser {
public:
    JsonParser();

    // Parsing API ends
    json parseJsonString_serverConfig(const std::string& jsonString) const;
    json parseJsonFile_serverConfig(const std::string& jsonFilepath) const;
    GameData::GameData parseJsonFile_gameSpec(const std::string& jsonFilepath) const;
private:
    enum class FileType {GAME_SPEC, SERVER_CONFIG};
    using jsonRootElemProperties = std::vector<std::pair<std::string, json::value_t>>;

    // Parsing/Validation
    json parseJson(const std::string& jsonSource,
                   const bool isFile,
                   const JsonParser::FileType fileType) const;
    bool validateJsonObject(const json& jsonObject,
                            const FileType& fileType) const;
    // Validation helpers
    bool validateJsonContent_gameSpec(const json& jsonObject) const;
    bool validateJsonContent_serverConfig(const json& jsonObject) const;
    bool validateJsonContent_rootLevelElements(const json& jsonObject,
                                               const jsonRootElemProperties& rootElemProperties) const;

    // Wraps Nlohmann's parse() to catch exceptions
    json safeParse(const std::string& jsonSource, const bool isFile) const;
};



} // namespace JsonParser
