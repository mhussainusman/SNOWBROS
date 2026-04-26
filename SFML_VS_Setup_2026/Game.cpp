#include "Game.h"

Game::Game()
    : mWindow(sf::VideoMode(800, 660), "Snow Bros", sf::Style::Close),
    mPlayer1(0),
    mPlayer2(1),
    mShowHitboxes(false),
    mGameOver(false),
    mScore1(0),
    mScore2(0),
    mCurrentLevel(1)
    {

    mWindow.setFramerateLimit(60);

    // ground — full width, enemies always land here
    mPlatforms.push_back(Platform(0.f, 615, 800.f, 20.f));

    // level 2 — full width minus small gaps on each side
    // enemy walks full width, falls off either side to ground
    mPlatforms.push_back(Platform(80.f, 495.f, 640.f, 20.f));

    // level 3 — offset right
    // enemy falls off left side to ground, right side to level 2
    mPlatforms.push_back(Platform(200.f, 385.f, 600.f, 20.f));

    // level 4 — offset left
    // enemy falls off right side to level 3, left side down
    mPlatforms.push_back(Platform(0.f, 275.f, 600.f, 20.f));

    // top — centered, shorter
    // enemy falls off both sides to level 4
    mPlatforms.push_back(Platform(150.f, 185.f, 500.f, 20.f));

    mHUD.loadFont("assets/Fonts/ps2.ttf");


    // test enemies
    mEnemies.push_back(new Botom(100.f, 480.f));
    mEnemies.push_back(new Botom(600.f, 360.f));
    mEnemies.push_back(new Botom(300.f, 240.f));

    mEnemies.push_back(new FlyingEnemy(400.f, 300.f));

	mEnemies.push_back(new Tornado(400.f, 100.f));


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
    mPlayer1.update(deltaTime, mPlatforms);
    mPlayer2.update(deltaTime, mPlatforms);

    // player 1 throw
    if (mPlayer1.wantsToThrow()) {
        sf::Vector2f pos = mPlayer1.getPosition();
        float snowX = mPlayer1.isFacingRight() ?
            pos.x + 40.f : pos.x - 20.f;
        float snowY = pos.y + 17.f;
        mSnowballs.push_back(Snowball(snowX, snowY,
            mPlayer1.isFacingRight(), 0));
    }

    // player 2 throw
    if (mPlayer2.wantsToThrow()) {
        sf::Vector2f pos = mPlayer2.getPosition();
        float snowX = mPlayer2.isFacingRight() ?
            pos.x + 40.f : pos.x - 20.f;
        float snowY = pos.y + 17.f;
        mSnowballs.push_back(Snowball(snowX, snowY,
            mPlayer2.isFacingRight(), 1));
    }

    // update snowballs
    for (int i = 0; i < mSnowballs.size(); i++)
        mSnowballs[i].update(deltaTime);

    // remove expired snowballs
    for (int i = mSnowballs.size() - 1; i >= 0; i--) {
        if (mSnowballs[i].isExpired())
            mSnowballs.erase(mSnowballs.begin() + i);
    }

    // update enemies
    for (int i = 0; i < mEnemies.size(); i++)
        mEnemies[i]->update(deltaTime, mPlatforms);

    // collision checks
    checkSnowballEnemyCollision();
    checkPlayerEnemyCollision();
    checkRollingEnemyCollision();
    checkKnifePlayerCollision();

    // remove dead enemies
    for (int i = mEnemies.size() - 1; i >= 0; i--) {
        if (mEnemies[i]->isDead()) {
            delete mEnemies[i];
            mEnemies.erase(mEnemies.begin() + i);
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
    for (int i = 0; i < mSnowballs.size(); i++) {
        if (mSnowballs[i].isExpired()) continue;

        for (int j = 0; j < mEnemies.size(); j++) {
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
    for (int i = 0; i < mEnemies.size(); i++) {

        // handle encased enemies — kick only
        if (mEnemies[i]->isFullyEncased()) {
            if (mPlayer1.isAlive() &&
                mPlayer1.getBounds().intersects(
                    mEnemies[i]->getBounds()) &&
                sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
                mEnemies[i]->startRolling(mPlayer1.isFacingRight());

            if (mPlayer2.isAlive() &&
                mPlayer2.getBounds().intersects(
                    mEnemies[i]->getBounds()) &&
                sf::Keyboard::isKeyPressed(sf::Keyboard::S))
                mEnemies[i]->startRolling(mPlayer2.isFacingRight());

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
    for (int i = 0; i < mEnemies.size(); i++) {
        if (!mEnemies[i]->isRolling()) continue;

        for (int j = 0; j < mEnemies.size(); j++) {
            if (i == j) continue;
            if (mEnemies[j]->isRolling()) continue;
            if (mEnemies[j]->isDead()) continue;

            if (!mEnemies[i]->getBounds().intersects(
                mEnemies[j]->getBounds())) continue;

            if (mEnemies[j]->isFullyEncased())
                mEnemies[j]->startRolling(mEnemies[i]->isRollingRight());
            else {
                mScore1 += mEnemies[j]->getPoints();
                mEnemies[j]->setDead();
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

    for (int i = 0; i < mPlatforms.size(); i++)
        mPlatforms[i].draw(mWindow, mShowHitboxes);

    for (int i = 0; i < mEnemies.size(); i++)
        mEnemies[i]->draw(mWindow, mShowHitboxes);

    for (int i = 0; i < mSnowballs.size(); i++)
        mSnowballs[i].draw(mWindow, mShowHitboxes);

    mPlayer1.draw(mWindow, mShowHitboxes);
    mPlayer2.draw(mWindow, mShowHitboxes);

    mHUD.draw(mWindow);

    mWindow.display();
}

void Game::checkKnifePlayerCollision() {
    for (int i = 0; i < mEnemies.size(); i++) {

        // cast to Tornado to access knife
        // dynamic_cast returns nullptr if not a Tornado
        Tornado* tornado = dynamic_cast<Tornado*>(mEnemies[i]);
        if (tornado == nullptr) continue;
        if (!tornado->isKnifeActive()) continue;

        // pass nearest player position to tornado
        // find which player is closer
        sf::Vector2f p1 = mPlayer1.getPosition();
        sf::Vector2f p2 = mPlayer2.getPosition();
        sf::Vector2f tPos = tornado->getKnifeBounds().left == 0 ?
            sf::Vector2f(0, 0) : sf::Vector2f(
                tornado->getKnifeBounds().left,
                tornado->getKnifeBounds().top);

        // calculate distance to each player
        float dist1 = sqrt(
            (p1.x - tPos.x) * (p1.x - tPos.x) +
            (p1.y - tPos.y) * (p1.y - tPos.y));
        float dist2 = sqrt(
            (p2.x - tPos.x) * (p2.x - tPos.x) +
            (p2.y - tPos.y) * (p2.y - tPos.y));

        // tell tornado which player is nearest
        if (mPlayer1.isAlive() && mPlayer2.isAlive())
            tornado->setNearestPlayerPos(dist1 < dist2 ? p1 : p2);
        else if (mPlayer1.isAlive())
            tornado->setNearestPlayerPos(p1);
        else if (mPlayer2.isAlive())
            tornado->setNearestPlayerPos(p2);

        // check knife collision with players
        if (mPlayer1.isAlive() &&
            !mPlayer1.isRespawning() &&
            tornado->getKnifeBounds().intersects(
                mPlayer1.getBounds()))
            mPlayer1.loseLife();

        if (mPlayer2.isAlive() &&
            !mPlayer2.isRespawning() &&
            tornado->getKnifeBounds().intersects(
                mPlayer2.getBounds()))
            mPlayer2.loseLife();
    }
}