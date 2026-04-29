#pragma once
#include <SFML/Graphics.hpp>
#include "Platform.h"

// Player: handles movement, gravity, jumping and collision
class Player {
public:

    Player(int playerIndex);// 0: player 1, 1: player 2

    void update(float deltaTime, const Platform* platforms, int platformCount);

    // showHitbox: when true draws green outline showing collision area
    void draw(sf::RenderWindow& window, bool showHitbox);

    sf::Vector2f getPosition() const;
    sf::FloatRect getBounds() const;
    bool isFacingRight() const; // used by snowball to know which direction to travel

    // called by Game when player presses throw button
// returns true if a snowball should be spawned
    bool wantsToThrow();

    // lives system
    int getLives() const;        // returns current lives
    void loseLife();             // called when player touches enemy
    bool isAlive() const;        // true when lives > 0
    bool isRespawning() const;   // true during respawn invincibility period
    void resetLives();

private:
    void handleCollision(const Platform* platforms, int platformCount);

    sf::RectangleShape mHitbox;   // collision box
    sf::RectangleShape mVisual;   // placeholder visual, replaced with sprite later

    float mSpeed;
    float mVelocityY;
    bool mIsOnGround;
    bool mFacingRight;  // true when player is facing right

    int mPlayerIndex;
    bool mThrowKeyHeld; // prevents holding key expecting repetitive throw

    const float GRAVITY = 900.f;
    const float JUMP_FORCE = -480.f;
    const float MAX_FALL_SPEED = 600.f;

    int mLives;                  // current lives remaining
    float mRespawnTimer;         // counts down after death
    float mRespawnTime;          // how long respawn invincibility lasts
    bool mRespawning;            // true during invincibility period

    // starting position — used for respawning
    float mStartX;
    float mStartY;


};