#pragma once
#include <SFML/Graphics.hpp>
#include "Player.h"

/**
 * @class Game
 * @brief Core game class. Owns the window and drives the game loop.
 */
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
};