#include "Enemy.h"

// Enemy base constructor
Enemy::Enemy(float x, float y, int hitsToEncase)
    : mVelocityY(0.f),
    mIsOnGround(false),
    mDead(false),
    mSnowHits(0),
    mHitsToEncase(hitsToEncase),
    mEncased(false),
    mRolling(false),
    mRollSpeed(300.f),
    mRollingRight(true),
    mMeltTimer(0.f),
    mMeltTime(0.f) {
    mHitbox.setPosition(x, y);
    mHitbox.setFillColor(sf::Color::Transparent);
    mVisual.setPosition(x, y);
}

void Enemy::takeDamage() {
    if (mRolling || mDead) return;

    if (mEncased) {
        mMeltTimer = 0.f;
        return;
    }

    mSnowHits++;
    mMeltTimer = 0.f;

    if (mSnowHits >= mHitsToEncase) {
        mEncased = true;
        mMeltTime = 6.f;
        mVisual.setFillColor(sf::Color::White);
    }
    else {
        mMeltTime = 3.f;
        mVisual.setFillColor(sf::Color(200, 200, 255));
    }
}

bool Enemy::isFullyEncased() const { return mEncased; }
bool Enemy::isDead()          const { return mDead; }
bool Enemy::isRolling()       const { return mRolling; }

sf::FloatRect Enemy::getBounds() const {
    return mHitbox.getGlobalBounds();
}

void Enemy::startRolling(bool kickedRight) {
    mRolling = true;
    mRollingRight = kickedRight;
    mMeltTimer = 0.f;
}

void Enemy::updateMelt(float deltaTime) {
    if ((mEncased || mSnowHits > 0) && !mRolling) {
        mMeltTimer += deltaTime;
        if (mMeltTimer >= mMeltTime) {
            mSnowHits = 0;
            mEncased = false;
            mMeltTimer = 0.f;
            mMeltTime = 0.f;
            mVisual.setFillColor(mOriginalColor);
        }
    }
}

void Enemy::updateRolling(float deltaTime, const std::vector<Platform>& platforms) {
    float moveX = mRollingRight ? mRollSpeed : -mRollSpeed;
    mHitbox.move(moveX * deltaTime, 0.f);
    mVisual.move(moveX * deltaTime, 0.f);

    mVelocityY += GRAVITY * deltaTime;
    if (mVelocityY > MAX_FALL_SPEED)
        mVelocityY = MAX_FALL_SPEED;

    mHitbox.move(0.f, mVelocityY * deltaTime);
    mVisual.move(0.f, mVelocityY * deltaTime);

    for (int i = 0; i < platforms.size(); i++) {
        sf::FloatRect enemy = mHitbox.getGlobalBounds();
        sf::FloatRect plat = platforms[i].getBounds();
        if (!enemy.intersects(plat)) continue;

        float overlapTop = (enemy.top + enemy.height) - plat.top;
        if (mVelocityY >= 0 && overlapTop > 0 && overlapTop < 50.f) {
            mHitbox.move(0.f, -overlapTop);
            mVisual.move(0.f, -overlapTop);
            mVelocityY = 0.f;
        }
    }

    sf::Vector2f pos = mHitbox.getPosition();
    if (pos.x > 850.f || pos.x + mHitbox.getSize().x < -50.f)
        mDead = true;
}

void Enemy::applyGravity(float deltaTime, const std::vector<Platform>& platforms) {
    mIsOnGround = false;

    mVelocityY += GRAVITY * deltaTime;
    if (mVelocityY > MAX_FALL_SPEED)
        mVelocityY = MAX_FALL_SPEED;

    mHitbox.move(0.f, mVelocityY * deltaTime);
    mVisual.move(0.f, mVelocityY * deltaTime);

    for (int i = 0; i < platforms.size(); i++) {
        sf::FloatRect enemy = mHitbox.getGlobalBounds();
        sf::FloatRect plat = platforms[i].getBounds();
        if (!enemy.intersects(plat)) continue;

        float overlapTop = (enemy.top + enemy.height) - plat.top;
        if (mVelocityY >= 0 && overlapTop > 0 && overlapTop < 50.f) {
            mHitbox.move(0.f, -overlapTop);
            mVisual.move(0.f, -overlapTop);
            mVelocityY = 0.f;
            mIsOnGround = true;
        }
    }
}

bool Enemy::isRollingRight() const {return mRollingRight;}

void Enemy::setDead() { mDead = true; }
   


