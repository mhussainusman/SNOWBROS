#pragma once
#include <SFML/Graphics.hpp>
#include "Platform.h"


class Player {
public:

    Player(int playerIndex);

    void addLife();

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

    // for power ups
    void setSpeedBoost(bool active);
    void setBalloonMode(bool active);

    // called once from Character Select when a player confirms their pick
    // 0 = blue, 1 = green, 2 = red
    void setCharacterSprite(int characterIndex);

    bool isInBalloonMode() const { return mBalloonMode; }

private:
    void handleCollision(const Platform* platforms, int platformCount);
    void handleBalloonCollision(); // screen edge bounce only
    void syncSpritePosition();     // keeps sprite centered on mHitbox every frame

    sf::RectangleShape mHitbox;
    sf::Texture mTexture;
    sf::Sprite  mSprite;
    bool        mTextureLoaded;

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

    float mBaseScaleX = 1.f;   // scale needed to fit sprite into mHitbox width
    float mBaseScaleY = 1.f;   // scale needed to fit sprite into mHitbox height


};