#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Platform.h"

// class Player: Represents the player. Handles movement, gravity, and jumping.
 
class Player {
public:
    Player();

    void update(float deltaTime, const std::vector<Platform>& platforms);
    void draw(sf::RenderWindow& window);

    sf::Vector2f getPosition() const;
    sf::FloatRect getBounds() const;

private:
    void applyGravity(float deltaTime);
    void handleCollision(const std::vector<Platform>& platforms);

    sf::RectangleShape mShape;
    float mSpeed;
    float mVelocityY;      // vertical speed (positive = falling)
    bool mIsOnGround;      // is player standing on a platform?

    // constants
    const float GRAVITY = 800.f;
    const float JUMP_FORCE = -450.f;
};