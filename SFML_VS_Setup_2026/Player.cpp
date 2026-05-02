#include "Player.h"

Player::Player(int playerIndex)
    : mSpeed(180.f),
    mVelocityY(0.f),
    mVelocityX(0.f),
    mIsOnGround(false),
    mFacingRight(true),
    mPlayerIndex(playerIndex),
    mThrowKeyHeld(false),
    mLives(10),
    mRespawnTimer(0.f),
    mRespawnTime(3.f),
    mRespawning(false),
    mStartX(0.f),
    mStartY(0.f),
    mSpeedBoosted(false),
    mBalloonMode(false)
{
    mHitbox.setSize(sf::Vector2f(40.f, 50.f));
    mVisual.setSize(sf::Vector2f(40.f, 50.f));
    mHitbox.setFillColor(sf::Color::Transparent);

    if (mPlayerIndex == 0) {
        mStartX = 600.f;
        mStartY = 490.f;
        mVisual.setFillColor(sf::Color(100, 180, 255));
    }
    else {
        mStartX = 200.f;
        mStartY = 490.f;
        mVisual.setFillColor(sf::Color(255, 165, 0));
    }

    mHitbox.setPosition(mStartX, mStartY);
    mVisual.setPosition(mStartX, mStartY);
}

void Player::update(float deltaTime,
    const Platform* platforms, int platformCount)
{
    if (mLives <= 0) return;

    // respawn invincibility countdown
    if (mRespawning) {
        mRespawnTimer += deltaTime;
        if (mRespawnTimer >= mRespawnTime) {
            mRespawning = false;
            mRespawnTimer = 0.f;
        }
    }

    // ── BALLOON MODE ─────────────────────────────────────────────
    // works exactly like FlyingEnemy — free 2D movement
    // gravity is off, player controls all 4 directions
    // bounces off all screen edges, passes through platforms
    if (mBalloonMode) {

        mVelocityX = 0.f;
        mVelocityY = 0.f;

        float speed = BALLOON_SPEED;

        if (mPlayerIndex == 0) {
            // player 1 — arrow keys horizontal
            // Up arrow = rise, Ctrl = descend
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
                mVelocityX = -speed;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
                mVelocityX = speed;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
                mVelocityY = -speed;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::RControl))
                mVelocityY = speed;
        }
        else {
            // player 2 — WASD horizontal
            // W = rise, CapsLock = descend
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
                mVelocityX = -speed;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
                mVelocityX = speed;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) 
                mVelocityY = -speed;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
                mVelocityY = speed;
        }

        // update facing direction
        if (mVelocityX > 0) mFacingRight = true;
        if (mVelocityX < 0) mFacingRight = false;

        // move freely
        mHitbox.move(mVelocityX * deltaTime, mVelocityY * deltaTime);
        mVisual.move(mVelocityX * deltaTime, mVelocityY * deltaTime);

        // bounce off screen edges — same as FlyingEnemy
        handleBalloonCollision();

        // track throw key for balloon mode too
        bool throwPressed = false;
        if (mPlayerIndex == 0)
            throwPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Down);
        else
            throwPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::S);
        if (!throwPressed) mThrowKeyHeld = false;

        return; // skip normal gravity and platform collision
    }

    // ── NORMAL MODE ──────────────────────────────────────────────
    float currentSpeed = mSpeedBoosted ? mSpeed * 1.5f : mSpeed;
    float moveX = 0.f;

    if (mPlayerIndex == 0) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            moveX = -currentSpeed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            moveX = currentSpeed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && mIsOnGround) {
            mVelocityY = JUMP_FORCE;
            mIsOnGround = false;
        }
    }
    else {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
            moveX = -currentSpeed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
            moveX = currentSpeed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && mIsOnGround) {
            mVelocityY = JUMP_FORCE;
            mIsOnGround = false;
        }
    }

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

    // screen edges — horizontal only, no vertical clamp in normal mode
    sf::Vector2f pos = mHitbox.getPosition();
    if (pos.x < 0.f) {
        mHitbox.setPosition(0.f, pos.y);
        mVisual.setPosition(0.f, pos.y);
    }
    if (pos.x + mHitbox.getSize().x > 800.f) {
        mHitbox.setPosition(800.f - mHitbox.getSize().x, pos.y);
        mVisual.setPosition(800.f - mHitbox.getSize().x, pos.y);
    }

    // throw key tracking
    bool throwPressed = false;
    if (mPlayerIndex == 0)
        throwPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Down);
    else
        throwPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::S);
    if (!throwPressed) mThrowKeyHeld = false;
}

