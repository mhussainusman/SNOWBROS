#include "Game.h"

Game::Game()
    : mWindow(sf::VideoMode(800, 660), "Snow Bros", sf::Style::Close),
    mPlayer1(0),
    mPlayer2(1),
    mShowHitboxes(false),
    mGameOver(false),
    mScore1(0),
    mScore2(0),
    mCurrentLevel(1),
    mPlatformCount(0), mPlatformCapacity(10),
    mEnemyCount(0), mEnemyCapacity(10),
    mSnowballCount(0), mSnowballCapacity(10)
    {

    mPlatforms = new Platform[mPlatformCapacity];
    mEnemies = new Enemy * [mEnemyCapacity];
    mSnowballs = new Snowball[mSnowballCapacity];

    mWindow.setFramerateLimit(60);

    addPlatform(Platform(0.f, 615.f, 800.f, 20.f));
    addPlatform(Platform(80.f, 495.f, 640.f, 20.f));
    addPlatform(Platform(200.f, 385.f, 600.f, 20.f));
    addPlatform(Platform(0.f, 275.f, 600.f, 20.f));
    addPlatform(Platform(150.f, 185.f, 500.f, 20.f));


    mHUD.loadFont("assets/Fonts/ps2.ttf");


    // test enemies
    addEnemy(new Botom(100.f, 480.f));
    addEnemy(new Botom(600.f, 360.f));
    addEnemy(new Botom(300.f, 240.f));
    addEnemy(new FlyingEnemy(400.f, 300.f));
    addEnemy(new Tornado(400.f, 100.f));


}

void Game::run() {
    sf::Clock clock;
    while (mWindow.isOpen()) {
        float deltaTime = clock.restart().asSeconds();
        processEvents();
        update(deltaTime);
        render();
    }
}

void Game::processEvents() {
    sf::Event event;
    while (mWindow.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            mWindow.close();

        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Escape)
                mWindow.close();

            if (!mGameOver) {
                if (event.key.code == sf::Keyboard::H)
                    mShowHitboxes = !mShowHitboxes;
            }
        }
    }
}

void Game::update(float deltaTime) {

    // stop everything if game is over
    if (mGameOver) return;

    // update both players
    mPlayer1.update(deltaTime, mPlatforms, mPlatformCount);
    mPlayer2.update(deltaTime, mPlatforms, mPlatformCount);

    // player 1 throw
    if (mPlayer1.wantsToThrow()) {
        sf::Vector2f pos = mPlayer1.getPosition();
        float snowX = mPlayer1.isFacingRight() ?
            pos.x + 40.f : pos.x - 20.f;
        float snowY = pos.y + 17.f;
        addSnowball(Snowball(snowX, snowY,
            mPlayer1.isFacingRight(), 0));
    }

    // player 2 throw
    if (mPlayer2.wantsToThrow()) {
        sf::Vector2f pos = mPlayer2.getPosition();
        float snowX = mPlayer2.isFacingRight() ?
            pos.x + 40.f : pos.x - 20.f;
        float snowY = pos.y + 17.f;
        addSnowball(Snowball(snowX, snowY,
            mPlayer2.isFacingRight(), 1));
    }

    // update snowballs
    for (int i = 0; i < mSnowballCount; i++)
        mSnowballs[i].update(deltaTime);

    // remove expired snowballs
    for (int i = mSnowballCount - 1; i >= 0; i--) {
        if (mSnowballs[i].isExpired()) {
            for (int k = i; k < mSnowballCount - 1; k++)
                mSnowballs[k] = mSnowballs[k + 1];
            mSnowballCount--;

        }
            
    }

    // update enemies
    for (int i = 0; i < mEnemyCount; i++)
        mEnemies[i]->update(deltaTime, mPlatforms,mPlatformCount);

    // collision checks
    checkSnowballEnemyCollision();
    checkPlayerEnemyCollision();
    checkRollingEnemyCollision();
    checkKnifePlayerCollision();

    // remove dead enemies
    for (int i = mEnemyCount - 1; i >= 0; i--) {
        if (mEnemies[i]->isDead()) {
            delete mEnemies[i];
            for (int k = i; k < mEnemyCount - 1; k++)
                mEnemies[k] = mEnemies[k + 1];
            mEnemyCount--;
        }
    }

    // update HUD ← ADD HERE
    mHUD.update(mScore1, mPlayer1.getLives(),
        mScore2, mPlayer2.getLives(),
        mCurrentLevel);

    // game over when both players out of lives
    if (!mPlayer1.isAlive() && !mPlayer2.isAlive())
        mGameOver = true;
}

