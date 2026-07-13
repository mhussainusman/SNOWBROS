#include "Boss.h"
#include <cmath>
#include <cstdlib>

// ─────────────────────────────────────────────
// BOSS BASE
// ─────────────────────────────────────────────

Boss::Boss(float x, float y, int maxHealth, int gemReward)
    : Enemy(x, y, 999),   // 999 = boss never encased by snowball
    mHealth(maxHealth),
    mMaxHealth(maxHealth),
    mGemReward(gemReward),
    mPhase(1)
{
}

void Boss::takeBossHit() {
    if (mHealth <= 0) return;

    mHealth--;

    // enter phase 2 at 50% health
    if (mHealth <= mMaxHealth / 2 && mPhase == 1) {
        mPhase = 2;
        // phase 2 — reddish enrage tint
        if (mTextureLoaded)
            mSprite.setColor(sf::Color(255, 130, 130));
    }

    if (mHealth <= 0)
        mDead = true;
}

bool Boss::isDefeated()   const { return mHealth <= 0; }
int  Boss::getGemReward() const { return mGemReward; }
int  Boss::getHealth()    const { return mHealth; }
int  Boss::getMaxHealth() const { return mMaxHealth; }

void Boss::drawHealthBar(sf::RenderWindow& window) {
    float bossX = mHitbox.getPosition().x;
    float bossY = mHitbox.getPosition().y;
    float bossW = mHitbox.getSize().x;
    float barX = bossX;
    float barY = bossY - 28.f;
    float barW = bossW;
    float barH = 16.f;

    // background
    sf::RectangleShape bg(sf::Vector2f(barW, barH));
    bg.setPosition(barX, barY);
    bg.setFillColor(sf::Color(80, 0, 0));
    window.draw(bg);

    // health fill
    float percent = (float)mHealth / (float)mMaxHealth;
    sf::RectangleShape fill(sf::Vector2f(barW * percent, barH));
    fill.setPosition(barX, barY);
    if (percent > 0.5f)
        fill.setFillColor(sf::Color(50, 200, 50));
    else if (percent > 0.25f)
        fill.setFillColor(sf::Color(200, 200, 0));
    else
        fill.setFillColor(sf::Color(200, 50, 50));
    window.draw(fill);

    // outline
    sf::RectangleShape outline(sf::Vector2f(barW, barH));
    outline.setPosition(barX, barY);
    outline.setFillColor(sf::Color::Transparent);
    outline.setOutlineColor(sf::Color::White);
    outline.setOutlineThickness(2.f);
    window.draw(outline);
}

// ─────────────────────────────────────────────
// MOGERA CHILD
// ─────────────────────────────────────────────

MogeraChild::MogeraChild(float x, float y, bool movingRight)
    : Enemy(x, y, 1),   // 1 hit to encase — easy to kill
    mMoveSpeed(120.f),
    mMovingRight(movingRight)
{
    mHitbox.setSize(sf::Vector2f(39.f, 39.f));
    mHitbox.setPosition(x, y);
    loadEnemyTexture("assets/Images/mogera_child.png");
    syncSpritePosition();
}

void MogeraChild::update(float deltaTime,
    const Platform* platforms, int platformCount)
{
    if (mDead) return;

    if (mRolling) {
        updateRolling(deltaTime, platforms, platformCount);
        return;
    }

    updateMelt(deltaTime);

    if (mEncased) {
        applyGravity(deltaTime, platforms, platformCount);
        syncSpritePosition();
        return;
    }

    // move in one direction — simple straight line walk
    float moveX = mMovingRight ? mMoveSpeed : -mMoveSpeed;
    mHitbox.move(moveX * deltaTime, 0.f);

    // die when off screen — don't bounce
    sf::Vector2f pos = mHitbox.getPosition();
    if (pos.x > 850.f || pos.x + mHitbox.getSize().x < -50.f)
        mDead = true;

    applyGravity(deltaTime, platforms, platformCount);
    syncSpritePosition();
}

