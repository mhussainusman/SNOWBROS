#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Platform.h"

// Enemy: abstract base class — all enemies inherit from this
// any class with = 0 functions cannot be created directly
class Enemy {
public:

    // constructor: sets starting position and how many hits to encase
    Enemy(float x, float y, int hitsToEncase);

    // pure virtual functions — every enemy MUST override these
    virtual void update(float deltaTime, const std::vector<Platform>& platforms) = 0;
    virtual void draw(sf::RenderWindow& window) = 0;

    // these are the same for all enemies so we write them once here
    void takeDamage();           // called when snowball hits enemy
    bool isFullyEncased() const; // true when snow coat is full
    bool isDefeated() const;     // true when rolled into another enemy
    bool isDead() const;         // true when enemy should be removed

    sf::FloatRect getBounds() const; // used for collision detection

protected:
    // protected means child classes CAN access these
    // but outside code cannot

    sf::RectangleShape mShape;  // visual rectangle for now
    float mVelocityY;           // vertical speed for gravity
    bool mIsOnGround;           // is enemy standing on a platform
    bool mDefeated;             // has been rolled into another enemy
    bool mDead;                 // should be removed from game

    int mSnowHits;              // how many times hit by snowball so far
    int mHitsToEncase;          // how many hits needed to fully encase
    bool mEncased;              // is enemy fully covered in snow

    // physics constants same for all enemies
    const float GRAVITY = 900.f;
    const float MAX_FALL_SPEED = 600.f;

    // shared gravity function — enemies fall same way as player
    void applyGravity(float deltaTime, const std::vector<Platform>& platforms);
};

// Botom: simplest enemy, walks left and right, falls with gravity
// inherits from Enemy
class Botom : public Enemy {
public:
    Botom(float x, float y);

    // overriding the pure virtual functions from Enemy
    void update(float deltaTime, const std::vector<Platform>& platforms) override;
    void draw(sf::RenderWindow& window) override;

private:
    float mMoveSpeed;    // how fast Botom walks
    bool mMovingRight;   // which direction currently walking
    float mDirectionTimer;   // how long until Botom changes direction
    float mDirectionInterval; // how many seconds between direction changes
};
