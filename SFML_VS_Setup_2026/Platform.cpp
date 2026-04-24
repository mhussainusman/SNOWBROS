#include "Platform.h"

Platform::Platform(float x, float y, float width, float height) {

    // hitbox: exact collision area
    mHitbox.setPosition(x, y);
    mHitbox.setSize(sf::Vector2f(width, height));
    mHitbox.setFillColor(sf::Color::Transparent);

    // visual: what player sees on screen
    // same size and position as hitbox for now
    // can be changed later when sprites are added
    mVisual.setPosition(x, y);
    mVisual.setSize(sf::Vector2f(width, height));
    mVisual.setFillColor(sf::Color(255, 255, 255)); // white platforms
}

void Platform::draw(sf::RenderWindow& window, bool showHitbox) {

    // always draw the visual
    if(!showHitbox) window.draw(mVisual);

    // only draw hitbox outline when debug mode is on
    else {
        sf::RectangleShape debugBox;
        debugBox.setPosition(mHitbox.getPosition());
        debugBox.setSize(sf::Vector2f(
            mHitbox.getGlobalBounds().width,
            mHitbox.getGlobalBounds().height));
        debugBox.setFillColor(sf::Color::Transparent);
        debugBox.setOutlineColor(sf::Color::Blue); // blue for platforms
        debugBox.setOutlineThickness(2.f);
        window.draw(debugBox);
    }
}

sf::FloatRect Platform::getBounds() const {
    return mHitbox.getGlobalBounds();
}