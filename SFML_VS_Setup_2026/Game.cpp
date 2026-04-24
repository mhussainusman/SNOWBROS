#include "Game.h"

Game::Game()
    : mWindow(sf::VideoMode(800, 600), "Snow Bros", sf::Style::Close),
    mPlayer1(0),   // player 1
    mPlayer2(1),   // player 2
    mShowHitboxes(false) {

    mWindow.setFramerateLimit(60);

    // temporary test platforms
    mPlatforms.push_back(Platform(0.f, 550.f, 800.f, 20.f));
    mPlatforms.push_back(Platform(50.f, 440.f, 180.f, 20.f));
    mPlatforms.push_back(Platform(550.f, 420.f, 180.f, 20.f));
    mPlatforms.push_back(Platform(250.f, 330.f, 180.f, 20.f));
    mPlatforms.push_back(Platform(100.f, 220.f, 170.f, 20.f));
    mPlatforms.push_back(Platform(430.f, 200.f, 170.f, 20.f));
    mPlatforms.push_back(Platform(250.f, 110.f, 200.f, 20.f));

    // test enemies
    mEnemies.push_back(new Botom(200.f, 50.f));
    mEnemies.push_back(new Botom(500.f, 50.f));
    mEnemies.push_back(new Botom(350.f, 50.f));
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
            if (event.key.code == sf::Keyboard::H)
                mShowHitboxes = !mShowHitboxes;
        }
    }
}

void Game::update(float deltaTime) {

    // update both players
    mPlayer1.update(deltaTime, mPlatforms);
    mPlayer2.update(deltaTime, mPlatforms);

    // check if player 1 wants to throw
    if (mPlayer1.wantsToThrow()) {
        // spawn snowball at player 1 position
        sf::Vector2f pos = mPlayer1.getPosition();
        float snowX = mPlayer1.isFacingRight() ?
            pos.x + 40.f :  // right side of player
            pos.x - 20.f;   // left side of player
        float snowY = pos.y + 17.f; // middle height of player
        mSnowballs.push_back(Snowball(snowX, snowY,
            mPlayer1.isFacingRight(), 0));
    }

    // check if player 2 wants to throw
    if (mPlayer2.wantsToThrow()) {
        sf::Vector2f pos = mPlayer2.getPosition();
        float snowX = mPlayer2.isFacingRight() ?
            pos.x + 40.f :
            pos.x - 20.f;
        float snowY = pos.y + 17.f;
        mSnowballs.push_back(Snowball(snowX, snowY,
            mPlayer2.isFacingRight(), 1));
    }

    // update all active snowballs
    for (int i = 0; i < mSnowballs.size(); i++)
        mSnowballs[i].update(deltaTime);

    // remove expired snowballs
    for (int i = mSnowballs.size() - 1; i >= 0; i--) {
        if (mSnowballs[i].isExpired())
            mSnowballs.erase(mSnowballs.begin() + i);
    }

    // update all enemies
    for (int i = 0; i < mEnemies.size(); i++)
        mEnemies[i]->update(deltaTime, mPlatforms);

    // check snowball hits on enemies
    checkSnowballEnemyCollision();

    // check player touching enemies
    checkPlayerEnemyCollision();
    
    checkRollingEnemyCollision();

    // remove dead enemies
    for (int i = mEnemies.size() - 1; i >= 0; i--) {
        if (mEnemies[i]->isDead()) {
            delete mEnemies[i];
            mEnemies.erase(mEnemies.begin() + i);
        }
    }
}

void Game::checkSnowballEnemyCollision() {

    for (int i = 0; i < mSnowballs.size(); i++) {

        // skip already expired snowballs
        if (mSnowballs[i].isExpired())
            continue;

        for (int j = 0; j < mEnemies.size(); j++) {

            // check if snowball hitbox overlaps enemy hitbox
            if (mSnowballs[i].getBounds().intersects(
                mEnemies[j]->getBounds())) {

                // apply snow damage to enemy
                mEnemies[j]->takeDamage();

                // remove snowball
                mSnowballs[i].setExpired();

                // move to next snowball
                break;
            }
        }
    }
}

void Game::checkPlayerEnemyCollision() {

    for (int i = 0; i < mEnemies.size(); i++) {

        // --- PLAYER 1 vs ENEMY ---
        if (mPlayer1.getBounds().intersects(mEnemies[i]->getBounds())) {

            // if enemy fully encased and player presses kick
            if (mEnemies[i]->isFullyEncased() &&
                sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
                mEnemies[i]->startRolling(mPlayer1.isFacingRight());
            }

            // player touching normal enemy = lose life
            // we will add life loss logic later
        }

        // --- PLAYER 2 vs ENEMY ---
        if (mPlayer2.getBounds().intersects(mEnemies[i]->getBounds())) {

            if (mEnemies[i]->isFullyEncased() &&
                sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
                mEnemies[i]->startRolling(mPlayer2.isFacingRight());
            }

            // player touching normal enemy = lose life
            // we will add life loss logic later
        }
    }
}

void Game::render() {
    mWindow.clear(sf::Color(30, 30, 50));

    // draw platforms
    for (int i = 0; i < mPlatforms.size(); i++)
        mPlatforms[i].draw(mWindow, mShowHitboxes);

    // draw enemies
    for (int i = 0; i < mEnemies.size(); i++)
        mEnemies[i]->draw(mWindow, mShowHitboxes);

    // draw snowballs
    for (int i = 0; i < mSnowballs.size(); i++)
        mSnowballs[i].draw(mWindow, mShowHitboxes);

    // draw players on top of everything
    mPlayer1.draw(mWindow, mShowHitboxes);
    mPlayer2.draw(mWindow, mShowHitboxes);

    mWindow.display();
}

void Game::checkRollingEnemyCollision() {

    for (int i = 0; i < mEnemies.size(); i++) {

        // only care about rolling enemies
        if (!mEnemies[i]->isRolling()) continue;

        for (int j = 0; j < mEnemies.size(); j++) {

            // dont check enemy against itself
            if (i == j) continue;

            // skip enemies that are already rolling or dead
            if (mEnemies[j]->isRolling()) continue;
            if (mEnemies[j]->isDead()) continue;

            // check if rolling enemy overlaps this enemy
            if (!mEnemies[i]->getBounds().intersects(
                mEnemies[j]->getBounds()))
                continue;

            // collision happened
            if (mEnemies[j]->isFullyEncased()) {
                // encased enemy joins the roll
                // rolls in same direction as the rolling snowball
                mEnemies[j]->startRolling(mEnemies[i]->isRollingRight());
            }
            else {
                // normal enemy hit by rolling snowball → dies instantly
                mEnemies[j]->setDead();
            }
        }
    }
}