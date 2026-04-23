#pragma once
#include <SFML/Graphics.hpp>

// Snowball: projectile thrown by player
// travels horizontally until it hits an enemy or reaches max distance
class Snowball {
public:
    // x, y = starting position, movingRight = direction
    Snowball(float x, float y, bool movingRight);

    void update(float deltaTime);
    void draw(sf::RenderWindow& window);

    sf::FloatRect getBounds() const; // for collision with enemies
    bool isExpired() const;          // true when snowball should be removed

	void setExpired(); // mark snowball for removal when it hits an enemy
private:
    sf::RectangleShape mShape;
    float mSpeed;         // how fast snowball travels
    bool mMovingRight;    // direction of travel
    float mDistanceTravelled; // how far it has gone
    float mMaxDistance;   // max distance before disappearing
    bool mExpired;        // should be removed from game
};