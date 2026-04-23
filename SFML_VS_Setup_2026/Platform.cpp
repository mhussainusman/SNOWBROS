#include "Platform.h"

Platform::Platform(float x, float y, float width, float height) {
    mShape.setPosition(x, y);
    mShape.setSize(sf::Vector2f(width, height));
    mShape.setFillColor(sf::Color(255, 255, 255)); // white platforms
}

void Platform::draw(sf::RenderWindow& window) {
    window.draw(mShape);
}

sf::FloatRect Platform::getBounds() const {
    return mShape.getGlobalBounds();
} 