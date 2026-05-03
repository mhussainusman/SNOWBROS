#pragma once
#include "login_system.h"
#include <fstream>
#include <sstream>
#include <string>

// ---------------------------------------------------------------
// AccountManager
// Plain array storage — no map, vector, or any banned STL.
// File format (accounts.dat), one line per player:
//   username|passwordHash|highScore|gamesPlayed
// ---------------------------------------------------------------
class Account_Manager {
public:
    static constexpr const char* DB_PATH = "accounts.dat";
    static constexpr int         MAX_USERS = 100;

    enum class Result {
        OK,
        WRONG_PASSWORD,
        USER_NOT_FOUND,
        USER_ALREADY_EXISTS,
        EMPTY_FIELD,
        DB_FULL
    };

    Account_Manager() : mCount(0) {
        loadFromFile();
    }

    // ---- LOGIN --------------------------------------------------
    Result tryLogin(const std::string& username,
        const std::string& password,
        PlayerAccount& outAccount)
    {
        if (username.empty() || password.empty())
            return Result::EMPTY_FIELD;

        int idx = findIndex(username);
        if (idx == -1)
            return Result::USER_NOT_FOUND;

        if (mAccounts[idx].passwordHash != hashPassword(password))
            return Result::WRONG_PASSWORD;

        outAccount = mAccounts[idx];
        return Result::OK;
    }

    // ---- REGISTER -----------------------------------------------
    Result registerUser(const std::string& username,
        const std::string& password)
    {
        if (username.empty() || password.empty())
            return Result::EMPTY_FIELD;

        if (findIndex(username) != -1)
            return Result::USER_ALREADY_EXISTS;

        if (mCount >= MAX_USERS)
            return Result::DB_FULL;

        PlayerAccount acc;
        acc.username = username;
        acc.passwordHash = hashPassword(password);
        acc.highScore = 0;
        acc.gamesPlayed = 0;

        mAccounts[mCount++] = acc;
        saveToFile();
        return Result::OK;
    }

    // ---- SAVE PROGRESS ------------------------------------------
    void saveProgress(const std::string& username, int score) {
        int idx = findIndex(username);
        if (idx == -1) return;

        mAccounts[idx].gamesPlayed++;
        if (score > mAccounts[idx].highScore)
            mAccounts[idx].highScore = score;

        saveToFile();
    }

    // ---- LEADERBOARD --------------------------------------------
    // Pass in a caller-owned array of size MAX_USERS.
    // Returns how many entries were filled.
    // Example:
    //   PlayerAccount board[AccountManager::MAX_USERS];
    //   int count = mAccountManager.getLeaderboard(board);
    int getLeaderboard(PlayerAccount outArray[]) const {
        for (int i = 0; i < mCount; i++)
            outArray[i] = mAccounts[i];

        // Bubble sort — descending by highScore (no <algorithm> needed)
        for (int i = 0; i < mCount - 1; i++)
            for (int j = 0; j < mCount - i - 1; j++)
                if (outArray[j].highScore < outArray[j + 1].highScore) {
                    PlayerAccount tmp = outArray[j];
                    outArray[j] = outArray[j + 1];
                    outArray[j + 1] = tmp;
                }

        return mCount;
    }

    int getCount() const { return mCount; }

private:
    PlayerAccount mAccounts[MAX_USERS];
    int           mCount;

    // Linear search — returns index or -1
    int findIndex(const std::string& username) const {
        for (int i = 0; i < mCount; i++)
            if (mAccounts[i].username == username)
                return i;
        return -1;
    }

    void loadFromFile() {
        std::ifstream file(DB_PATH);
        if (!file.is_open()) return;

        std::string line;
        while (std::getline(file, line) && mCount < MAX_USERS) {
            if (line.empty()) continue;
            PlayerAccount acc;
            if (parseLine(line, acc))
                mAccounts[mCount++] = acc;
        }
    }

    void saveToFile() const {
        std::ofstream file(DB_PATH, std::ios::trunc);
        for (int i = 0; i < mCount; i++) {
            file << mAccounts[i].username << "|"
                << mAccounts[i].passwordHash << "|"
                << mAccounts[i].highScore << "|"
                << mAccounts[i].gamesPlayed << "\n";
        }
    }

    // Splits on '|' using sstream — no vector needed
    static bool parseLine(const std::string& line, PlayerAccount& out) {
        std::istringstream ss(line);
        std::string parts[4];
        int n = 0;
        std::string token;
        while (n < 4 && std::getline(ss, token, '|'))
            parts[n++] = token;

        if (n != 4) return false;

        out.username = parts[0];
        out.passwordHash = parts[1];
        out.highScore = std::stoi(parts[2]);
        out.gamesPlayed = std::stoi(parts[3]);
        return true;
    }
};