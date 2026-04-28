#include "Player.h"

Player::Player(int playerIndex)
    : mSpeed(180.f),
    mVelocityY(0.f),
    mIsOnGround(false),
    mFacingRight(true),
    mPlayerIndex(playerIndex),
    mThrowKeyHeld(false),
    mLives(3),            // 3 total = 2 shown in HUD + 1 base life
    mRespawnTimer(0.f),
    mRespawnTime(3.f),    // 3 seconds invincibility after respawn
    mRespawning(false),
    mStartX(0.f),
    mStartY(0.f) {

    mHitbox.setSize(sf::Vector2f(40.f, 50.f));
    mVisual.setSize(sf::Vector2f(40.f, 50.f));
    mHitbox.setFillColor(sf::Color::Transparent);

    // player 1 respawns at midpoint of right half of ground
    // player 2 respawns at midpoint of left half of ground
    if (mPlayerIndex == 0) {
        mStartX = 600.f;
        mStartY = 490.f;
        mVisual.setFillColor(sf::Color(100, 180, 255)); // light blue
    }
    else {
        mStartX = 200.f;
        mStartY = 490.f;
        mVisual.setFillColor(sf::Color(255, 165, 0)); // light green
    }

    mHitbox.setPosition(mStartX, mStartY);
    mVisual.setPosition(mStartX, mStartY);
}

void Player::update(float deltaTime,
    const Platform* platforms,int platformCount) {

    // if no lives left stop updating completely
    if (mLives <= 0) return;

    // respawn invincibility timer
    // counts up every frame until 3 seconds passed
    if (mRespawning) {
        mRespawnTimer += deltaTime;
        if (mRespawnTimer >= mRespawnTime) {
            mRespawning = false;   // invincibility over
            mRespawnTimer = 0.f;   // reset for next death
        }
        // player can still move during invincibility
        // just cannot lose life
    }

    // horizontal movement
    float moveX = 0.f;

    if (mPlayerIndex == 0) {
        // player 1 — arrow keys
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            moveX = -mSpeed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            moveX = mSpeed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && mIsOnGround) {
            mVelocityY = JUMP_FORCE;
            mIsOnGround = false;
        }
    }
    else {
        // player 2 — WASD
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
            moveX = -mSpeed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
            moveX = mSpeed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && mIsOnGround) {
            mVelocityY = JUMP_FORCE;
            mIsOnGround = false;
        }
    }

    // track facing direction for snowball
    if (moveX > 0) mFacingRight = true;
    if (moveX < 0) mFacingRight = false;

    mHitbox.move(moveX * deltaTime, 0.f);
    mVisual.move(moveX * deltaTime, 0.f);

    // gravity
    mVelocityY += GRAVITY * deltaTime;
    if (mVelocityY > MAX_FALL_SPEED)
        mVelocityY = MAX_FALL_SPEED;

    mHitbox.move(0.f, mVelocityY * deltaTime);
    mVisual.move(0.f, mVelocityY * deltaTime);

    // platform collision
    handleCollision(platforms, platformCount);

    // screen edges
    sf::Vector2f pos = mHitbox.getPosition();
    if (pos.x < 0.f) {
        mHitbox.setPosition(0.f, pos.y);
        mVisual.setPosition(0.f, pos.y);
    }
    if (pos.x + mHitbox.getSize().x > 800.f) {
        mHitbox.setPosition(800.f - mHitbox.getSize().x, pos.y);
        mVisual.setPosition(800.f - mHitbox.getSize().x, pos.y);
    }

    // track throw key
    bool throwPressed = false;
    if (mPlayerIndex == 0)
        throwPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Down);
    else
        throwPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::S);

    if (!throwPressed)
        mThrowKeyHeld = false;
}

bool Player::wantsToThrow() {
    // if dead dont throw
    if (mLives <= 0) return false;

    bool throwPressed = false;
    if (mPlayerIndex == 0)
        throwPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Down);
    else
        throwPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::S);

        if (throwPressed && !mThrowKeyHeld) {
        mThrowKeyHeld = true;
        return true;
    }
    return false;
}

void Player::loseLife() {
    // ignore if already respawning — still invincible
    if (mRespawning) return;

    // ignore if already dead
    if (mLives <= 0) return;

    mLives--;

    if (mLives > 0) {
        // still has lives — respawn at starting position
        mHitbox.setPosition(mStartX, mStartY);
        mVisual.setPosition(mStartX, mStartY);
        mVelocityY = 0.f;
        mRespawnTimer = 0.f;  // reset timer from zero
        mRespawning = true;   // start invincibility
    }
    else {
        // no lives left — move off screen so player disappears
        mHitbox.setPosition(-200.f, -200.f);
        mVisual.setPosition(-200.f, -200.f);
        mVelocityY = 0.f;
    }
}

int Player::getLives() const {
    return mLives;
}

bool Player::isAlive() const {
    return mLives > 0;
}

bool Player::isRespawning() const {
    return mRespawning;
}

void Player::handleCollision(const Platform* platforms, int platformCount) {
    mIsOnGround = false;

    for (int i = 0; i < platformCount; i++) {
        sf::FloatRect player = mHitbox.getGlobalBounds();
        sf::FloatRect plat = platforms[i].getBounds();

        if (!player.intersects(plat))
            continue;

        float overlapTop = (player.top + player.height) - plat.top;

        if (mVelocityY >= 0 && overlapTop > 0 && overlapTop < 50.f) {
            mHitbox.move(0.f, -overlapTop);
            mVisual.move(0.f, -overlapTop);
            mVelocityY = 0.f;
            mIsOnGround = true;
        }
    }
}

void Player::draw(sf::RenderWindow& window, bool showHitbox) {

    // if dead dont draw anything
    if (mLives <= 0) return;

    if (!showHitbox) {
        window.draw(mVisual);
    }
    else {
        sf::RectangleShape debugBox;
        debugBox.setPosition(mHitbox.getPosition());
        debugBox.setSize(mHitbox.getSize());
        debugBox.setFillColor(sf::Color::Transparent);
        debugBox.setOutlineColor(sf::Color::Green);
        debugBox.setOutlineThickness(2.f);
        window.draw(debugBox);
    }
}

sf::Vector2f Player::getPosition() const {
    return mHitbox.getPosition();
}

sf::FloatRect Player::getBounds() const {
    return mHitbox.getGlobalBounds();
}

bool Player::isFacingRight() const {
    return mFacingRight;
}