#include "PowerUp.h"

// default constructor for arrays
PowerUp::PowerUp()
    : mType(GEM),
    mCollected(false),
    mExpired(false),
    mLifeTimer(0.f),
    mColor(sf::Color::Yellow) {
    mHitbox.setSize(sf::Vector2f(20.f, 20.f));
    mVisual.setSize(sf::Vector2f(20.f, 20.f));
    mHitbox.setFillColor(sf::Color::Transparent);
    mVisual.setFillColor(mColor);
}

PowerUp::PowerUp(float x, float y, PowerUpType type)
    : mType(type),
    mCollected(false),
    mExpired(false),
    mLifeTimer(0.f)
     {

    // set color based on type
    switch (type) {
    case GEM:            mColor = sf::Color(0, 255, 200);   break; // cyan gem
    case SPEED_BOOST:    mColor = sf::Color(255, 100, 0);   break; // orange
    case SNOWBALL_POWER: mColor = sf::Color(100, 100, 255); break; // blue
    case DISTANCE_BOOST: mColor = sf::Color(255, 255, 0);   break; // yellow
    case BALLOON_MODE:   mColor = sf::Color(255, 0, 200);   break; // pink
    case EXTRA_LIFE:     mColor = sf::Color(255, 50, 50);   break; // red
    }

    mHitbox.setSize(sf::Vector2f(20.f, 20.f));
    mHitbox.setPosition(x, y);
    mHitbox.setFillColor(sf::Color::Transparent);

    mVisual.setSize(sf::Vector2f(20.f, 20.f));
    mVisual.setPosition(x, y);
    mVisual.setFillColor(mColor);
}

void PowerUp::update(float deltaTime) {
    if (mCollected || mExpired) return;

    // disappear after 10 seconds if not collected
    mLifeTimer += deltaTime;
    if (mLifeTimer >= 10.f)
        mExpired = true;
}

void PowerUp::draw(sf::RenderWindow& window, bool showHitbox) {
    if (mCollected || mExpired) return;

    window.draw(mVisual);

    if (showHitbox) {
        sf::RectangleShape debug;
        debug.setPosition(mHitbox.getPosition());
        debug.setSize(mHitbox.getSize());
        debug.setFillColor(sf::Color::Transparent);
        debug.setOutlineColor(sf::Color::Magenta);
        debug.setOutlineThickness(2.f);
        window.draw(debug);
    }
}

sf::FloatRect PowerUp::getBounds() const {
    return mHitbox.getGlobalBounds();
}

bool PowerUp::isCollected() const {
    return mCollected;
}

bool PowerUp::isExpired() const {
    return mExpired;
}

void PowerUp::collect() {
    mCollected = true;
}

PowerUpType PowerUp::getType() const {
    return mType;
}