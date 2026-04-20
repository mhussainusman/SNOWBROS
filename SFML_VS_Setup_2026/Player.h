#pragma once

#include <SFML/Graphics.hpp>

/**
 * @class Player
 * @brief Represents the player character. Handles input and movement.
 */
class Player {
public:
    Player();

    void update(float deltaTime);
    void draw(sf::RenderWindow& window);

    sf::Vector2f getPosition() const;

private:
    sf::RectangleShape mShape;
    float mSpeed;
};