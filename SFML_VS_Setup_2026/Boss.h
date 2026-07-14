#pragma once
#include "Enemy.h"
#include <vector>

// ─────────────────────────────────────────────
// BOSS — base class for all bosses
// inherits from Enemy so it fits in mEnemies array
// adds health pool, health bar, phase system, gem drop
// ─────────────────────────────────────────────
class Boss : public Enemy {
public:
    Boss(float x, float y, int maxHealth, int gemReward);

    // every boss must implement these
    virtual void update(float deltaTime,
        const Platform* platforms, int platformCount) = 0;
    virtual void draw(sf::RenderWindow& window, bool showHitbox) = 0;
    virtual int  getPoints() const = 0;

    // health system
    void takeBossHit();           // called when snowball hits boss
    bool isDefeated()    const;   // true when health reaches 0
    int  getGemReward()  const;   // gems dropped on defeat
    int  getHealth()     const;   // current health
    int  getMaxHealth()  const;   // max health for health bar

    // boss draws its own health bar
    void drawHealthBar(sf::RenderWindow& window);

protected:
    int mHealth;       // current health
    int mMaxHealth;    // starting health
    int mGemReward;    // gems awarded when defeated
    int mPhase;        // 1 = normal, 2 = enraged (50% health)

    // boss specific constants
    static constexpr float SCREEN_W = 800.f;
    static constexpr float SCREEN_H = 660.f;
};

// ─────────────────────────────────────────────
// MOGERA CHILD — small enemy spawned by Mogera
// now behaves like a real enemy: walks, bounces off screen
// walls, jumps periodically, can be snowballed/encased/kicked
// same as any other ground enemy — no longer despawns off-screen
// ─────────────────────────────────────────────
class MogeraChild : public Enemy {
public:
    MogeraChild(float x, float y, bool movingRight);

    void update(float deltaTime,
        const Platform* platforms, int platformCount) override;
    void draw(sf::RenderWindow& window, bool showHitbox) override;
    int  getPoints() const override;

private:
    float mMoveSpeed;
    bool  mMovingRight;

    float mJumpTimer;      // counts time since last jump
    float mJumpInterval;   // how often it jumps — randomized each hop
    const float JUMP_FORCE = -480.f;
};

// ─────────────────────────────────────────────
// MOGERA — Level 5 boss
// mostly stationary, spawns MogeraChild enemies
// takes multiple snowball hits to defeat
// ─────────────────────────────────────────────
class Mogera : public Boss {
public:
    Mogera(float x, float y);

    void update(float deltaTime,
        const Platform* platforms, int platformCount) override;
    void draw(sf::RenderWindow& window, bool showHitbox) override;
    int  getPoints() const override;

    // Game calls this to collect newly spawned children
    bool hasNewChild() const { return mChildReady; }
    MogeraChild* takeChild();
    bool shouldDieOnHit() const { return true; }
private:
    float mSpawnTimer;      // counts time between child spawns
    float mSpawnInterval;   // how often a child spawns
    bool  mChildReady;      // true when a child is waiting to be collected
    MogeraChild* mPendingChild; // the child waiting to be added to game
};

// ─────────────────────────────────────────────
// GAMAKICHI PROJECTILE — rocket fired by Gamakichi
// travels in a straight line, explodes on contact
// ─────────────────────────────────────────────
struct GamakichiRocket {
    sf::Sprite   sprite;
    sf::Vector2f velocity;
    bool         active = true;
};

// ─────────────────────────────────────────────
// GAMAKICHI — Level 10 boss
// fires rockets in multiple directions
// two attack phases — phase 2 at 50% health
// ─────────────────────────────────────────────
class Gamakichi : public Boss {
public:
    Gamakichi(float x, float y);

    void update(float deltaTime,
        const Platform* platforms, int platformCount) override;
    void draw(sf::RenderWindow& window, bool showHitbox) override;
    int  getPoints() const override;

    // Game collects rockets every frame
    bool hasNewRocket()    const { return mRocketReady; }
    GamakichiRocket takeRocket();

    // check if a rocket hit the player
    // Game passes player bounds to check
    bool checkRocketHit(sf::FloatRect playerBounds);

private:
    float mFireTimer;        // counts time between shots
    float mFireInterval;     // how often rockets fire
    bool  mRocketReady;
    GamakichiRocket mPendingRocket;

    sf::Vector2f mPlayerPos; // nearest player position for aiming
    int mRocketsFired;       // tracks how many rockets fired this burst

    // active rockets on screen
    GamakichiRocket mRockets[20];
    int mRocketCount;

    void fireRocket(sf::Vector2f direction);

public:
    void setPlayerPos(sf::Vector2f pos) { mPlayerPos = pos; }
    void updateRockets(float deltaTime);
    void drawRockets(sf::RenderWindow& window);
    int  getRocketCount() const { return mRocketCount; }
    GamakichiRocket& getRocket(int i) { return mRockets[i]; }
};