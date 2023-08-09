#pragma once

#include <unistd.h>

#include <deque>
#include <string>
#include <vector>

#include "Server.h"
#include "ServerConfig.h"
#include "User.h"

struct MessageResult {
    std::string result;
    bool shouldShutdown;
};

class GameServer {
  public:
    GameServer() = default;
    GameServer(const GameServer&) = delete;
    GameServer(GameServer&&) = delete;
    virtual ~GameServer() = default;

    GameServer& operator=(const GameServer&) = delete;
    GameServer& operator=(GameServer&&) = delete;

    void setupConfig();
    void run();

  private:
    unsigned short port;
    std::string serverHtml;
    std::string inviteCode;
    GameData::GameData gameData;

    void removeDisconnectedUser(const networking::Connection& c);
    void onConnect(const networking::Connection& c);
    void onDisconnect(const networking::Connection& c);
    void changeUserNickname(uintptr_t id, std::string& nickname);
  

    std::string getUserNickname(uintptr_t id);
    std::string getHTTPMessage(const char* htmlLocation);
    std::string parseInviteCode(const std::string& inviteCode);

    std::vector<networking::Connection> clients;
    std::vector<User> users;

    std::deque<networking::Message> buildOutgoing(const std::string& log);
    MessageResult processMessages(networking::Server& server, const std::deque<networking::Message>& incoming);
};