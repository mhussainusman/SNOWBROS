#pragma once
#include <SFML/Graphics.hpp>
#include "Platform.h"
#include "Enemy.h"

// LevelManager: reads level data from config files
// loads platforms and enemies for each level
// no hardcoded level counts anywhere
class LevelManager {
public:
    LevelManager();

    // load a level from file
    // returns true if successful, false if file not found
    bool loadLevel(int levelNumber,
        Platform* platforms, int& platformCount,
        Enemy** enemies, int& enemyCount);

    // returns total number of levels available
    int getTotalLevels() const;

    // returns true if this level is a boss level
    bool isBossLevel(int levelNumber) const;

    // returns true if this level is a bonus level
    bool isBonusLevel(int levelNumber) const;

    // call at game start to randomly pick 2 bonus levels
    void selectBonusLevels();

private:
    int mTotalLevels;       // read from config, not hardcoded
    int mBonusLevel1;       // randomly selected bonus level
    int mBonusLevel2;       // randomly selected bonus level

    // creates correct enemy type from string name
    Enemy* createEnemy(const std::string& type, float x, float y);
};