#pragma once
#include <SFML/Graphics.hpp>
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
    virtual void update(float deltaTime, const Platform* platforms, int platformCount) = 0;
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
    void startRolling(bool kickedRight, int playerIndex=0);

    // handles rolling movement every frame
    void updateRolling(float deltaTime, const Platform* platforms, int platformCount);

    bool isRollingRight() const; // returns which direction rolling
    void setDead();              // instantly kills this enemy

    virtual int getPoints() const = 0;

    bool isPartiallyEncased() const; // true when hit once but not fully encased

    int getKickedByPlayer() const;

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
  
  void applyGravity(float deltaTime, const Platform* platforms, int platformCount);
  int mKickedByPlayer;

};


// Botom: basic enemy
// walks left and right, falls with gravity, turns at walls
// inherits from Enemy
class Botom : public Enemy {
public:
    Botom(float x, float y, int variant=0);

    // overriding pure virtual functions from Enemy
    void update(float deltaTime, const Platform* platforms, int platformCount) override;
    void draw(sf::RenderWindow& window, bool showHitbox) override;

    int getPoints() const override;


private:
    float mMoveSpeed;          // how fast Botom walks
    bool mMovingRight;         // current walking direction
    float mJumpTimer;          // counts time since last jump
    float mJumpInterval;       // how often enemy jumps
  

    const float JUMP_FORCE = -520.f;

};
// ------Flying Enemy 

class FlyingEnemy : public Botom {
public:
    FlyingEnemy(float x, float y);

    void update(float deltaTime,
        const Platform* platforms, int platformCount) override;
    void draw(sf::RenderWindow& window, bool showHitbox) override;
    int getPoints() const override;

protected:
    bool mFlying;           // true when in flight mode
    float mFlightTimer;     // counts time in current state
    float mFlightInterval;  // how long before taking flight
    float mFlightDuration;  // how long flight lasts
    float mFlightSpeedX;    // horizontal speed while flying
    float mFlightSpeedY;    // vertical speed while flying
};

// -----Tornado Enermy---------

// Tornado: inherits FlyingEnemy
// faster flight, throws knife at nearest player
class Tornado : public FlyingEnemy {
public:
    Tornado(float x, float y);

    void update(float deltaTime,
        const Platform* platforms, int platformCount) override;
    void draw(sf::RenderWindow& window, bool showHitbox) override;
    int getPoints() const override;

    // Game passes nearest player position every frame
    void setNearestPlayerPos(sf::Vector2f pos);

    // returns knife hitbox for collision with player
    sf::FloatRect getKnifeBounds() const;
    bool isKnifeActive() const;

private:
    // knife variables — no separate class needed
    sf::RectangleShape mKnifeVisual;  // yellow rectangle
    sf::Vector2f mKnifeDirection;     // normalized direction
    bool mKnifeActive;                // is knife flying right now
    float mKnifeSpeed;                // how fast knife travels
    float mKnifeTimer;                // counts time since last throw
    float mKnifeInterval;             // seconds between throws

    sf::Vector2f mNearestPlayerPos;   // where to aim
};