#include "Enemy.h"

// constructor: place enemy at x,y and set how many hits to encase
Enemy::Enemy(float x, float y, int hitsToEncase)
    : mVelocityY(0.f),
    mIsOnGround(false),
    mDefeated(false),
    mDead(false),
    mSnowHits(0),
    mHitsToEncase(hitsToEncase),
    mEncased(false) {
    mShape.setPosition(x, y);
}

// called when a snowball hits this enemy
void Enemy::takeDamage() {
    if (mEncased) return; // already encased, ignore

    mSnowHits++;          // add one snow hit

    // change color to show snow building up
    // more hits = more white (more snow)
    if (mSnowHits >= mHitsToEncase) {
        mEncased = true;
        mShape.setFillColor(sf::Color::White); // fully white = fully encased
    }
    else {
        // partially covered — mix of original color and white
        mShape.setFillColor(sf::Color(200, 200, 255)); // light blue-white
    }
}

// returns true when enemy is fully covered in snow
bool Enemy::isFullyEncased() const {
    return mEncased;
}

// returns true when this enemy has been defeated by a rolling snowball
bool Enemy::isDefeated() const {
    return mDefeated;
}

// returns true when enemy should be deleted from the game
bool Enemy::isDead() const {
    return mDead;
}

// returns the bounding rectangle for collision detection
sf::FloatRect Enemy::getBounds() const {
    return mShape.getGlobalBounds();
}

// gravity works same for all enemies — written once here
void Enemy::applyGravity(float deltaTime, const std::vector<Platform>& platforms) {
    mIsOnGround = false;

    // pull enemy down every frame
    mVelocityY += GRAVITY * deltaTime;
    if (mVelocityY > MAX_FALL_SPEED)
        mVelocityY = MAX_FALL_SPEED;

    mShape.move(0.f, mVelocityY * deltaTime);

    // check if enemy landed on a platform
    for (const auto& platform : platforms) {
        sf::FloatRect enemy = mShape.getGlobalBounds();
        sf::FloatRect plat = platform.getBounds();

        if (!enemy.intersects(plat))
            continue;

        float overlapTop = (enemy.top + enemy.height) - plat.top;

        // same one-way platform logic as player
        if (mVelocityY >= 0 && overlapTop > 0 && overlapTop < 20.f) {
            mShape.move(0.f, -overlapTop);
            mVelocityY = 0.f;
            mIsOnGround = true;
        }
    }
}

// Botom constructor
Botom::Botom(float x, float y)
    : Enemy(x, y, 2),    // needs 2 snowball hits to encase
    mMoveSpeed(80.f),
    mMovingRight(true),
    mDirectionTimer(0.f),
    mDirectionInterval(2.f) { // changes direction every 2 seconds
    mShape.setSize(sf::Vector2f(38.f, 42.f));
    mShape.setFillColor(sf::Color(220, 80, 80)); // red enemy
}

void Botom::update(float deltaTime, const std::vector<Platform>& platforms) {
    if (mEncased) return; // if fully encased stop moving

    // count time and change direction randomly
    mDirectionTimer += deltaTime;
    if (mDirectionTimer >= mDirectionInterval) {
        mDirectionTimer = 0.f;
        mMovingRight = !mMovingRight; // flip direction

        // randomize next direction change between 1 and 3 seconds
        mDirectionInterval = 1.f + (rand() % 200) / 100.f;
    }

    // move left or right
    float moveX = mMovingRight ? mMoveSpeed : -mMoveSpeed;
    mShape.move(moveX * deltaTime, 0.f);

    // stop at screen edges — turn around
    sf::Vector2f pos = mShape.getPosition();
    if (pos.x < 0.f) {
        mShape.setPosition(0.f, pos.y);
        mMovingRight = true;  // hit left wall, go right
    }
    if (pos.x + mShape.getSize().x > 800.f) {
        mShape.setPosition(800.f - mShape.getSize().x, pos.y);
        mMovingRight = false; // hit right wall, go left
    }

    // apply gravity — enemy falls and lands on platforms
    applyGravity(deltaTime, platforms);
}

void Botom::draw(sf::RenderWindow& window) {
    window.draw(mShape);
}

