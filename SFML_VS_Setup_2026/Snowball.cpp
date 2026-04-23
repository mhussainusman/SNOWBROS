#include "Snowball.h"

Snowball::Snowball(float x, float y, bool movingRight)
    : mSpeed(400.f),
    mMovingRight(movingRight),
    mDistanceTravelled(0.f),
    mMaxDistance(800.f),  // travels full screen width
    mExpired(false) {
    mShape.setSize(sf::Vector2f(16.f, 16.f));
    mShape.setFillColor(sf::Color(150, 220, 255)); // light blue snowball
    mShape.setPosition(x, y);
}

void Snowball::update(float deltaTime) {
    // move left or right depending on direction player was facing
    float moveX = mMovingRight ? mSpeed : -mSpeed;
    mShape.move(moveX * deltaTime, 0.f);

    // track how far it has travelled
    mDistanceTravelled += mSpeed * deltaTime;

    // expire when it reaches max distance
    if (mDistanceTravelled >= mMaxDistance)
        mExpired = true;

    // also expire if it goes off screen
    sf::Vector2f pos = mShape.getPosition();
    if (pos.x > 800.f || pos.x + mShape.getSize().x < 0.f)
        mExpired = true;
}

void Snowball::setExpired() {
    mExpired = true;
}

void Snowball::draw(sf::RenderWindow& window) {
    window.draw(mShape);
}

sf::FloatRect Snowball::getBounds() const {
    return mShape.getGlobalBounds();
}

bool Snowball::isExpired() const {
    return mExpired;
}