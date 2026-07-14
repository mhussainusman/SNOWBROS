#include "PowerUp.h"
#include <string>

// static member definitions — one shared copy for all PowerUp instances
sf::Texture PowerUp::sTextureCache[6];
bool        PowerUp::sTextureLoaded[6] = { false, false, false, false, false, false };
bool        PowerUp::sPreloaded = false;

// loads all 6 power-up PNGs from disk exactly once, up front.
// array order matches PowerUpType's declaration order (GEM=0 ... EXTRA_LIFE=5)
void PowerUp::preloadAll() {
    if (sPreloaded) return;
    sPreloaded = true;

    const char* paths[6] = {
        "assets/Images/powerup_gem.png",             // GEM
        "assets/Images/powerup_speed.png",           // SPEED_BOOST
        "assets/Images/powerup_snowball_power.png",  // SNOWBALL_POWER
        "assets/Images/powerup_distance.png",        // DISTANCE_BOOST
        "assets/Images/powerup_balloon.png",         // BALLOON_MODE
        "assets/Images/powerup_life.png"             // EXTRA_LIFE
    };

    for (int i = 0; i < 6; i++)
        sTextureLoaded[i] = sTextureCache[i].loadFromFile(paths[i]);
}

// default constructor for arrays
PowerUp::PowerUp()
    : mType(GEM),
    mCollected(false),
    mExpired(false),
    mLifeTimer(0.f),

    mColor(sf::Color::Yellow) {
    mHitbox.setSize(sf::Vector2f(32.f, 32.f));
    mVisual.setSize(sf::Vector2f(32.f, 32.f));
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

    mHitbox.setSize(sf::Vector2f(32.f, 32.f));
    mHitbox.setPosition(x, y);
    mHitbox.setFillColor(sf::Color::Transparent);

    mVisual.setSize(sf::Vector2f(32.f, 32.f));
    mVisual.setPosition(x, y);
    mVisual.setFillColor(mColor);

    // defensive fallback — if Game forgot to call preloadAll() at startup,
    // load here instead of crashing/rendering blank. Should never actually
    // run if Game's constructor calls PowerUp::preloadAll() as intended.
    if (!sPreloaded)
        preloadAll();

    // just read from the shared cache — no disk read, no GPU upload,
    // just a pointer assignment. This is what removes the gameplay stall.
    mTextureLoaded = sTextureLoaded[type];
    if (mTextureLoaded) {
        mTexturePtr = &sTextureCache[type];
        sf::Vector2u texSize = mTexturePtr->getSize();
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
        sprite.setTexture(*mTexturePtr);
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