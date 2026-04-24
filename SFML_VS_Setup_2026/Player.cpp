#include "Player.h"

Player::Player(int playerIndex)
    : mSpeed(180.f),
    mVelocityY(0.f),
    mIsOnGround(false),
    mFacingRight(true),
    mPlayerIndex(playerIndex),
    mThrowKeyHeld(false) {

    // set size
    mHitbox.setSize(sf::Vector2f(40.f, 50.f));
    mVisual.setSize(sf::Vector2f(40.f, 50.f));
    mHitbox.setFillColor(sf::Color::Transparent);

    // player 1 starts left side, player 2 starts right side
    if (mPlayerIndex == 0) {
        mHitbox.setPosition(100.f, 100.f);
        mVisual.setPosition(100.f, 100.f);
        mVisual.setFillColor(sf::Color(100, 180, 255)); // light blue
    }
    else {
        mHitbox.setPosition(650.f, 100.f);
        mVisual.setPosition(650.f, 100.f);
        mVisual.setFillColor(sf::Color(100, 255, 150)); // light green
    }
}

void Player::update(float deltaTime,
    const std::vector<Platform>& platforms) {

    float moveX = 0.f;

    if (mPlayerIndex == 0) {
        // player 1 controls — arrow keys
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
        // player 2 controls — WASD
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

    // collision
    handleCollision(platforms);

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

    // track throw key — reset when key released
    bool throwPressed = false;
    if (mPlayerIndex == 0)
        throwPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Down);
    else
        throwPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::S);

    if (!throwPressed)
        mThrowKeyHeld = false;
}

bool Player::wantsToThrow() {
    // check throw key for this player
    bool throwPressed = false;
    if (mPlayerIndex == 0)
        throwPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Down);
    else
        throwPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::S);

    // only return true once per key press
    // mThrowKeyHeld prevents holding key from spamming snowballs
    if (throwPressed && !mThrowKeyHeld) {
        mThrowKeyHeld = true;
        return true;
    }
    return false;
}

void Player::handleCollision(const std::vector<Platform>& platforms) {
    mIsOnGround = false;

    for (int i = 0; i < platforms.size(); i++) {
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