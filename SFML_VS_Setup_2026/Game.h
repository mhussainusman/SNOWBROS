#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Player.h"
#include "Platform.h"
using namespace std;

// @class Game: Owns the window and drives the game loop.
 
class Game {
public:
    Game();
    void run();

private:
    void processEvents();
    void update(float deltaTime);
    void render();
    // member variables
	sf::RenderWindow mWindow; 
    Player mPlayer; // from player.h
	vector<Platform> mPlatforms; // from platform.h, vector to hold multiple platforms
};