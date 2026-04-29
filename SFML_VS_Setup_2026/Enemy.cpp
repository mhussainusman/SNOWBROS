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
    mMeltTime(0.f),
    mKickedByPlayer(0) {

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

void Enemy::startRolling(bool kickedRight, int playerIndex) {
    mRolling = true;
    mRollingRight = kickedRight;
    mMeltTimer = 0.f;
    mKickedByPlayer = playerIndex;
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

void Enemy::updateRolling(float deltaTime, const Platform* platforms, int platformCount) {
    float moveX = mRollingRight ? mRollSpeed : -mRollSpeed;
    mHitbox.move(moveX * deltaTime, 0.f);
    mVisual.move(moveX * deltaTime, 0.f);

    mVelocityY += GRAVITY * deltaTime;
    if (mVelocityY > MAX_FALL_SPEED)
        mVelocityY = MAX_FALL_SPEED;

    mHitbox.move(0.f, mVelocityY * deltaTime);
    mVisual.move(0.f, mVelocityY * deltaTime);

    for (int i = 0; i < platformCount; i++) {
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

void Enemy::applyGravity(float deltaTime, const Platform* platforms, int platformCount) {
    mIsOnGround = false;

    mVelocityY += GRAVITY * deltaTime;
    if (mVelocityY > MAX_FALL_SPEED)
        mVelocityY = MAX_FALL_SPEED;

    mHitbox.move(0.f, mVelocityY * deltaTime);
    mVisual.move(0.f, mVelocityY * deltaTime);

    for (int i = 0; i < platformCount; i++) {
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

bool Enemy::isRollingRight() const { return mRollingRight; }

void Enemy::setDead() { mDead = true; }
bool Enemy::isPartiallyEncased() const { return mSnowHits > 0 && !mEncased; }

int Enemy::getKickedByPlayer() const { return mKickedByPlayer; }

// --- BOTOM ---

Botom::Botom(float x, float y,int variant)
	: Enemy(x, y, 2 + variant),// hits to encase increases with variant
	mMoveSpeed(130.f * (1.f + variant * 0.25f)),// speed increases with variant
    mMovingRight(rand() % 2),
    mJumpTimer(0.f),
    mJumpInterval(1.5f)
    {

    mVisual.setSize(sf::Vector2f(38.f, 42.f));
    mHitbox.setSize(sf::Vector2f(34.f, 38.f));
    mHitbox.setPosition(x, y);
    mVisual.setPosition(x, y);

    // color based on variant
    if (variant == 0) {
        mOriginalColor = sf::Color(220, 80, 80);   // red
    }
    else if (variant == 1) {
        mOriginalColor = sf::Color(80, 220, 80);   // green
    }
    else if (variant == 2) {
        mOriginalColor = sf::Color(80, 80, 220);   // blue
    }
    else {
        mOriginalColor = sf::Color(150, 80, 220);  // purple
    }

    mVisual.setFillColor(mOriginalColor);

    // start moving in random direction
    mMovingRight = (rand() % 2 == 0);
}

void Botom::update(float deltaTime,
    const Platform* platforms, int platformCount) {

    if (mRolling) {
        updateRolling(deltaTime, platforms, platformCount);
        return;
    }

    updateMelt(deltaTime);

    if (mEncased) {
        applyGravity(deltaTime, platforms, platformCount);
        mVisual.setPosition(mHitbox.getPosition());
        return;
    }

    if (mSnowHits > 0) {
        applyGravity(deltaTime, platforms, platformCount);
        mVisual.setPosition(mHitbox.getPosition());
        return;
    }

    // --- SIMPLE WALK ---
    // just walk in current direction
    // no random direction changes mid-platform
    float moveX = mMovingRight ? mMoveSpeed : -mMoveSpeed;
    mHitbox.move(moveX * deltaTime, 0.f);

    // --- WALL BOUNCE ---
    // only turn at screen walls — nothing else
    sf::Vector2f pos = mHitbox.getPosition();
    if (pos.x < 0.f) {
        mHitbox.setPosition(0.f, pos.y);
        mMovingRight = true;
    }
    if (pos.x + mHitbox.getSize().x > 800.f) {
        mHitbox.setPosition(800.f - mHitbox.getSize().x, pos.y);
        mMovingRight = false;
    }

    // --- JUMP ---
    // jump every 2-3 seconds when on ground
    if (mIsOnGround) {
        mJumpTimer += deltaTime;
        if (mJumpTimer >= mJumpInterval) {
            mJumpTimer = 0.f;
            mJumpInterval = 2.f + (rand() % 100) / 100.f;
            mVelocityY = JUMP_FORCE;
            mIsOnGround = false;
        }
    }

    // --- GRAVITY ---
    // enemy falls off edges naturally
    // this is how it moves between platforms
    applyGravity(deltaTime, platforms, platformCount);

    // sync visual
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

int Botom::getPoints() const {
    return 100;  // Botom always worth 100 points
}

//----Flying Enemy----------


FlyingEnemy::FlyingEnemy(float x, float y)
    : Botom(x, y),          // inherits everything from Botom
    mFlying(false),
    mFlightTimer(0.f),
    mFlightInterval(4.f), // takes flight after 4 seconds on ground
    mFlightDuration(3.f), // stays in flight for 3 seconds
    mFlightSpeedX(0.f),
    mFlightSpeedY(0.f) {

    // override Botom's appearance
    // slightly different color so player can distinguish
    mOriginalColor = sf::Color(80, 180, 80); // green
    mVisual.setFillColor(mOriginalColor);

    // needs 3 hits to encase — harder than Botom
    mHitsToEncase = 3;
}

void FlyingEnemy::update(float deltaTime,
    const Platform* platforms, int platformCount) {

    // rolling and melt handled same as Botom
    if (mRolling) {
        updateRolling(deltaTime, platforms, platformCount);
        return;
    }

    updateMelt(deltaTime);

    if (mEncased) {
        applyGravity(deltaTime, platforms, platformCount);
        mVisual.setPosition(mHitbox.getPosition());
        return;
    }

    if (mSnowHits > 0) {
        applyGravity(deltaTime, platforms, platformCount);
        mVisual.setPosition(mHitbox.getPosition());
        return;
    }

    // count time in current state
    mFlightTimer += deltaTime;

    if (!mFlying) {
        // --- GROUND STATE ---
        // behaves exactly like Botom on ground
        // call Botom's update for ground movement
        Botom::update(deltaTime, platforms, platformCount);

        // after flight interval → take flight
        if (mFlightTimer >= mFlightInterval) {
            mFlying = true;
            mFlightTimer = 0.f;

            // pick a random flight direction in 8 directions
            // -1, 0 or 1 for both x and y
            int dirX = (rand() % 3) - 1; // -1, 0, or 1
            int dirY = (rand() % 3) - 1; // -1, 0, or 1

            // make sure it actually moves somewhere
            if (dirX == 0 && dirY == 0) dirX = 1;

            mFlightSpeedX = dirX * 180.f;
            mFlightSpeedY = dirY * 180.f;
        }
    }
    else {
        // --- FLIGHT STATE ---
        // moves freely ignoring gravity and platforms
        mHitbox.move(mFlightSpeedX * deltaTime,
            mFlightSpeedY * deltaTime);

        // bounce off screen edges while flying
        sf::Vector2f pos = mHitbox.getPosition();

        if (pos.x < 0.f) {
            mHitbox.setPosition(0.f, pos.y);
            mFlightSpeedX = -mFlightSpeedX; // reverse X
        }
        if (pos.x + mHitbox.getSize().x > 800.f) {
            mHitbox.setPosition(800.f - mHitbox.getSize().x, pos.y);
            mFlightSpeedX = -mFlightSpeedX;
        }
        if (pos.y < 0.f) {
            mHitbox.setPosition(pos.x, 0.f);
            mFlightSpeedY = -mFlightSpeedY; // reverse Y
        }
        if (pos.y + mHitbox.getSize().y > 550.f) {
            mHitbox.setPosition(pos.x, 550.f - mHitbox.getSize().y);
            mFlightSpeedY = -mFlightSpeedY;
        }

        // after flight duration → return to ground
        if (mFlightTimer >= mFlightDuration) {
            mFlying = false;
            mFlightTimer = 0.f;
            mVelocityY = 0.f;

            // pick new random flight interval 3-6 seconds
            mFlightInterval = 3.f + (rand() % 300) / 100.f;
        }

        mVisual.setPosition(mHitbox.getPosition());
    }
}

void FlyingEnemy::draw(sf::RenderWindow& window, bool showHitbox) {

    // draw visual
    window.draw(mVisual);

    // draw hitbox in debug mode
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

int FlyingEnemy::getPoints() const {
    return 200; // worth more than Botom
}


// --- TORNADO ---

Tornado::Tornado(float x, float y)
    : FlyingEnemy(x, y),
    mKnifeActive(false),
    mKnifeSpeed(350.f),
    mKnifeTimer(0.f),
    mKnifeInterval(3.f) {

    // override appearance — purple color
    mOriginalColor = sf::Color(180, 80, 220);
    mVisual.setFillColor(mOriginalColor);

    // harder to encase — needs 4 hits
    mHitsToEncase = 4;

    // faster flight than FlyingEnemy
    mFlightInterval = 2.f;   // takes flight every 2 seconds
    mFlightDuration = 4.f;   // stays in flight longer
    // flight speed set when taking flight

    // knife visual — small yellow rectangle
    mKnifeVisual.setSize(sf::Vector2f(15.f, 6.f));
    mKnifeVisual.setFillColor(sf::Color::Yellow);
}

void Tornado::setNearestPlayerPos(sf::Vector2f pos) {
    mNearestPlayerPos = pos;
}

void Tornado::update(float deltaTime,
    const Platform* platforms, int platformCount) {

    if (mRolling) {
        updateRolling(deltaTime, platforms, platformCount);
        return;
    }

    updateMelt(deltaTime);

    if (mEncased) {
        applyGravity(deltaTime, platforms, platformCount);
        mVisual.setPosition(mHitbox.getPosition());
        return;
    }

    if (mSnowHits > 0) {
        applyGravity(deltaTime, platforms, platformCount);
        mVisual.setPosition(mHitbox.getPosition());
        return;
    }

    // call FlyingEnemy update for movement
    FlyingEnemy::update(deltaTime, platforms, platformCount);

    // --- KNIFE THROWING ---
    mKnifeTimer += deltaTime;

    if (mKnifeTimer >= mKnifeInterval && !mKnifeActive) {
        mKnifeTimer = 0.f;

        // calculate direction toward nearest player
        sf::Vector2f myPos = mHitbox.getPosition();
        sf::Vector2f diff = mNearestPlayerPos - myPos;

        // normalize direction — make it length 1
        // so speed is consistent regardless of distance
        float length = sqrt(diff.x * diff.x + diff.y * diff.y);
        if (length > 0) {
            mKnifeDirection = sf::Vector2f(
                diff.x / length,
                diff.y / length);
        }

        // spawn knife at Tornado center position
        mKnifeVisual.setPosition(
            myPos.x + mHitbox.getSize().x / 2.f,
            myPos.y + mHitbox.getSize().y / 2.f);

        mKnifeActive = true;
    }

    // --- KNIFE MOVEMENT ---
    if (mKnifeActive) {
        mKnifeVisual.move(
            mKnifeDirection.x * mKnifeSpeed * deltaTime,
            mKnifeDirection.y * mKnifeSpeed * deltaTime);

        // deactivate knife when off screen
        sf::Vector2f kPos = mKnifeVisual.getPosition();
        if (kPos.x < -20.f || kPos.x > 820.f ||
            kPos.y < -20.f || kPos.y > 620.f)
            mKnifeActive = false;
    }
}

void Tornado::draw(sf::RenderWindow& window, bool showHitbox) {

    // draw tornado visual
    window.draw(mVisual);

    // draw knife if active
    if (mKnifeActive)
        window.draw(mKnifeVisual);

    // debug hitboxes
    if (showHitbox) {
        sf::RectangleShape debugBox;
        debugBox.setPosition(mHitbox.getPosition());
        debugBox.setSize(mHitbox.getSize());
        debugBox.setFillColor(sf::Color::Transparent);
        debugBox.setOutlineColor(sf::Color::Red);
        debugBox.setOutlineThickness(2.f);
        window.draw(debugBox);

        // show knife hitbox too
        if (mKnifeActive) {
            sf::RectangleShape knifeDebug;
            knifeDebug.setPosition(mKnifeVisual.getPosition());
            knifeDebug.setSize(mKnifeVisual.getSize());
            knifeDebug.setFillColor(sf::Color::Transparent);
            knifeDebug.setOutlineColor(sf::Color::Yellow);
            knifeDebug.setOutlineThickness(2.f);
            window.draw(knifeDebug);
        }
    }
}

int Tornado::getPoints() const {
    return 350;
}

sf::FloatRect Tornado::getKnifeBounds() const {
    return mKnifeVisual.getGlobalBounds();
}

bool Tornado::isKnifeActive() const {
    return mKnifeActive;
}