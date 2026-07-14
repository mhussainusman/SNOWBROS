#include "Player.h"

Player::Player(int playerIndex)
    : mSpeed(180.f),
    mVelocityY(0.f),
    mIsOnGround(false),
    mFacingRight(true),
    mPlayerIndex(playerIndex),
    mThrowKeyHeld(false),
    mLives(3),            // 3 total = 2 shown in HUD + 1 base life
    mRespawnTimer(0.f),
    mRespawnTime(3.f),    // 3 seconds invincibility after respawn
    mRespawning(false),
    mStartX(0.f),
    mStartY(0.f),
    mSpeedBoosted(false),
    mBalloonMode(false)
{

    mHitbox.setSize(sf::Vector2f(40.f, 59.f));
    mHitbox.setFillColor(sf::Color::Transparent);

    // player 1 respawns at midpoint of right half of ground
    // player 2 respawns at midpoint of left half of ground
    // this is just the DEFAULT sprite before Character Select overrides it
    if (mPlayerIndex == 0) {
        mStartX = 600.f;
        mStartY = 490.f;
        mTextureLoaded = mTexture.loadFromFile("assets/Images/player_blue.png");
    }
    else {
        mStartX = 200.f;
        mStartY = 490.f;
        mTextureLoaded = mTexture.loadFromFile("assets/Images/player_green.png");
    }

    if (mTextureLoaded) {
        mSprite.setTexture(mTexture);
        sf::FloatRect b = mSprite.getLocalBounds();
        mSprite.setOrigin(b.width / 2.f, b.height / 2.f); // centered origin — needed for flip
        mBaseScaleX = mHitbox.getSize().x / b.width;
        mBaseScaleY = mHitbox.getSize().y / b.height;
   
    }

    mHitbox.setPosition(mStartX, mStartY);
    syncSpritePosition();
}

void Player::update(float deltaTime,
    const Platform* platforms, int platformCount) {

    // if no lives left stop updating completely
    if (mLives <= 0) return;

    // respawn invincibility countdown
    if (mRespawning) {
        mRespawnTimer += deltaTime;
        if (mRespawnTimer >= mRespawnTime) {
            mRespawning = false;   // invincibility over
            mRespawnTimer = 0.f;   // reset for next death
        }
        // player can still move during invincibility
        // just cannot lose life
    }

    // ── BALLOON MODE ─────────────────────────────────────────────
 // works exactly like FlyingEnemy — free 2D movement
 // gravity is off, player controls all 4 directions
 // bounces off all screen edges, passes through platforms
    if (mBalloonMode) {

        mVelocityX = 0.f;
        mVelocityY = 0.f;

        float speed = BALLOON_SPEED;

        if (mPlayerIndex == 0) {

            // Up arrow = rise, Ctrl = descend
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
                mVelocityX = -speed;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
                mVelocityX = speed;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
                mVelocityY = -speed;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::RControl))
                mVelocityY = speed;
        }
        else {


            if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
                mVelocityX = -speed;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
                mVelocityX = speed;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
                mVelocityY = -speed;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
                mVelocityY = speed;
        }

        // update facing direction
        if (mVelocityX > 0) mFacingRight = true;
        if (mVelocityX < 0) mFacingRight = false;

        // move freely
        mHitbox.move(mVelocityX * deltaTime, mVelocityY * deltaTime);
        syncSpritePosition();

        // bounce off screen edges — same as FlyingEnemy
        handleBalloonCollision();

        // track throw key for balloon mode too
        bool throwPressed = false;
        if (mPlayerIndex == 0)
            throwPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Down);
        else
            throwPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::S);
        if (!throwPressed) mThrowKeyHeld = false;

        return; // skip normal gravity and platform collision
    }

    // ── NORMAL MODE ──────────────────────────────────────────────
    float currentSpeed = mSpeedBoosted ? mSpeed * 1.5f : mSpeed;
    float moveX = 0.f;


    if (mPlayerIndex == 0) {
        // player 1 — arrow keys
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            moveX = -currentSpeed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            moveX = currentSpeed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && mIsOnGround) {
            mVelocityY = JUMP_FORCE;
            mIsOnGround = false;
        }
    }
    else {
        // player 2 — WASD
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
            moveX = -currentSpeed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
            moveX = currentSpeed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && mIsOnGround) {
            mVelocityY = JUMP_FORCE;
            mIsOnGround = false;
        }
    }




    // track facing direction for snowball
    if (moveX > 0) mFacingRight = true;
    if (moveX < 0) mFacingRight = false;

    mHitbox.move(moveX * deltaTime, 0.f);
    syncSpritePosition();

    // gravity
    mVelocityY += GRAVITY * deltaTime;
    if (mVelocityY > MAX_FALL_SPEED)
        mVelocityY = MAX_FALL_SPEED;

    mHitbox.move(0.f, mVelocityY * deltaTime);
    syncSpritePosition();

    // platform collision
    handleCollision(platforms, platformCount);

    // screen edges— horizontal only, no vertical clamp in normal mode
    sf::Vector2f pos = mHitbox.getPosition();
    if (pos.x < 0.f) {
        mHitbox.setPosition(0.f, pos.y);
        syncSpritePosition();
    }
    if (pos.x + mHitbox.getSize().x > 800.f) {
        mHitbox.setPosition(800.f - mHitbox.getSize().x, pos.y);
        syncSpritePosition();
    }



    // throw key tracking
    bool throwPressed = false;
    if (mPlayerIndex == 0)
        throwPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Down);
    else
        throwPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::S);

    if (!throwPressed) mThrowKeyHeld = false;
}

