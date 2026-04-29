#include "LevelManager.h"
#include "Enemy.h"
#include <fstream>
#include <string>

LevelManager::LevelManager()
    : mTotalLevels(0),
    mBonusLevel1(-1),
    mBonusLevel2(-1) {

    // count how many level files exist
    // keeps trying until file not found
    // this is how we avoid hardcoding level count
    for (int i = 1; i <= 100; i++) {
        std::string filename = "data/level" +
            std::to_string(i) + ".txt";
        std::ifstream file(filename);
        if (!file.is_open()) break;
        mTotalLevels++;
        file.close();
    }
}

int LevelManager::getTotalLevels() const {
    return mTotalLevels;
}

bool LevelManager::isBossLevel(int levelNumber) const {
    // levels 5 and 10 are boss levels
    return levelNumber == 5 || levelNumber == 10;
}

bool LevelManager::isBonusLevel(int levelNumber) const {
    return levelNumber == mBonusLevel1 ||
        levelNumber == mBonusLevel2;
}

void LevelManager::selectBonusLevels() {
    // randomly pick 2 non-boss levels as bonus levels
    // called once at game start

    // collect all non-boss level numbers
    int nonBossLevels[100];
    int count = 0;

    for (int i = 1; i <= mTotalLevels; i++) {
        if (!isBossLevel(i))
            nonBossLevels[count++] = i;
    }

    // pick 2 random ones
    if (count >= 2) {
        int idx1 = rand() % count;
        int idx2;
        do {
            idx2 = rand() % count;
        } while (idx2 == idx1); // make sure they are different

        mBonusLevel1 = nonBossLevels[idx1];
        mBonusLevel2 = nonBossLevels[idx2];
    }
}

bool LevelManager::loadLevel(int levelNumber,
    Platform* platforms, int& platformCount,
    Enemy** enemies, int& enemyCount) {

    // build file path
    std::string filename = "data/level" +
        std::to_string(levelNumber) + ".txt";

    std::ifstream file(filename);
    if (!file.is_open()) return false; // file not found

    // reset counts
    platformCount = 0;
    enemyCount = 0;

    std::string token;

    while (file >> token) {

        // skip comment lines starting with //
        if (token == "//") {
            std::string line;
            std::getline(file, line);
            continue;
        }

        if (token == "PLATFORM") {
            float x, y, width, height;
            file >> x >> y >> width >> height;
            platforms[platformCount++] =
                Platform(x, y, width, height);
        }
        else if (token == "ENEMY") {
            std::string type;
            float x, y;
            file >> type >> x >> y;

            Enemy* e = createEnemy(type, x, y);
            if (e != nullptr)
                enemies[enemyCount++] = e;
        }
    }

    file.close();
    return true;
}

Enemy* LevelManager::createEnemy(const std::string& type,float x, float y) 
{
    // factory pattern — creates right enemy from string name
    // this is the EnemyFactory design pattern your spec requires
    if (type == "Botom")
        return new Botom(x, y);
    if (type == "BotomGreen")
        return new Botom(x, y, 1); // variant 1 = green
    if (type == "BotomBlue")
        return new Botom(x, y, 2); // variant 2 = blue
    if (type == "BotomPurple")
        return new Botom(x, y, 3); // variant 3 = purple
    if (type == "FlyingEnemy")
        return new FlyingEnemy(x, y);
    if (type == "Tornado")
        return new Tornado(x, y);

    return nullptr; // unknown type
}