void Game::checkSnowballEnemyCollision() {
    for (int i = 0; i < mSnowballCount; i++) {
        if (mSnowballs[i].isExpired()) continue;

        for (int j = 0; j < mEnemyCount; j++) {
            if (mSnowballs[i].getBounds().intersects(
                mEnemies[j]->getBounds())) {
                mEnemies[j]->takeDamage();
                mSnowballs[i].setExpired();

                break;
            }
        }
    }
}

void Game::checkPlayerEnemyCollision() {
    for (int i = 0; i < mEnemyCount; i++) {

        // handle encased enemies — kick only
        if (mEnemies[i]->isFullyEncased()) {
            if (mPlayer1.isAlive() &&
                mPlayer1.getBounds().intersects(mEnemies[i]->getBounds()) &&
                sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) 
            {
                mEnemies[i]->startRolling(mPlayer1.isFacingRight(),0);
                mScore1 += mEnemies[i]->getPoints();
               
            }

            if (mPlayer2.isAlive() &&
                mPlayer2.getBounds().intersects(
                    mEnemies[i]->getBounds()) &&
                sf::Keyboard::isKeyPressed(sf::Keyboard::S))
            {
                mEnemies[i]->startRolling(mPlayer2.isFacingRight(),1);
                mScore2 += mEnemies[i]->getPoints();
            }

            continue; // skip life loss for encased enemies
        }

        // skip rolling enemies
        if (mEnemies[i]->isRolling()) continue;

        // normal enemy touches player — lose life
        // isRespawning check prevents losing life during invincibility

        if (mEnemies[i]->isPartiallyEncased()) continue;


        if (mPlayer1.isAlive() &&
            !mPlayer1.isRespawning() &&
            mPlayer1.getBounds().intersects(mEnemies[i]->getBounds()))
            mPlayer1.loseLife();

        if (mPlayer2.isAlive() &&
            !mPlayer2.isRespawning() &&
            mPlayer2.getBounds().intersects(mEnemies[i]->getBounds())
            )
        {mPlayer2.loseLife();}
    }
}

void Game::checkRollingEnemyCollision() {
    for (int i = 0; i < mEnemyCount; i++) {
        if (!mEnemies[i]->isRolling()) continue;
        if (mEnemies[i]->isDead()) continue;

        for (int j = 0; j < mEnemyCount; j++) {
            if (i == j) continue;
            if (mEnemies[j]->isRolling()) continue;
            if (mEnemies[j]->isDead()) continue;

            if (!mEnemies[i]->getBounds().intersects(
                mEnemies[j]->getBounds())) continue;

            if (mEnemies[j]->isFullyEncased()) {
                mEnemies[j]->startRolling(
                    mEnemies[i]->isRollingRight(), 0);
                break;
            }
            else {
                mEnemies[j]->setDead();
                int points = mEnemies[j]->getPoints();
                if (mEnemies[i]->getKickedByPlayer() == 0)
                    mScore1 += points;
                else
                mScore2 += points;
                break;
            }
        }
    }
}

