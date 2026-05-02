#pragma once
#include <SFML/Graphics.hpp>
#include "Platform.h"

class Player {
public:
    Player(int playerIndex);

    void update(float deltaTime,
        const Platform* platforms, int platformCount);
    void draw(sf::RenderWindow& window, bool showHitbox);

    sf::Vector2f  getPosition()    const;
    sf::FloatRect getBounds()      const;
    bool          isFacingRight()  const;
    bool          wantsToThrow();

    int  getLives()       const;
    void loseLife();
    bool isAlive()        const;
    bool isRespawning()   const;
    void resetLives();

    // power ups
    void setSpeedBoost(bool active);
    void setBalloonMode(bool active);
    void addLife();
    bool isInBalloonMode() const { return mBalloonMode; }

private:
    void handleCollision(const Platform* platforms, int platformCount);
    void handleBalloonCollision(); // screen edge bounce only

    sf::RectangleShape mHitbox;
    sf::RectangleShape mVisual;

    float mSpeed;
    float mVelocityY;
    float mVelocityX;   // NEW — needed for balloon free movement
    bool  mIsOnGround;
    bool  mFacingRight;

    int  mPlayerIndex;
    bool mThrowKeyHeld;

    const float GRAVITY = 900.f;
    const float JUMP_FORCE = -480.f;
    const float MAX_FALL_SPEED = 600.f;

    // balloon movement speed — same as FlyingEnemy
    const float BALLOON_SPEED = 180.f;

    int   mLives;
    float mRespawnTimer;
    float mRespawnTime;
    bool  mRespawning;

    float mStartX;
    float mStartY;

    bool mSpeedBoosted;
    bool mBalloonMode;
};