#pragma once

#include <SFML/Graphics.hpp>

// types of power ups and collectables
enum PowerUpType {
    GEM,             // collectable — adds gems and score
    SPEED_BOOST,     // player moves 50% faster for 15 seconds
    SNOWBALL_POWER,  // encase enemy in 1 hit until level ends
    DISTANCE_BOOST,  // snowball travels full screen
    BALLOON_MODE,    // player floats, immune to ground enemies
    EXTRA_LIFE       // permanent +1 life

};

// PowerUp: dropped by enemies when they die
// player walks over it to collect
class PowerUp {
public:
    PowerUp(float x, float y, PowerUpType type);
    PowerUp(); // default constructor for arrays

    void update(float deltaTime);
    void draw(sf::RenderWindow& window, bool showHitbox);

    sf::FloatRect getBounds() const;
    bool isCollected() const;
    bool isExpired() const;
    void collect();

    PowerUpType getType() const;

private:
    sf::RectangleShape mHitbox;
    sf::RectangleShape mVisual;   // fallback if sprite fails to load

    // sprite — no persistent sf::Sprite member (see note on copy safety);
    // built fresh in draw() from these each frame
    sf::Texture mTexture;
    bool mTextureLoaded = false;
    float mBaseScaleX = 1.f;   // scale needed to fit sprite into mHitbox size
    float mBaseScaleY = 1.f;

    PowerUpType mType;
    bool mCollected;
    bool mExpired;

    float mLifeTimer;       // how long before it disappears

    sf::Color mColor;       // color based on type — used by mVisual fallback
};