#include "PowerUp.h"
#include <string>

// default constructor for arrays
PowerUp::PowerUp()
    : mType(GEM),
    mCollected(false),
    mExpired(false),
    mLifeTimer(0.f),

    mColor(sf::Color::Yellow) {
    mHitbox.setSize(sf::Vector2f(32.f, 34.f));
    mVisual.setSize(sf::Vector2f(32.f, 34.f));
    mHitbox.setFillColor(sf::Color::Transparent);
    mVisual.setFillColor(mColor);
}

PowerUp::PowerUp(float x, float y, PowerUpType type)
    : mType(type),
    mCollected(false),
    mExpired(false),
    mLifeTimer(0.f)
{

    // set color based on type — used by mVisual fallback if sprite fails
    switch (type) {
    case GEM:            mColor = sf::Color(0, 255, 200);   break; // cyan gem
    case SPEED_BOOST:    mColor = sf::Color(255, 100, 0);   break; // orange
    case SNOWBALL_POWER: mColor = sf::Color(100, 100, 255); break; // blue
    case DISTANCE_BOOST: mColor = sf::Color(255, 255, 0);   break; // yellow
    case BALLOON_MODE:   mColor = sf::Color(255, 0, 200);   break; // pink
    case EXTRA_LIFE:     mColor = sf::Color(255, 50, 50);   break; // red
    }

    mHitbox.setSize(sf::Vector2f(32.f, 34.f));
    mHitbox.setPosition(x, y);
    mHitbox.setFillColor(sf::Color::Transparent);

    mVisual.setSize(sf::Vector2f(32.f, 34.f));
    mVisual.setPosition(x, y);
    mVisual.setFillColor(mColor);

    // sprite — load each PNG from disk only ONCE per type, ever, then
    // reuse it from this cache for every future power-up of that type.
    // Without this, spawning 8 gems in one frame (a boss death burst)
    // hits the disk 8 times in that single frame and stalls it — that
    // was the freeze/lag you saw when Mogera died.
    static sf::Texture cache[6];
    static bool cacheLoaded[6] = { false, false, false, false, false, false };

    if (!cacheLoaded[type]) {
        std::string path;
        switch (type) {
        case GEM:            path = "assets/Images/powerup_gem.png";            break;
        case SPEED_BOOST:    path = "assets/Images/powerup_speed.png";          break;
        case SNOWBALL_POWER: path = "assets/Images/powerup_snowball_power.png"; break;
        case DISTANCE_BOOST: path = "assets/Images/powerup_distance.png";       break;
        case BALLOON_MODE:   path = "assets/Images/powerup_balloon.png";        break;
        case EXTRA_LIFE:     path = "assets/Images/powerup_life.png";           break;
        }
        cacheLoaded[type] = cache[type].loadFromFile(path);
    }

    mTextureLoaded = cacheLoaded[type];
    if (mTextureLoaded) {
        mTexture = cache[type];   // fast in-memory copy, NOT a disk read
        sf::Vector2u texSize = mTexture.getSize();
        mBaseScaleX = mHitbox.getSize().x / (float)texSize.x;
        mBaseScaleY = mHitbox.getSize().y / (float)texSize.y;
    }
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

    if (mTextureLoaded) {
        sf::Sprite sprite;
        sprite.setTexture(mTexture);
        sprite.setScale(mBaseScaleX, mBaseScaleY);
        sprite.setPosition(mHitbox.getPosition());
        window.draw(sprite);
    }
    else {
        window.draw(mVisual);
    }

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