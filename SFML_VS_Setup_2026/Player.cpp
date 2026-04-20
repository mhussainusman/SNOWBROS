#include "Player.h"

Player::Player()
    : mSpeed(200.f) {
    mShape.setSize(sf::Vector2f(40.f, 50.f));
    mShape.setFillColor(sf::Color(100, 180, 255));
    mShape.setPosition(100.f, 400.f);
}

void Player::update(float deltaTime) {
    sf::Vector2f velocity(0.f, 0.f);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
        velocity.x -= mSpeed;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
        velocity.x += mSpeed;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
        velocity.y -= mSpeed;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
        velocity.y += mSpeed;

    mShape.move(velocity * deltaTime);
}

void Player::draw(sf::RenderWindow& window) {
    window.draw(mShape);
}

sf::Vector2f Player::getPosition() const {
    return mShape.getPosition();
}