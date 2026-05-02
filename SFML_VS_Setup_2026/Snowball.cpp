#include "Snowball.h"

Snowball::Snowball(float x, float y, bool movingRight, int playerIndex)
    : mSpeed(400.f),
    mMovingRight(movingRight),
    mDistanceTravelled(0.f),
    mMaxDistance(300.f),
    mExpired(false),
    mPlayerIndex(playerIndex),
    mVelocityY(0.f)
{

    // hitbox — actual collision area
    mHitbox.setSize(sf::Vector2f(20.f, 16.f));
    mHitbox.setPosition(x, y);
    mHitbox.setFillColor(sf::Color::Transparent);

    // visual — what player sees
    // slightly bigger than hitbox so it looks good
    mVisual.setSize(sf::Vector2f(20.f, 16.f));
    mVisual.setPosition(x, y);
    mVisual.setFillColor(sf::Color(150, 220, 255)); // light blue snowball
}

void Snowball::update(float deltaTime) {
    if (mExpired) return;

    // horizontal movement
    float moveX = mMovingRight ? mSpeed : -mSpeed;
    mHitbox.move(moveX * deltaTime, 0.f);

    // gravity curves snowball downward over time
    if (mMaxDistance<=300.f) {
        mVelocityY += 500.f * deltaTime;
        mHitbox.move(0.f, mVelocityY * deltaTime);
    }

    mVisual.setPosition(mHitbox.getPosition());

    // track horizontal distance
    mDistanceTravelled += mSpeed * deltaTime;
    if (mDistanceTravelled >= mMaxDistance)
        mExpired = true;

    // expire when off screen
    sf::Vector2f pos = mHitbox.getPosition();
    if (pos.y > 680.f)
        mExpired = true;
    if (pos.x > 820.f || pos.x + mHitbox.getSize().x < -20.f)
        mExpired = true;
}

void Snowball::draw(sf::RenderWindow& window, bool showHitbox) {

    if (!showHitbox) {
        // normal mode — draw visual
        window.draw(mVisual);
    }
    else {
        // debug mode — draw hitbox outline in yellow
        sf::RectangleShape debugBox;
        debugBox.setPosition(mHitbox.getPosition());
        debugBox.setSize(mHitbox.getSize());
        debugBox.setFillColor(sf::Color::Transparent);
        debugBox.setOutlineColor(sf::Color::Yellow); // yellow for projectiles
        debugBox.setOutlineThickness(2.f);
        window.draw(debugBox);
    }
}

sf::FloatRect Snowball::getBounds() const {
    return mHitbox.getGlobalBounds();
}

bool Snowball::isExpired() const {
    return mExpired;
}

void Snowball::setExpired() {
    mExpired = true;
}

int Snowball::getPlayerIndex() const {
    return mPlayerIndex;
}
void Snowball::setMaxDistance(float dist) {
    mMaxDistance = dist;
}