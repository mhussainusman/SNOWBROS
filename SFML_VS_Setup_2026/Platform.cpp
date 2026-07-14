#include "Platform.h"

Platform::Platform(float x, float y, float width, float height) {

    // hitbox: exact collision area
    mHitbox.setPosition(x, y);
    mHitbox.setSize(sf::Vector2f(width, height));
    mHitbox.setFillColor(sf::Color::Transparent);

    // visual: fallback if the sprite tile fails to load
    mVisual.setPosition(x, y);
    mVisual.setSize(sf::Vector2f(width, height));
    mVisual.setFillColor(sf::Color(255, 255, 255)); // white platforms

    // sprite — one small tile texture, loaded from disk only ONCE and
    // shared (static) by every Platform ever created. setRepeated(true)
    // tells the GPU to tile it seamlessly instead of stretching it —
    // that's what stops a long platform from looking smeared.
    static sf::Texture tileTexture;
    static bool attempted = false;
    static bool tileLoaded = false;

    if (!attempted) {
        attempted = true;
        tileLoaded = tileTexture.loadFromFile("assets/Images/platform_tile.png");
        if (tileLoaded)
            tileTexture.setRepeated(true);
    }

    mTextureLoaded = tileLoaded;
    if (mTextureLoaded) {
        mSprite.setTexture(tileTexture);
        mSprite.setPosition(x, y);
        // asking for a rect bigger than the source image just tiles it,
        // since setRepeated(true) was set above
        mSprite.setTextureRect(sf::IntRect(0, 0, (int)width, (int)height));
    }
}

void Platform::draw(sf::RenderWindow& window, bool showHitbox) {

    // always draw the visual
    if (!showHitbox) {
        if (mTextureLoaded)
            window.draw(mSprite);
        else
            window.draw(mVisual);
    }

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