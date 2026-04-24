#pragma once
#include <SFML/Graphics.hpp>

// Platform: a static surface that players and enemies can stand on
class Platform {
public:
    Platform(float x, float y, float width, float height);

    // showHitbox: when true draws blue outline showing collision area
    void draw(sf::RenderWindow& window, bool showHitbox);
    sf::FloatRect getBounds() const;

private:
    sf::RectangleShape mVisual;   // what gets drawn on screen
    sf::RectangleShape mHitbox;   // used for collision detection
};