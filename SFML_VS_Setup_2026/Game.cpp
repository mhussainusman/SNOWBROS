#include "Game.h"

Game::Game() // constructor initializes window and player, sets up platforms
	: mWindow(sf::VideoMode(800, 600), "Snow Bros", sf::Style::Close), mPlayer() // player constructor called, 
    // sets up player shape and position
{
    mWindow.setFramerateLimit(60); 

   
    // format: x, y, width, height
    mPlatforms.push_back(Platform(0.f, 550.f, 800.f, 20.f));  // ground
    mPlatforms.push_back(Platform(100.f, 430.f, 200.f, 20.f));  // low platform
    mPlatforms.push_back(Platform(400.f, 320.f, 200.f, 20.f));  // mid platform
    mPlatforms.push_back(Platform(200.f, 200.f, 200.f, 20.f));  // high platform

}

void Game::run() {
	sf::Clock clock;// track time b/w frames for smooth movement

	while (mWindow.isOpen()) { // main game loop runs until window is closed, each iteration is one frame

		float deltaTime = clock.restart().asSeconds();// deltatime is 1/fps
		if (deltaTime > 0.05f) deltaTime = 0.05f; // prevents large deltaTime caused by too mch load or freezing
        // large dT->large velocity->collision can't be detected
        processEvents();
		update(deltaTime);// 
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