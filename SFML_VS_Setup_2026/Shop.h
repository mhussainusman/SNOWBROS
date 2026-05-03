#pragma once
#include <string>
#include <fstream>
#include <sstream>
using namespace std;

enum ShopItem {
    ITEM_EXTRA_LIFE = 0,  // 50 gems
    ITEM_SPEED_BOOST = 1,  // 20 gems
    ITEM_SNOWBALL_POWER = 2,  // 30 gems
    ITEM_DISTANCE_BOOST = 3,  // 25 gems
    ITEM_BALLOON_MODE = 4,  // 35 gems
    ITEM_COUNT = 5
};

struct ShopItemInfo {
    string name;
    int    cost;
    string effect;
};

class Shop {
public:
    Shop();

    // Single shared shop saved under combined key "user1_user2"
    void  loadForPlayers(const string& user1, const string& user2);
    void  saveForPlayers(const string& user1, const string& user2);

    // Legacy single-player wrappers (kept for compatibility)
    void  loadForPlayer(const string& username);
    void  saveForPlayer(const string& username);

    // Gem economy: enemy +20, boss +50, level complete +10
    void  addGems(int amount);
    int   getGems() const;

    bool  purchase(ShopItem item);

    bool  hasExtraLifePending() const;
    void  consumeExtraLife();

    bool  isSpeedBoostActive()    const;
    float getSpeedBoostTimeLeft() const;
    void  updateSpeedBoost(float dt);

    bool  isSnowballPowerActive() const;
    bool  isDistanceBoostActive() const;

    bool  isBalloonModeActive()   const;
    float getBalloonTimeLeft()    const;
    void  updateBalloonMode(float dt);

    void  onNewLevel();

    const ShopItemInfo& getItemInfo(ShopItem item) const;
    int    getSelectedIndex() const { return mSelected; }
    void   setSelectedIndex(int i) { mSelected = i; }
    string getLastMessage()   const { return mMessage; }

private:
    int   mGems;
    float mSpeedBoostTimer;
    float mBalloonTimer;
    bool  mSnowballPowerActive;
    bool  mDistanceBoostActive;
    bool  mExtraLifePending;
    int   mSelected;
    string mMessage;

    string mPendingHistory[20];   // items bought this session, not yet saved
    int    mPendingHistoryCount;

    static const ShopItemInfo sItems[ITEM_COUNT];

    void   reset();
    string dataPath(const string& key) const;
};