void MogeraChild::draw(sf::RenderWindow& window, bool showHitbox) {
    if (mDead) return;

    if (!mTextureLoaded) {
        window.draw(mHitbox);
    }
    else {
        applySpriteScale(mMovingRight);
        window.draw(mSprite);
    }

    if (showHitbox) {
        sf::RectangleShape debug;
        debug.setPosition(mHitbox.getPosition());
        debug.setSize(mHitbox.getSize());
        debug.setFillColor(sf::Color::Transparent);
        debug.setOutlineColor(sf::Color::Red);
        debug.setOutlineThickness(2.f);
        window.draw(debug);
    }
}

int MogeraChild::getPoints() const { return 50; }

// ─────────────────────────────────────────────
// MOGERA
// ─────────────────────────────────────────────

Mogera::Mogera(float x, float y)
    : Boss(x, y, 30, 200),  // 30 health, 200 gems on defeat
    mSpawnTimer(0.f),
    mSpawnInterval(3.f),  // spawns a child every 3 seconds
    mChildReady(false),
    mPendingChild(nullptr)
{
    // Mogera is large — takes up significant screen space
    mHitbox.setSize(sf::Vector2f(130.f, 110.f));
    mHitbox.setPosition(x, y);
    loadEnemyTexture("assets/Images/mogera.png");
    syncSpritePosition();
}

void Mogera::update(float deltaTime,
    const Platform* platforms, int platformCount)
{
    if (mDead) return;

    // Mogera is mostly stationary — just apply gravity to land
    applyGravity(deltaTime, platforms, platformCount);
    syncSpritePosition();

    // phase 2 — spawn children faster
    float interval = (mPhase == 2) ? 1.5f : mSpawnInterval;

    // spawn child enemy periodically
    mSpawnTimer += deltaTime;
    if (mSpawnTimer >= interval) {
        mSpawnTimer = 0.f;

        // spawn child on alternating sides
        float childX = (rand() % 2 == 0) ?
            mHitbox.getPosition().x - 30.f :
            mHitbox.getPosition().x + mHitbox.getSize().x + 10.f;
        float childY = mHitbox.getPosition().y;
        bool goRight = (childX > mHitbox.getPosition().x);

        mPendingChild = new MogeraChild(childX, childY, goRight);
        mChildReady = true;
    }
}

MogeraChild* Mogera::takeChild() {
    mChildReady = false;
    MogeraChild* child = mPendingChild;
    mPendingChild = nullptr;
    return child;
}

void Mogera::draw(sf::RenderWindow& window, bool showHitbox) {
    if (mDead) return;

    if (mTextureLoaded) {
        applySpriteScale(true);
        window.draw(mSprite);
    }
    else
        window.draw(mHitbox);

    // health bar drawn above boss
    drawHealthBar(window);

    if (showHitbox) {
        sf::RectangleShape debug;
        debug.setPosition(mHitbox.getPosition());
        debug.setSize(mHitbox.getSize());
        debug.setFillColor(sf::Color::Transparent);
        debug.setOutlineColor(sf::Color(255, 0, 255));
        debug.setOutlineThickness(3.f);
        window.draw(debug);
    }
}

int Mogera::getPoints() const { return 5000; }

// GAMAKICHI


Gamakichi::Gamakichi(float x, float y)
    : Boss(x, y, 40, 500),  // 40 health, 500 gems on defeat
    mFireTimer(0.f),
    mFireInterval(2.f),
    mRocketReady(false),
    mRocketCount(0),
    mRocketsFired(0)
{
    mHitbox.setSize(sf::Vector2f(140.f, 110.f));
    mHitbox.setPosition(x, y);
    loadEnemyTexture("assets/Images/gamakichi.png");
    syncSpritePosition();

    mPlayerPos = sf::Vector2f(400.f, 300.f);
}

