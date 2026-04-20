#include "Game.h"

Game::Game()
    : mWindow(sf::VideoMode(800, 600), "Snow Bros"),
    mPlayer() {
    mWindow.setFramerateLimit(60);
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
    mPlayer.update(deltaTime);
}

void Game::render() {
    mWindow.clear(sf::Color(30, 30, 50));
    mPlayer.draw(mWindow);
    mWindow.display();
}