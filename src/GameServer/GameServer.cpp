#include "GameServer.h"

#include <glog/logging.h>

#include <algorithm>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <sstream>
#include <iostream>
#include "Client.h"
#include "GameData.h"
#include "GameRules.h"
#include "GameState.h"
#include "JsonParser.h"
#include "ServerConfig.h"


void GameServer::setupConfig() {
    ServerConfig config{"../social-gaming/data/serverconfig.json"};

    std::string gameName;
    std::cout << "Please specify the game you would like to play\n";
    std::cin >> gameName;
    GameData::GameData gameData = config.parseGamefile(gameName);
    // is valid will also check if game exists
    if (!config.isValid()) {
        LOG(ERROR) << "Invalid server configuration";
        google::ShutdownGoogleLogging();
        std::exit(1);
    }

    this->port = config.getPort();
    this->serverHtml = config.getServerHtml();
    this->inviteCode = config.generateInviteCode();
    this->gameData = std::move(gameData);
    LOG(INFO) << "Validated server configuration file... Launching server";
    LOG(INFO) << "Clients can connect with invite code " + inviteCode;
}

void GameServer::removeDisconnectedUser(const networking::Connection& c) {
    for (auto user : users) {
        if (user.getConnection() == c) {
            auto beginErase = std::remove(std::begin(users), std::end(users), user);
            users.erase(beginErase, std::end(users));
            return;
        }
    }
}

void GameServer::onConnect(const networking::Connection& c) {
    LOG(INFO) << "New connection found: " << c.id;
    clients.push_back(c);
    User user = User(c);
    users.push_back(user);
}

void GameServer::onDisconnect(const networking::Connection& c) {
    LOG(INFO) << "Connection lost: " << c.id;
    auto eraseBegin = std::remove(std::begin(clients), std::end(clients), c);
    clients.erase(eraseBegin, std::end(clients));
    removeDisconnectedUser(c);
}

/**
 * Changes the value of the user's nickname in the User object in the `users` vector.
 * Makes no changes if the user does not exist in the vector.
 *
 * @param id id of the users Connection object's
 * @param nickname A string of the new nickname
 */
void GameServer::changeUserNickname(uintptr_t id, std::string& nickname) {
    for (auto& user : users) {
        if (user.userConnection.id == id) {
            user.nickname = nickname;
            return;
        }
    }
    LOG(ERROR) << "Attempted to change the nickname of a user not presently connected.";
}

/**
 * Retrives the nickname value from the 'nicknames' map and returns it.
 * If the user is not present in the map, it will return an empty string
 * as a default value.
 *
 * @param id id of the users Connection object's
 * @returns the nickname of the user or an empty string
 */
std::string GameServer::getUserNickname(uintptr_t id) {
    for (auto user : users) {
        if (user.userConnection.id == id) {
            return user.nickname;
        }
    }
    LOG(ERROR) << "Attempted to get the nickname of a user not presently connected. Returning empty string.";
    return "";
}

std::deque<networking::Message> GameServer::buildOutgoing(const std::string& log) {
    std::deque<networking::Message> outgoing;
    for (auto client : clients) {
        outgoing.push_back({client, log});
    }
    return outgoing;
}

std::string GameServer::getHTTPMessage(const char* htmlLocation) {
    if (access(htmlLocation, R_OK) != -1) {
        std::ifstream infile{htmlLocation};
        return std::string{std::istreambuf_iterator<char>(infile),
                           std::istreambuf_iterator<char>()};
    } else {
        LOG(ERROR) << "Unable to open HTML index file: " << htmlLocation;
        std::exit(-1);
    }
}

MessageResult GameServer::processMessages(networking::Server& server,
                                          const std::deque<networking::Message>& incoming) {
    const char COMMAND_PREFIX = '/';
    std::ostringstream result;
    bool quit = false;

    for (auto& message : incoming) {
        auto displayName = getUserNickname(message.connection.id);

        if (message.text == "quit") {
            server.disconnect(message.connection);
        } else if (message.text == "shutdown") {
            LOG(INFO) << "Shutting down";
            quit = true;
        } else if (message.text.at(0) == COMMAND_PREFIX) {
            // TODO-#58: Refactor to address string manipulation and reduce complexity
            const auto command = message.text.substr(1, message.text.find(' ') - 1);

            if (command == "nickname") {
                auto nickname = message.text.substr(message.text.find(' ') + 1);
                changeUserNickname(message.connection.id, nickname);

                // Send conformation message that nickname has been chnaged
                result << displayName << " has changed their name to: " << nickname << ".\n";
            } 
            else if (command == "execute") {
                if (!gameData.isValid) {
                    result << "\tCannot execute game - not yet loaded\n";
                }
                else {
                    result << "Found cmd: execute (Execute game)\n";
                    // TODO-#45: Move this logic into a module separate from the command handling section
                    result << "\tExecuting loaded game\n";

                    // TODO-#57: Move this little mechanism for converting users to IDs to somewhere else
                    //           once input/output is implemented
                    GameState::PlayerIDList playerIDs = {};
                    std::transform(users.begin(),
                                   users.end(),
                                   std::back_inserter(playerIDs),
                                   [](User& user) {
                                       return user.getConnection().id;
                                   });
                    GameState::GameState gameState = GameState::GameState(gameData.variableMap,
                                                                          playerIDs,
                                                                          gameData.perPlayerVariableMap);

                    // TODO-#51: Just to demonstrate that the rule is doing something, can remove later
                    result << "\tdebug_target variable BEFORE executing rules: "
                           << gameState.getValue("debug_target").value << "\n";

                        for (const auto& rule : this->gameData.topLevelRules) {
                            if (rule->executeRule(gameState) == GameRules::RuleExecutionResult::FAILURE) {
                                LOG(ERROR) << "Top level rule failed to execute";
                                break;
                            }
                        }
                    
                    // TODO-#51: Just to demonstrate that the rule is doing something, can remove later
                    result << "\tdebug_target variable AFTER executing rules: "
                           << gameState.getValue("debug_target").value << "\n";

                    result << "\tGame finished executing.\n";
                }
            } 
            else {
                result << displayName << "> command not found.\n";
            }
        } else {
            result << displayName << "> " << message.text << "\n";
        }
    }
    return MessageResult{result.str(), quit};
}

void GameServer::run() {
    networking::Server server(
        this->port, this->serverHtml,
        [this](networking::Connection& c) { onConnect(c); },
        [this](networking::Connection& c) { onDisconnect(c); });

    while (true) {
        bool errorWhileUpdating = false;
        try {
            server.update();
        } catch (std::exception& e) {
            LOG(ERROR) << "Exception from Server update: " << e.what();
            errorWhileUpdating = true;
        }

        auto incoming = server.receive();
        auto [log, shouldQuit] = processMessages(server, incoming);
        auto outgoing = buildOutgoing(log);
        server.send(outgoing);

        if (shouldQuit || errorWhileUpdating) {
            break;
        }

        sleep(1);
    }
}