void Gamakichi::update(float deltaTime,
    const Platform* platforms, int platformCount)
{
    if (mDead) return;

    applyGravity(deltaTime, platforms, platformCount);
    syncSpritePosition();

    // update existing rockets
    updateRockets(deltaTime);

    // phase 2 fires faster
    float interval = (mPhase == 2) ? 1.f : mFireInterval;

    mFireTimer += deltaTime;
    if (mFireTimer >= interval) {
        mFireTimer = 0.f;

        sf::Vector2f center = sf::Vector2f(
            mHitbox.getPosition().x + mHitbox.getSize().x / 2.f,
            mHitbox.getPosition().y + mHitbox.getSize().y / 2.f);

        // phase 1 — fire at player
        // phase 2 — fire in 4 directions simultaneously
        if (mPhase == 1) {
            sf::Vector2f dir = mPlayerPos - center;
            float len = sqrt(dir.x * dir.x + dir.y * dir.y);
            if (len > 0) dir = sf::Vector2f(dir.x / len, dir.y / len);
            fireRocket(dir);
        }
        else {
            // fire in 4 directions
            fireRocket(sf::Vector2f(1.f, 0.f));
            fireRocket(sf::Vector2f(-1.f, 0.f));
            fireRocket(sf::Vector2f(0.f, 1.f));
            fireRocket(sf::Vector2f(0.7f, 0.7f));
        }
    }
}

void Gamakichi::fireRocket(sf::Vector2f direction) {
    if (mRocketCount >= 20) return;

    static sf::Texture rocketTexture;
    static bool rocketTextureLoaded =
        rocketTexture.loadFromFile("assets/Images/gamakichi_rocket.png");

    GamakichiRocket r;
    if (rocketTextureLoaded) {
        r.sprite.setTexture(rocketTexture);
        sf::FloatRect b = r.sprite.getLocalBounds();
        r.sprite.setOrigin(b.width / 2.f, b.height / 2.f);

        // scale down to a reasonable projectile size — source PNG is a
        // cropped sprite-sheet frame, much larger than the old 16x8 rectangle
        sf::Vector2u texSize = rocketTexture.getSize();
        if (texSize.y > 0) {
            float targetHeight = 36.f; // change rocket's size
            float scale = targetHeight / (float)texSize.y;
            r.sprite.setScale(scale, scale);
        }
    }
    r.sprite.setPosition(
        mHitbox.getPosition().x + mHitbox.getSize().x / 2.f,
        mHitbox.getPosition().y + mHitbox.getSize().y / 2.f);
    r.velocity = direction * 250.f;
    r.active = true;

    mRockets[mRocketCount++] = r;
}

void Gamakichi::updateRockets(float deltaTime) {
    for (int i = 0; i < mRocketCount; i++) {
        if (!mRockets[i].active) continue;

        mRockets[i].sprite.move(
            mRockets[i].velocity * deltaTime);

        // deactivate when off screen
        sf::Vector2f pos = mRockets[i].sprite.getPosition();
        if (pos.x < -20.f || pos.x > 820.f ||
            pos.y < -20.f || pos.y > 680.f)
            mRockets[i].active = false;
    }

    // remove inactive rockets
    for (int i = mRocketCount - 1; i >= 0; i--) {
        if (!mRockets[i].active) {
            for (int k = i; k < mRocketCount - 1; k++)
                mRockets[k] = mRockets[k + 1];
            mRocketCount--;
        }
    }
}

void Gamakichi::drawRockets(sf::RenderWindow& window) {
    for (int i = 0; i < mRocketCount; i++) {
        if (mRockets[i].active)
            window.draw(mRockets[i].sprite);
    }
}

bool Gamakichi::checkRocketHit(sf::FloatRect playerBounds) {
    for (int i = 0; i < mRocketCount; i++) {
        if (!mRockets[i].active) continue;
        if (mRockets[i].sprite.getGlobalBounds().intersects(playerBounds)) {
            mRockets[i].active = false;
            return true;
        }
    }
    return false;
}

void Gamakichi::draw(sf::RenderWindow& window, bool showHitbox) {
    if (mDead) return;

    if (mTextureLoaded) {
        applySpriteScale(true);
        window.draw(mSprite);
    }
    else
        window.draw(mHitbox);

    drawRockets(window);
    drawHealthBar(window);

    if (showHitbox) {
        sf::RectangleShape debug;
        debug.setPosition(mHitbox.getPosition());
        debug.setSize(mHitbox.getSize());
        debug.setFillColor(sf::Color::Transparent);
        debug.setOutlineColor(sf::Color(255, 0, 255));
        debug.setOutlineThickness(3.f);
        window.draw(debug);
    }
}

int Gamakichi::getPoints() const { return 10000; }

GamakichiRocket Gamakichi::takeRocket() {
    mRocketReady = false;
    return mPendingRocket;
}