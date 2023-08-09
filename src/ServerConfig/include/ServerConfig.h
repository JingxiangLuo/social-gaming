#pragma once
#include "JsonParser.h"

class ServerConfig 
{
    //Server configuration file specified by json
    //The file will be interpreted, and be attached to the Server instance as configurations
public:
    ServerConfig(const std::string& configFilepath);
    void setHtmlFilepath(const std::string& htmlFilepath);
    unsigned short getPort();
    std::string getServerHtml();
    std::string generateInviteCode(); //keep invite code different from port number
    GameData::GameData parseGamefile(const std::string& gameName);
    bool isValid();
    
private:
    void configure(const json& config);
    //map of string and json path
    const std::unordered_map<std::string,std::string> gameFilepaths
    {
        //add game file paths and matching string here
        { "Test", "../social-gaming/test/json/gameSpec_forEach_basic.json"}
    };
    std::string configFilepath;
    std::string htmlFilepath;
    unsigned short port;
    bool valid = false;
};