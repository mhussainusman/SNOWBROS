#include "Shop.h"
#include <cstring>

const ShopItemInfo Shop::sItems[ITEM_COUNT] = {
    { "EXTRA LIFE",           50  },
    { "SPEED BOOST (30s)",    20  },
    { "SNOWBALL POWER (LVL)", 30  },
    { "DISTANCE BOOST (LVL)", 25  },
    { "BALLOON MODE (30s)",   35  }
};

Shop::Shop() { reset(); }

void Shop::reset() {
    mPendingHistoryCount = 0;
    mGems = 0;
    mSpeedBoostTimer = 0.f;
    mBalloonTimer = 0.f;
    mSnowballPowerActive = false;
    mDistanceBoostActive = false;
    mExtraLifePending = false;
    mSelected = 0;
    mMessage = "";
}

// -------------------------------------------------------
// File path: data/shop_<key>.txt
// -------------------------------------------------------
string Shop::dataPath(const string& key) const {
    return "data/shop_" + key + ".txt";
}

// -------------------------------------------------------
// Shared shop — key = "user1_user2"
// -------------------------------------------------------
void Shop::loadForPlayers(const string& user1, const string& user2) {
    loadForPlayer(user1 + "_" + user2);
}

void Shop::saveForPlayers(const string& user1, const string& user2) {
    saveForPlayer(user1 + "_" + user2);
}

// -------------------------------------------------------
// Load gems from file
// File format:
//   GEMS=<n>
//   BUY=<item name>
// -------------------------------------------------------
void Shop::loadForPlayer(const string& username) {
    reset();
    ifstream f(dataPath(username));
    if (!f.is_open()) return;

    string line;
    while (getline(f, line)) {
        if (line.size() > 5 && line.substr(0, 5) == "GEMS=")
            mGems = stoi(line.substr(5));
    }
    f.close();
}

// -------------------------------------------------------
// Save gems + purchase history
// -------------------------------------------------------
void Shop::saveForPlayer(const string& username) {
    const int MAX_HISTORY = 100;
    const int LINE_LEN = 128;
    char history[MAX_HISTORY][LINE_LEN];
    int  historyCount = 0;

    {
        ifstream f(dataPath(username));
        if (f.is_open()) {
            string line;
            while (getline(f, line) && historyCount < MAX_HISTORY) {
                if (line.size() >= 4 &&
                    line[0] == 'B' && line[1] == 'U' &&
                    line[2] == 'Y' && line[3] == '=') {
                    strncpy_s(history[historyCount], LINE_LEN,
                        line.c_str(), _TRUNCATE);
                    historyCount++;
                }
            }
            f.close();
        }
    }

    ofstream f(dataPath(username));
    if (!f.is_open()) return;
    f << "GEMS=" << mGems << "\n";
    for (int i = 0; i < historyCount; i++)       // old history
        f << history[i] << "\n";
    for (int i = 0; i < mPendingHistoryCount; i++) // new purchases this session
        f << mPendingHistory[i] << "\n";
    f.close();
    mPendingHistoryCount = 0;   // clear after saving
}

// -------------------------------------------------------
// Gem economy
//   boss  kill    : caller passes +200
//   level complete: caller passes +20
// -------------------------------------------------------
void Shop::addGems(int amount) {
    mGems += amount;
    if (mGems < 0) mGems = 0;
}

int Shop::getGems() const { return mGems; }

// -------------------------------------------------------
// Purchase
// -------------------------------------------------------
bool Shop::purchase(ShopItem item) {
    int cost = sItems[item].cost;
    if (mGems < cost) {
        mMessage = "NOT ENOUGH GEMS!";
        return false;
    }
    mGems -= cost;
    mMessage = "PURCHASED: " + sItems[item].name;

    switch (item) {
    case ITEM_EXTRA_LIFE:     mExtraLifePending = true;        break;
    case ITEM_SPEED_BOOST:    mSpeedBoostTimer = 30.f;        break;
    case ITEM_SNOWBALL_POWER: mSnowballPowerActive = true;     break;
    case ITEM_DISTANCE_BOOST: mDistanceBoostActive = true;     break;
    case ITEM_BALLOON_MODE:   mBalloonTimer = 30.f;           break;
    default: break;
    }

    // Record this purchase in the pending history buffer
    if (mPendingHistoryCount < 20) {
        // Get current timestamp
        time_t now = time(nullptr);
        char timebuf[32];
        tm timeinfo;
        localtime_s(&timeinfo, &now);
        strftime(timebuf, sizeof(timebuf), "%Y-%m-%d %H:%M", &timeinfo);
        mPendingHistory[mPendingHistoryCount++] =
            "BUY=" + sItems[item].name + "|" + timebuf;
    }
    return true;
}

bool  Shop::hasExtraLifePending()   const { return mExtraLifePending; }
void  Shop::consumeExtraLife() { mExtraLifePending = false; }

bool  Shop::isSpeedBoostActive()    const { return mSpeedBoostTimer > 0.f; }
float Shop::getSpeedBoostTimeLeft() const { return mSpeedBoostTimer; }
void  Shop::updateSpeedBoost(float dt) { if (mSpeedBoostTimer > 0.f) mSpeedBoostTimer -= dt; }

bool  Shop::isSnowballPowerActive() const { return mSnowballPowerActive; }
bool  Shop::isDistanceBoostActive() const { return mDistanceBoostActive; }

bool  Shop::isBalloonModeActive()   const { return mBalloonTimer > 0.f; }
float Shop::getBalloonTimeLeft()    const { return mBalloonTimer; }
void  Shop::updateBalloonMode(float dt) { if (mBalloonTimer > 0.f) mBalloonTimer -= dt; }

void Shop::onNewLevel() {
    mSnowballPowerActive = false;
    mDistanceBoostActive = false;
}

const ShopItemInfo& Shop::getItemInfo(ShopItem item) const {
    return sItems[item];
}