#include <glog/logging.h>

#include <iostream>

#include "Client.h"
#include "GameChatWindow.h"
#include "GameServer.h"
#include "logconfig.h"

void printWelcome() {
    std::cout << "-----------------------------------------------------------------------\n";
    std::cout << "---  Welcome to social gaming!                                      ---\n";
    std::cout << "---                                                                 ---\n";
    std::cout << "---  Please select between hosting a lobby or joining a game        ---\n";
    std::cout << "---                                                                 ---\n";
    std::cout << "---                                                                 ---\n";
    std::cout << "---                                                                 ---\n";
    std::cout << "---  Type 1 to create a lobby                                       ---\n";
    std::cout << "---  Type 2 to join a lobby                                         ---\n";
    std::cout << "-----------------------------------------------------------------------\n";
}

std::string parseInviteCode(const std::string& inviteCode) {
    std::string parsedInviteCode;
    // inviteCode is just reverse of port number
    std::for_each(inviteCode.rbegin(), inviteCode.rend(), [&parsedInviteCode](char c) { parsedInviteCode += c; });
    return parsedInviteCode;
}

void executeJoinLobby() {
    std::string inviteCode;
    std::cout << "Please specify your invite code\n";
    std::cin >> inviteCode;

    networking::Client client{"localhost", parseInviteCode(inviteCode)};  // would it ever not be localhost?

    bool isExitCommandReceieved = false;
    auto onTextEntry = [&isExitCommandReceieved, &client](std::string text) {
        if ("exit" == text || "quit" == text) {
            isExitCommandReceieved = true;
        } else {
            client.send(text);
        }
    };

    ChatWindow chatWindow(onTextEntry);

    while (!isExitCommandReceieved && !client.isDisconnected()) {
        try {
            client.update();
        } catch (std::exception& e) {
            chatWindow.displayText("Exception from Client update:");
            chatWindow.displayText(e.what());
            break;
        }
        auto response = client.receive();
        if (!response.empty()) {
            chatWindow.displayText(response);
        }
        chatWindow.update();
    }
}

int main(int argc, char* argv[]) {
    google::InitGoogleLogging(argv[0], &customPrefix);
    FLAGS_alsologtostderr = 1;
    FLAGS_log_dir = "../social-gaming/log/files";

    printWelcome();

    std::string command;
    std::cin >> command;

    if (command == "1") {
        GameServer server;
        server.setupConfig();
        server.run();
    } else if (command == "2") {
        executeJoinLobby();
    } else {
        LOG(ERROR) << "Not a valid input, exiting program";
        google::ShutdownGoogleLogging();
        std::exit(1);
    }

    // glog cleanup
    google::ShutdownGoogleLogging();
    return 0;
}