// --- BOTOM ---

Botom::Botom(float x, float y)
    : Enemy(x, y, 2),
    mMoveSpeed(150.f),
    mMovingRight(rand()%2),
    mDirectionTimer(0.f),
    mDirectionInterval(0.5f+ (rand()%200)/100.f),
    mJumpTimer(0.f),
    mJumpInterval(1.5f),
    mVelocityX(0.f),
    mCanJump(false) {

    mVisual.setSize(sf::Vector2f(38.f, 42.f));
    mHitbox.setSize(sf::Vector2f(34.f, 38.f));
    mOriginalColor = sf::Color(220, 80, 80);
    mVisual.setFillColor(mOriginalColor);
    mHitbox.setPosition(x, y);
    mVisual.setPosition(x, y);

    // start moving in random direction
    mMovingRight = (rand() % 2 == 0);
}

void Botom::setPlayerPosition(sf::Vector2f playerPos) {
    mPlayerPos = playerPos;
}

void Botom::update(float deltaTime, const std::vector<Platform>& platforms) {

    // if rolling handle rolling and skip everything else
    if (mRolling) {
        updateRolling(deltaTime, platforms);
        return;
    }

    // update melt timer
    updateMelt(deltaTime);

    // if fully encased — stop movement but gravity still applies
    if (mEncased) {
        applyGravity(deltaTime, platforms);
        mVisual.setPosition(mHitbox.getPosition());
        return;
    }

    // if partially hit — stop horizontal movement but still fall
    if (mSnowHits > 0) {
        applyGravity(deltaTime, platforms);
        mVisual.setPosition(mHitbox.getPosition());
        return;
    }


    // --- RANDOM WALK ---
    // count time and randomly change direction
    mDirectionTimer += deltaTime;
    if (mDirectionTimer >= mDirectionInterval) {
        mDirectionTimer = 0.f;

        // randomly pick next direction — 50/50 chance
        mMovingRight = (rand() % 2 == 0);

        // next direction change between 0.5 and 2.5 seconds
        // short intervals make movement feel organic and unpredictable
        mDirectionInterval = 0.5f + (rand() % 200) / 100.f;
    }

    // move left or right at full speed
    float moveX = mMovingRight ? mMoveSpeed : -mMoveSpeed;
    mHitbox.move(moveX * deltaTime, 0.f);
    mVisual.move(moveX * deltaTime, 0.f);

    // --- SCREEN EDGE HANDLING ---
    sf::Vector2f pos = mHitbox.getPosition();
    if (pos.x < 0.f) {
        mHitbox.setPosition(0.f, pos.y);
        mVisual.setPosition(0.f, pos.y);
        mMovingRight = true; // bounce right
    }
    if (pos.x + mHitbox.getSize().x > 800.f) {
        mHitbox.setPosition(800.f - mHitbox.getSize().x, pos.y);
        mVisual.setPosition(800.f - mHitbox.getSize().x, pos.y);
        mMovingRight = false; // bounce left
    }

    // --- JUMPING ---
   // only jump when standing on ground
   // short interval so they get back up quickly
    if (mIsOnGround) {
        mJumpTimer += deltaTime;

        if (mJumpTimer >= mJumpInterval) {
            mJumpTimer = 0.f;

            // randomize next jump between 0.8 and 2 seconds
            // short enough that they dont stay on ground too long
            mJumpInterval = 0.3f + (rand() % 80) / 100.f;

            // always jump — no condition needed
            // random direction change on jump too
            mMovingRight = (rand() % 2 == 0);
            mVelocityY = JUMP_FORCE;
            mIsOnGround = false;
        }
    }


    // --- GRAVITY ---
    // enemy falls off platform edges naturally
    // this is how they move between platforms — no jumping needed
    applyGravity(deltaTime, platforms);

    // sync visual position with hitbox
    mVisual.setPosition(mHitbox.getPosition());
}
// DRAW BOTTOM

void Botom::draw(sf::RenderWindow& window, bool showHitbox) {
    window.draw(mVisual);

    if (showHitbox) {
        sf::RectangleShape debugBox;
        debugBox.setPosition(mHitbox.getPosition());
        debugBox.setSize(mHitbox.getSize());
        debugBox.setFillColor(sf::Color::Transparent);
        debugBox.setOutlineColor(sf::Color::Red);
        debugBox.setOutlineThickness(2.f);
        window.draw(debugBox);
    }
}