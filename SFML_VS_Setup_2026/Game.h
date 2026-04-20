#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Player.h"
#include "Platform.h"

// @class Game: Owns the window and drives the game loop.
 
class Game {
public:
    Game();
    void run();

private:
    void processEvents();
    void update(float deltaTime);
    void render();

    sf::RenderWindow mWindow;
    Player mPlayer;
    std::vector<Platform> mPlatforms; // list of platforms in the level
};