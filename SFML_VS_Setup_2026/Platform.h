#pragma once

#include <SFML/Graphics.hpp>

// class Platform: A static platform the player can stand on.
 
class Platform {
public:
    Platform(float x, float y, float width, float height);

    void draw(sf::RenderWindow& window);

    // returns the platform's bounding rectangle
    // used for collision detection with player
    sf::FloatRect getBounds() const;

private:
    sf::RectangleShape mShape;
};