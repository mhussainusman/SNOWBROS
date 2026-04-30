#pragma once
#include <SFML/Graphics.hpp>
#include <fstream>
#include <sstream>
#include <string>
#include <ctime>
#include <iomanip>
using namespace std;

// ---------------------------------------------------------------
//  One entry in the leaderboard
// ---------------------------------------------------------------
struct LeaderboardEntry {
    string username;
    int    score;
    int    level;
    string timestamp;   // "YYYY-MM-DD HH:MM"
};

// ---------------------------------------------------------------
//  Leaderboard  —  reads/writes  data/leaderboard.txt
//
//  File format (one record per line, pipe-separated):
//      username|score|level|timestamp
//
//  Entries are kept sorted descending by score at all times.
//  Maximum MAX_ENTRIES rows are stored.
// ---------------------------------------------------------------
class Leaderboard {
public:
    static const int MAX_ENTRIES = 20;   // cap stored in file
    static const int DISPLAY_ROWS = 10;  // rows shown on screen

    Leaderboard();

    // ---- data API ------------------------------------------------
    // Add a new result; re-sorts and trims to MAX_ENTRIES, then saves.
    void addEntry(const string& username, int score, int level);

    // Load from file (called in constructor & whenever needed).
    void load();

    // Save to file.
    void save() const;

    // How many entries are currently loaded.
    int  getCount() const { return mCount; }

    // Read-only access to a single entry (0 = highest score).
    const LeaderboardEntry& getEntry(int i) const { return mEntries[i]; }

    // Rank of given username by score (-1 if not present).
    int  getRankOf(const string& username) const;

    // ---- SFML rendering -----------------------------------------
    // Call once after loading your font.
    void setFont(const sf::Font& font);

    // Draw the full leaderboard screen (call when mState == LEADERBOARD).
    // Returns true if the user pressed Escape / Backspace to go back.
    bool draw(sf::RenderWindow& window, float deltaTime);

private:
    // ---- helpers -------------------------------------------------
    string currentTimestamp() const;
    void   sortEntries();          // insertion-sort descending by score
    void   trimToMax();

    // ---- data ----------------------------------------------------
    LeaderboardEntry mEntries[MAX_ENTRIES];
    int              mCount;
    string           mFilePath;

    // ---- rendering -----------------------------------------------
    sf::Font         mFont;
    bool             mFontLoaded;

    // scroll / animation
    int              mScrollOffset;   // first visible row index
    float            mAnimTimer;      // drives subtle pulse on #1 row
    float            mBgScrollX;      // slow horizontal star-scroll

    // snowflake decoration
    struct Flake { float x, y, speed, size; };
    static const int FLAKE_COUNT = 40;
    Flake            mFlakes[FLAKE_COUNT];

    void initFlakes();
    void updateFlakes(float dt);
    void drawBackground(sf::RenderWindow& w, float dt);
    void drawTitle(sf::RenderWindow& w);
    void drawTable(sf::RenderWindow& w);
    void drawScrollHints(sf::RenderWindow& w);
    void drawFooter(sf::RenderWindow& w);

    // key repeat for scrolling
    float mScrollCooldown;
};