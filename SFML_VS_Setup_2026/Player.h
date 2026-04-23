#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Platform.h"


// Player: movement, gravity, jumping, one-way platform collision
class Player {
public:
    Player();

    void update(float deltaTime, const std::vector<Platform>& platforms);
    void draw(sf::RenderWindow& window);

    sf::Vector2f getPosition() const;
    sf::FloatRect getBounds() const;

private:
    void handleCollision(const std::vector<Platform>& platforms);

    sf::RectangleShape mShape;
    float mSpeed;
    float mVelocityY;    // positive = falling down, negative = jumping up
    bool mIsOnGround;    // true when standing on a platform

    const float GRAVITY = 900.f;
    const float JUMP_FORCE = -480.f;    // negative = upward in SFML
    const float MAX_FALL_SPEED = 600.f;
};