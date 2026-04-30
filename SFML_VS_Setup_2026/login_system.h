#pragma once  // prevents double-include in same translation unit
#include <string>

// Stores a single player's account info
struct PlayerAccount {
    std::string username;
    std::string passwordHash;
    int highScore = 0;
    int gamesPlayed = 0;
};

// Declaration only — defined in AccountManager.cpp to avoid
// "already has a body" error when multiple files include this header
std::string hashPassword(const std::string& pw);