#pragma once

#include "Server.h"

class User
{
    // A user that can connect to the server
public:
    std::string nickname;
    networking::Connection userConnection;  

    User();
    User(networking::Connection c);
    networking::Connection getConnection();
    void setConnection(networking::Connection c) { userConnection = c; };
    bool operator==(User other) const { return userConnection.id == other.userConnection.id; };
};