void Game::render() {
    // black screen when game over
    if (mGameOver) {
        mWindow.clear(sf::Color::Black);
        mWindow.display();
        return;
    }

    mWindow.clear(sf::Color(30, 30, 50));

    for (int i = 0; i < mPlatformCount; i++)
        mPlatforms[i].draw(mWindow, mShowHitboxes);

    for (int i = 0; i < mEnemyCount; i++)
        mEnemies[i]->draw(mWindow, mShowHitboxes);

    for (int i = 0; i < mSnowballCount; i++)
        mSnowballs[i].draw(mWindow, mShowHitboxes);

    mPlayer1.draw(mWindow, mShowHitboxes);
    mPlayer2.draw(mWindow, mShowHitboxes);

    mHUD.draw(mWindow);

    mWindow.display();
}

void Game::checkKnifePlayerCollision() {
    for (int i = 0; i < mEnemyCount; i++) {
        Tornado* tornado = dynamic_cast<Tornado*>(mEnemies[i]);
        if (tornado == nullptr) continue;
        if (!tornado->isKnifeActive()) continue;

        // get positions
        sf::Vector2f p1 = mPlayer1.getPosition();
        sf::Vector2f p2 = mPlayer2.getPosition();
        sf::Vector2f tPos = sf::Vector2f(
            mEnemies[i]->getBounds().left,
            mEnemies[i]->getBounds().top);

        // find nearest player and tell tornado where to aim
        if (mPlayer1.isAlive() && mPlayer2.isAlive()) {
            float dist1 = (p1.x - tPos.x) * (p1.x - tPos.x) +
                (p1.y - tPos.y) * (p1.y - tPos.y);
            float dist2 = (p2.x - tPos.x) * (p2.x - tPos.x) +
                (p2.y - tPos.y) * (p2.y - tPos.y);
            tornado->setNearestPlayerPos(dist1 < dist2 ? p1 : p2);
        }
        else if (mPlayer1.isAlive())
            tornado->setNearestPlayerPos(p1);
        else if (mPlayer2.isAlive())
            tornado->setNearestPlayerPos(p2);

        // check knife collision
        if (mPlayer1.isAlive() && !mPlayer1.isRespawning() &&
            tornado->getKnifeBounds().intersects(mPlayer1.getBounds()))
            mPlayer1.loseLife();

        if (mPlayer2.isAlive() && !mPlayer2.isRespawning() &&
            tornado->getKnifeBounds().intersects(mPlayer2.getBounds()))
            mPlayer2.loseLife();
    }
}

void Game::addPlatform(Platform p) {
    if (mPlatformCount == mPlatformCapacity) {
        mPlatformCapacity *= 2;
        Platform* temp = new Platform[mPlatformCapacity];
        for (int i = 0; i < mPlatformCount; i++)
            temp[i] = mPlatforms[i];
        delete[] mPlatforms;
        mPlatforms = temp;
    }
    mPlatforms[mPlatformCount++] = p;
}

void Game::addEnemy(Enemy* e) {
    if (mEnemyCount == mEnemyCapacity) {
        mEnemyCapacity *= 2;
        Enemy** temp = new Enemy * [mEnemyCapacity];
        for (int i = 0; i < mEnemyCount; i++)
            temp[i] = mEnemies[i];
        delete[] mEnemies;
        mEnemies = temp;
    }
    mEnemies[mEnemyCount++] = e;
}

void Game::addSnowball(Snowball s) {
    if (mSnowballCount == mSnowballCapacity) {
        mSnowballCapacity *= 2;
        Snowball* temp = new Snowball[mSnowballCapacity];
        for (int i = 0; i < mSnowballCount; i++)
            temp[i] = mSnowballs[i];
        delete[] mSnowballs;
        mSnowballs = temp;
    }
    mSnowballs[mSnowballCount++] = s;
}

Game::~Game() {
    for (int i = 0; i < mEnemyCount; i++)
        delete mEnemies[i];
    delete[] mEnemies;
    delete[] mPlatforms;
    delete[] mSnowballs;
}