// bounce off all 4 screen edges during balloon mode
// identical logic to FlyingEnemy screen bounce
void Player::handleBalloonCollision() {
    sf::Vector2f pos = mHitbox.getPosition();
    float w = mHitbox.getSize().x;
    float h = mHitbox.getSize().y;

    // left edge
    if (pos.x < 0.f) {
        mHitbox.setPosition(0.f, pos.y);
        mVisual.setPosition(0.f, pos.y);
        mVelocityX = -mVelocityX; // reverse horizontal
    }
    // right edge
    if (pos.x + w > 800.f) {
        mHitbox.setPosition(800.f - w, pos.y);
        mVisual.setPosition(800.f - w, pos.y);
        mVelocityX = -mVelocityX;
    }
    // top edge
    if (pos.y < 0.f) {
        mHitbox.setPosition(pos.x, 0.f);
        mVisual.setPosition(pos.x, 0.f);
        mVelocityY = -mVelocityY; // reverse vertical
    }
    // bottom edge — treat ground as a bounce too while floating
    if (pos.y + h > 660.f) {
        mHitbox.setPosition(pos.x, 660.f - h);
        mVisual.setPosition(pos.x, 660.f - h);
        mVelocityY = -mVelocityY;
    }
}

void Player::handleCollision(const Platform* platforms,
    int platformCount)
{
    mIsOnGround = false;

    for (int i = 0; i < platformCount; i++) {
        sf::FloatRect player = mHitbox.getGlobalBounds();
        sf::FloatRect plat = platforms[i].getBounds();

        if (!player.intersects(plat)) continue;

        float overlapTop = (player.top + player.height) - plat.top;

        if (mVelocityY >= 0 && overlapTop > 0 && overlapTop < 50.f) {
            mHitbox.move(0.f, -overlapTop);
            mVisual.move(0.f, -overlapTop);
            mVelocityY = 0.f;
            mIsOnGround = true;
        }
    }
}

bool Player::wantsToThrow() {
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
    if (mRespawning) return;
    if (mLives <= 0)  return;

    mLives--;

    if (mLives > 0) {
        mHitbox.setPosition(mStartX, mStartY);
        mVisual.setPosition(mStartX, mStartY);
        mVelocityY = 0.f;
        mRespawnTimer = 0.f;
        mRespawning = true;
    }
    else {
        mHitbox.setPosition(-200.f, -200.f);
        mVisual.setPosition(-200.f, -200.f);
        mVelocityY = 0.f;
    }
}

void Player::resetLives() {
    mLives = 10;
    mRespawning = false;
    mRespawnTimer = 0.f;
    mHitbox.setPosition(mStartX, mStartY);
    mVisual.setPosition(mStartX, mStartY);
    mVelocityY = 0.f;
}

int  Player::getLives()      const { return mLives; }
bool Player::isAlive()       const { return mLives > 0; }
bool Player::isRespawning()  const { return mRespawning; }
bool Player::isFacingRight() const { return mFacingRight; }

sf::Vector2f Player::getPosition() const {
    return mHitbox.getPosition();
}

sf::FloatRect Player::getBounds() const {
    return mHitbox.getGlobalBounds();
}

void Player::draw(sf::RenderWindow& window, bool showHitbox) {
    if (mLives <= 0) return;
    window.draw(mVisual);

    if (showHitbox) {
        sf::RectangleShape debug;
        debug.setPosition(mHitbox.getPosition());
        debug.setSize(mHitbox.getSize());
        debug.setFillColor(sf::Color::Transparent);
        debug.setOutlineColor(sf::Color::Green);
        debug.setOutlineThickness(2.f);
        window.draw(debug);
    }
}

void Player::setSpeedBoost(bool active) { mSpeedBoosted = active; }
void Player::setBalloonMode(bool active) {
    mBalloonMode = active;
    // reset velocities when mode changes
    mVelocityX = 0.f;
    mVelocityY = 0.f;
}
void Player::addLife() { mLives++; }