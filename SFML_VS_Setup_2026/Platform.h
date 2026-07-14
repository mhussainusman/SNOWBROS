#pragma once
#include <SFML/Graphics.hpp>

// Platform: a static surface that players and enemies can stand on
class Platform {
public:
    Platform(float x, float y, float width, float height);
    Platform() {}

    // showHitbox: when true draws blue outline showing collision area
    void draw(sf::RenderWindow& window, bool showHitbox);
    sf::FloatRect getBounds() const;

private:
    sf::RectangleShape mVisual;   // fallback if the tile sprite fails to load
    sf::RectangleShape mHitbox;   // used for collision detection

    sf::Sprite mSprite;           // tiled platform surface
    bool mTextureLoaded = false;
};