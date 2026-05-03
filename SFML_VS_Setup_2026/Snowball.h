#pragma once
#include <SFML/Graphics.hpp>

// Snowball: projectile thrown by player
// travels horizontally until it hits enemy or goes off screen
class Snowball {
public:
    // x, y = starting position
    // movingRight = direction it travels
    // playerIndex = which player threw it (0 = player1, 1 = player2)
    Snowball(float x, float y, bool movingRight, int playerIndex);
	Snowball() {}

    void update(float deltaTime);
    void draw(sf::RenderWindow& window, bool showHitbox);

    sf::FloatRect getBounds() const; // for collision with enemies
    bool isExpired() const;          // true when should be removed
    void setExpired();               // called when hits enemy or wall
    int getPlayerIndex() const;      // which player threw this

    void applyPowerBoost();   // called when SNOWBALL POWER is active

private:
    sf::RectangleShape mHitbox;   // collision box
    sf::RectangleShape mVisual;   // what gets drawn

    float mSpeed;              // how fast snowball travels
    bool mMovingRight;         // direction of travel
    float mDistanceTravelled;  // how far it has gone so far
    float mMaxDistance;        // disappears after this distance
    bool mExpired;             // true when should be removed
    int mPlayerIndex;          // 0 = player1, 1 = player2
};