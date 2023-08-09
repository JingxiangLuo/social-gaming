#include "User.h"
#include "Server.h"

User::User(networking::Connection c) {
    userConnection = c;
    nickname = std::to_string(userConnection.id);
};

networking::Connection
User::getConnection() {
    return userConnection;
}
