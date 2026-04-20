#include "Player.h"

Player::Player()
    : mSpeed(200.f),
    mVelocityY(0.f),
    mIsOnGround(false) {
    mShape.setSize(sf::Vector2f(40.f, 50.f));
    mShape.setFillColor(sf::Color(100, 180, 255));
    mShape.setPosition(100.f, 100.f); // start near top so we can see falling
}

void Player::update(float deltaTime, const std::vector<Platform>& platforms) {
    // left and right movement
    sf::Vector2f velocity(0.f, 0.f);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
        velocity.x -= mSpeed;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
        velocity.x += mSpeed;

    mShape.move(velocity * deltaTime);

    // jump — only allowed when standing on ground
    if ((sf::Keyboard::isKeyPressed(sf::Keyboard::W) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) && mIsOnGround) {
        mVelocityY = JUMP_FORCE;
        mIsOnGround = false;
    }

    // apply gravity
    applyGravity(deltaTime);

    // check platform collisions
    handleCollision(platforms);
}

void Player::applyGravity(float deltaTime) {
    // increase downward speed every frame
    mVelocityY += GRAVITY * deltaTime;
    mShape.move(0.f, mVelocityY * deltaTime);
}

void Player::handleCollision(const std::vector<Platform>& platforms) {
    mIsOnGround = false;

    for (const auto& platform : platforms) {
        sf::FloatRect playerBounds = mShape.getGlobalBounds();
        sf::FloatRect platformBounds = platform.getBounds();

        if (playerBounds.intersects(platformBounds)) {
            // push player back on top of the platform
            mShape.setPosition(mShape.getPosition().x,
                platformBounds.top - mShape.getSize().y);
            mVelocityY = 0.f;
            mIsOnGround = true;
        }
    }
}

void Player::draw(sf::RenderWindow& window) {
    window.draw(mShape);
}

sf::Vector2f Player::getPosition() const {
    return mShape.getPosition();
}

sf::FloatRect Player::getBounds() const {
    return mShape.getGlobalBounds();
}