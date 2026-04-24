#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Platform.h"

// Enemy: abstract base class
// all enemies inherit from this
// cannot be created directly because of = 0 functions
class Enemy {
public:

    // x, y = starting position
    // hitsToEncase = snowballs needed to fully cover this enemy
    Enemy(float x, float y, int hitsToEncase);

    // pure virtual: every enemy must write their own version
    virtual void update(float deltaTime, const std::vector<Platform>& platforms) = 0;
    // draw: second parameter indicates         whether to render hitboxes for debugging
    virtual void draw(sf::RenderWindow& window, bool showHitbox) = 0;

    // same for all enemies, written once here
    void takeDamage();
    bool isFullyEncased() const;
    bool isDead() const;
    bool isRolling() const;

    // hitbox for collision detection — separate from visual
    sf::FloatRect getBounds() const;

    void updateMelt(float deltaTime);
    // called when player stands next to encased enemy and presses kick button
    void startRolling(bool kickedRight);

    // handles rolling movement every frame
    void updateRolling(float deltaTime, const std::vector<Platform>& platforms);

    bool isRollingRight() const; // returns which direction rolling
    void setDead();              // instantly kills this enemy

protected:

    // hitbox: used for all collision detection and physics
    // this never changes size — always accurate hit area
    sf::RectangleShape mHitbox;

    // visual: what gets drawn on screen
    // currently a colored rectangle — later replaced with sprite
    // can be bigger than hitbox
    sf::RectangleShape mVisual;

    float mVelocityY;       // vertical speed: positive=falling, negative=rising
    bool mIsOnGround;       // true when standing on a platform
    bool mDead;             // true when should be removed from game

    int mSnowHits;          // how many snowballs have hit so far
    int mHitsToEncase;      // how many hits needed to fully encase
    bool mEncased;          // true when fully covered in snow
    bool mRolling;          // true when player has kicked this enemy
    float mRollSpeed;       // how fast it rolls when kicked
    bool mRollingRight;     // which direction it is rolling

    float mMeltTimer;         // counts how long enemy has been encased
    float mMeltTime;          // how long until enemy melts (3 or 6 seconds)
    sf::Color mOriginalColor; // stores original color to restore after melting

    const float GRAVITY = 900.f;
    const float MAX_FALL_SPEED = 600.f;

    // gravity and platform landing — shared by all enemies
    void applyGravity(float deltaTime, const std::vector<Platform>& platforms);
};


// Botom: basic enemy
// walks left and right, falls with gravity, turns at walls
// inherits from Enemy
class Botom : public Enemy {
public:
    Botom(float x, float y);

    // overriding pure virtual functions from Enemy
    void update(float deltaTime, const std::vector<Platform>& platforms) override;
    void draw(sf::RenderWindow& window, bool showHitbox) override;

    void setPlayerPosition(sf::Vector2f playerPos);


private:
    float mMoveSpeed;          // how fast Botom walks
    bool mMovingRight;         // current walking direction
    float mDirectionTimer;     // counts time since last direction change
    float mDirectionInterval;  // seconds until next direction change

    sf::Vector2f mPlayerPos;   // where the player currently is
    float mJumpTimer;          // counts time since last jump
    float mJumpInterval;       // how often enemy jumps
    float mVelocityX;          // horizontal velocity
    bool mCanJump;             // true when on ground and can jump

    const float JUMP_FORCE = -520.f; 

};