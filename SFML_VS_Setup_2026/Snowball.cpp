#include "Snowball.h"

Snowball::Snowball(float x, float y, bool movingRight, int playerIndex)
    : mSpeed(400.f),
    mMovingRight(movingRight),
    mDistanceTravelled(0.f),
    mMaxDistance(300.f),
    mExpired(false),
    mPlayerIndex(playerIndex),
    mVelocityY(0.f)
{

    // hitbox — actual collision area
    mHitbox.setSize(sf::Vector2f(25.f, 21.f));
    mHitbox.setPosition(x, y);
    mHitbox.setFillColor(sf::Color::Transparent);

    // visual — fallback if sprite fails to load
    mVisual.setSize(sf::Vector2f(25.f, 21.f));
    mVisual.setPosition(x, y);
    mVisual.setFillColor(sf::Color(150, 220, 255)); // light blue snowball

    // sprite
    mTextureLoaded = mTexture.loadFromFile("assets/Images/snowball.png");
    if (mTextureLoaded) {
        sf::Vector2u texSize = mTexture.getSize();
        mBaseScaleX = mHitbox.getSize().x / (float)texSize.x;
        mBaseScaleY = mHitbox.getSize().y / (float)texSize.y;
    }
}

void Snowball::update(float deltaTime) {

    if (mExpired) return;
    //move horizontal
    float moveX = mMovingRight ? mSpeed : -mSpeed;
    mHitbox.move(moveX * deltaTime, 0.f);

    mVisual.setPosition(mHitbox.getPosition());

    // gravity curves snowball downward over time
    if (mMaxDistance <= 300.f) {
        mVelocityY += 500.f * deltaTime;
        mHitbox.move(0.f, mVelocityY * deltaTime);
    }





    // track total distance travelled
    mDistanceTravelled += mSpeed * deltaTime;

    // expire when max distance reached
    if (mDistanceTravelled >= mMaxDistance)
        mExpired = true;

    // expire if goes off screen
    sf::Vector2f pos = mHitbox.getPosition();

    if (pos.y > 680.f)
        mExpired = true;
    if (pos.x > 820.f || pos.x + mHitbox.getSize().x < -20.f)
        mExpired = true;
}

void Snowball::draw(sf::RenderWindow& window, bool showHitbox) {

    if (!showHitbox) {
        if (mTextureLoaded) {
            sf::Sprite sprite;
            sprite.setTexture(mTexture);
            sprite.setScale(mBaseScaleX, mBaseScaleY);
            sprite.setPosition(mHitbox.getPosition());
            window.draw(sprite);
        }
        else {
            window.draw(mVisual);
        }
    }
    else {
        // debug mode — draw hitbox outline in yellow
        sf::RectangleShape debugBox;
        debugBox.setPosition(mHitbox.getPosition());
        debugBox.setSize(mHitbox.getSize());
        debugBox.setFillColor(sf::Color::Transparent);
        debugBox.setOutlineColor(sf::Color::Yellow); // yellow for projectiles
        debugBox.setOutlineThickness(2.f);
        window.draw(debugBox);
    }
}

sf::FloatRect Snowball::getBounds() const {
    return mHitbox.getGlobalBounds();
}

bool Snowball::isExpired() const {
    return mExpired;
}

void Snowball::setExpired() {
    mExpired = true;
}

int Snowball::getPlayerIndex() const {
    return mPlayerIndex;
}

void Snowball::setMaxDistance(float dist) {
    mMaxDistance = dist;
}

void Snowball::applyPowerBoost() {
    mSpeed = 600.f;           // faster travel
    mMaxDistance = 800.f;     // guaranteed full screen width (already max, makes explicit)
}