// bounce off all 4 screen edges during balloon mode
// identical logic to FlyingEnemy screen bounce
void Player::handleBalloonCollision() {
    sf::Vector2f pos = mHitbox.getPosition();
    float w = mHitbox.getSize().x;
    float h = mHitbox.getSize().y;

    // left edge
    if (pos.x < 0.f) {
        mHitbox.setPosition(0.f, pos.y);
        syncSpritePosition();
        mVelocityX = -mVelocityX; // reverse horizontal
    }
    // right edge
    if (pos.x + w > 800.f) {
        mHitbox.setPosition(800.f - w, pos.y);
        syncSpritePosition();
        mVelocityX = -mVelocityX;
    }
    // top edge
    if (pos.y < 0.f) {
        mHitbox.setPosition(pos.x, 0.f);
        syncSpritePosition();
        mVelocityY = -mVelocityY; // reverse vertical
    }
    // bottom edge — treat ground as a bounce too while floating
    if (pos.y + h > 660.f) {
        mHitbox.setPosition(pos.x, 660.f - h);
        syncSpritePosition();
        mVelocityY = -mVelocityY;
    }
}

void Player::handleCollision(const Platform* platforms,
    int platformCount)
{
    mIsOnGround = false;

    for (int i = 0; i < platformCount; i++) {
        sf::FloatRect player = mHitbox.getGlobalBounds();
        sf::FloatRect plat = platforms[i].getBounds();

        if (!player.intersects(plat)) continue;


        float overlapTop = (player.top + player.height) - plat.top;

        if (mVelocityY >= 0 && overlapTop > 0 && overlapTop < 50.f) {
            mHitbox.move(0.f, -overlapTop);
            syncSpritePosition();
            mVelocityY = 0.f;
            mIsOnGround = true;
        }
    }
}

bool Player::wantsToThrow() {

    if (mLives <= 0) return false;

    bool throwPressed = false;
    if (mPlayerIndex == 0)
        throwPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Down);
    else
        throwPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::S);

    if (throwPressed && !mThrowKeyHeld) {
        mThrowKeyHeld = true;
        return true;
    }
    return false;
}


//----------------lives addition-----------------


void Player::loseLife() {
    // ignore if already respawning — still invincible
    if (mRespawning) return;

    // ignore if already dead
    if (mLives <= 0) return;

    mLives--;

    if (mLives > 0) {
        // still has lives — respawn at starting position
        mHitbox.setPosition(mStartX, mStartY);
        syncSpritePosition();
        mVelocityY = 0.f;
        mRespawnTimer = 0.f;
        mRespawning = true;
    }
    else {
        // no lives left — move off screen so player disappears
        mHitbox.setPosition(-200.f, -200.f);
        syncSpritePosition();
        mVelocityY = 0.f;
    }
}



void Player::resetLives() {
    mLives = 5;//changelives
    mRespawning = false;
    mRespawnTimer = 0.f;
    mHitbox.setPosition(mStartX, mStartY);
    syncSpritePosition();
    mVelocityY = 0.f;
}


int  Player::getLives()      const { return mLives; }
bool Player::isAlive()       const { return mLives > 0; }
bool Player::isRespawning()  const { return mRespawning; }
bool Player::isFacingRight() const { return mFacingRight; }


sf::Vector2f Player::getPosition() const {
    return mHitbox.getPosition();
}

sf::FloatRect Player::getBounds() const {
    return mHitbox.getGlobalBounds();
}

void Player::draw(sf::RenderWindow& window, bool showHitbox) {
    if (mLives <= 0) return;

    if (mTextureLoaded) {
        mSprite.setScale(mFacingRight ? -mBaseScaleX : mBaseScaleX, mBaseScaleY);
        window.draw(mSprite);
    }
    else {
        window.draw(mHitbox); // fallback if PNG failed to load
    }

    if (showHitbox) {
        sf::RectangleShape debug;
        debug.setPosition(mHitbox.getPosition());
        debug.setSize(mHitbox.getSize());
        debug.setFillColor(sf::Color::Transparent);
        debug.setOutlineColor(sf::Color::Green);
        debug.setOutlineThickness(2.f);
        window.draw(debug);
    }
}

void Player::setSpeedBoost(bool active) { mSpeedBoosted = active; }

void Player::setBalloonMode(bool active) {
    mBalloonMode = active;

    // reset velocities when mode changes
    mVelocityX = 0.f;
    mVelocityY = 0.f;
}

void Player::addLife() {
    if (mLives < 10) mLives++;
}

// called once from Character Select when a player confirms their pick
// 0 = blue, 1 = green, 2 = red — overrides whatever the constructor loaded by default
void Player::setCharacterSprite(int characterIndex) {
    if (characterIndex == 0)
        mTextureLoaded = mTexture.loadFromFile("assets/Images/player_blue.png");
    else if (characterIndex == 1)
        mTextureLoaded = mTexture.loadFromFile("assets/Images/player_green.png");
    else
        mTextureLoaded = mTexture.loadFromFile("assets/Images/player_red.png");

    if (mTextureLoaded) {
        mSprite.setTexture(mTexture, true);
        sf::FloatRect b = mSprite.getLocalBounds();
        mSprite.setOrigin(b.width / 2.f, b.height / 2.f);
        mBaseScaleX = mHitbox.getSize().x / b.width;
        mBaseScaleY = mHitbox.getSize().y / b.height;
    }
}

void Player::syncSpritePosition() {
    if (!mTextureLoaded) return;
    sf::Vector2f pos = mHitbox.getPosition();
    sf::Vector2f size = mHitbox.getSize();
    mSprite.setPosition(pos.x + size.x / 2.f, pos.y + size.y / 2.f);
}