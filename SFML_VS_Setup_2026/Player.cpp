#include "Player.h"

Player::Player()
    : mSpeed(180.f),
      mVelocityY(0.f),
      mIsOnGround(false) {
    mShape.setSize(sf::Vector2f(40.f, 50.f));
    mShape.setFillColor(sf::Color(100, 180, 255));
    mShape.setPosition(100.f, 100.f);

}

void Player::update(float deltaTime, const std::vector<Platform>& platforms) {

    // --- HORIZONTAL MOVEMENT ---
    // check left and right keys and move accordingly
    float moveX = 0.f;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
        moveX = -mSpeed;  // negative X = move left

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
        moveX = mSpeed;   // positive X = move right

    // multiply by deltaTime to make movement frame-rate independent
    mShape.move(moveX * deltaTime, 0.f);

    // --- JUMPING ---
    // only allowed when player is standing on something
    if ((sf::Keyboard::isKeyPressed(sf::Keyboard::W) ||
         sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) && mIsOnGround) {
        mVelocityY = JUMP_FORCE;  // shoot upward with negative velocity
        mIsOnGround = false;      // no longer on ground
    }

    // --- GRAVITY ---
    // every frame, increase downward speed (simulate gravity pulling down)
    mVelocityY += GRAVITY * deltaTime;

    // cap falling speed so player doesnt fall through platforms at high speed
    if (mVelocityY > MAX_FALL_SPEED)
        mVelocityY = MAX_FALL_SPEED;

    // apply vertical movement
    mShape.move(0.f, mVelocityY * deltaTime);

    // --- COLLISION ---
    // check and resolve platform collisions after moving
    handleCollision(platforms);

    // --- SCREEN BOUNDARIES ---
    // keep player inside the window horizontally
    sf::Vector2f pos = mShape.getPosition();
    if (pos.x < 0.f)
        mShape.setPosition(0.f, pos.y);
    if (pos.x + mShape.getSize().x > 800.f)
        mShape.setPosition(800.f - mShape.getSize().x, pos.y);
}

void Player::handleCollision(const std::vector<Platform>& platforms) {
    mIsOnGround = false; // assume not on ground, prove it below

    for (int i = 0; i < platforms.size();i++) {
        sf::FloatRect player = mShape.getGlobalBounds();
        sf::FloatRect plat   = platforms[i].getBounds();

        // skip if no overlap at all
        if (!player.intersects(plat))
            continue;

        float overlapTop = (player.top + player.height) - plat.top;

        if (mVelocityY >= 0 && overlapTop > 0 && overlapTop < 20.f) {
            mShape.move(0.f, -overlapTop);// - is there as we move up
            mVelocityY = 0.f;
            mIsOnGround = true;
        }
    }
}

void Player::draw(sf::RenderWindow& window) {
    window.draw(mShape);
}

sf::Vector2f Player::getPosition() const {
    return mShape.getPosition();
}

sf::FloatRect Player::getBounds() const {
    return mShape.getGlobalBounds();
}