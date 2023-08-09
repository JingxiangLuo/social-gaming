#include "ServerConfig.h"
#include "JsonParser.h"
#include <fstream>
#include <glog/logging.h>

ServerConfig::ServerConfig(const std::string& configFilepath)
{
    // https://stackoverflow.com/questions/25225948/how-to-check-if-a-file-exists-in-c-with-fstreamopen (check if file can be opened)
    std::fstream fileStream;
    fileStream.open(configFilepath);
    if (fileStream.fail())
    {
        this->valid = false;
        LOG(ERROR) << "Error opening config file: configFilepath not saved";
        return;
    }
    fileStream.close();
    this->configFilepath = configFilepath;

    JsonParser::JsonParser parser = JsonParser::JsonParser();
    json configData = parser.parseJsonFile_serverConfig(this->configFilepath);
    if (configData == nullptr) {
        LOG(ERROR) << "Failed to validate server configuration file: Skipping "
                   << "configuration";
        return;
    }
    configure(configData);
}

void ServerConfig::setHtmlFilepath(const std::string& htmlFilepath)
{
    // https://stackoverflow.com/questions/25225948/how-to-check-if-a-file-exists-in-c-with-fstreamopen (check if file can be opened)
    std::fstream fileStream;
    fileStream.open(htmlFilepath);
    if (fileStream.fail())
    {
        LOG(ERROR) << "Error opening html file: htmlFilepath not saved";
        this->valid = false;
        return;
    }

    fileStream.close();
    this->htmlFilepath = htmlFilepath;
}

void ServerConfig::configure(const json& config)
{
    this->port = config["port"];
    this->htmlFilepath = config["serverhtml"];
    this->valid = true;
}

std::string ServerConfig::generateInviteCode()
{
    std::string inviteCode = std::to_string(this->port);
    std::reverse(inviteCode.begin(),inviteCode.end());   
    return inviteCode;
}

unsigned short ServerConfig::getPort()
{
    return this->port;
}

std::string ServerConfig::getServerHtml()
{
    return this->htmlFilepath;
}

bool ServerConfig::isValid()
{
    return this->valid;
}

GameData::GameData ServerConfig::parseGamefile(const std::string& gameName)
{
    GameData::GameData gameData;
    if (this->gameFilepaths.count(gameName) == 0)
    {
        LOG(ERROR) << "Game does not exist";
        this->valid = false;
        return gameData;
    }
    JsonParser::JsonParser parser = JsonParser::JsonParser();
    gameData = parser.parseJsonFile_gameSpec(this->gameFilepaths.at(gameName));
    return gameData;
}