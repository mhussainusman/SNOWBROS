#include "Game.h"

Game::Game()
    : mWindow(sf::VideoMode(800, 600), "Snow Bros"),
    mPlayer() {
    mWindow.setFramerateLimit(60);

    // create some platforms for testing
    // format: x, y, width, height
    mPlatforms.push_back(Platform(0.f, 550.f, 800.f, 20.f));  // ground
    mPlatforms.push_back(Platform(100.f, 400.f, 200.f, 20.f));  // low platform
    mPlatforms.push_back(Platform(400.f, 300.f, 200.f, 20.f));  // mid platform
    mPlatforms.push_back(Platform(200.f, 180.f, 200.f, 20.f));  // high platform
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

        if (event.type == sf::Event::KeyPressed &&
            event.key.code == sf::Keyboard::Escape)
            mWindow.close();
    }
}

void Game::update(float deltaTime) {
    mPlayer.update(deltaTime, mPlatforms);
}

void Game::render() {
    mWindow.clear(sf::Color(30, 30, 50));

    // draw platforms first then player on top
    for (auto& platform : mPlatforms)
        platform.draw(mWindow);

    mPlayer.draw(mWindow);
    mWindow.display();
}