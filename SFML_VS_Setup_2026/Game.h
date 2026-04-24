#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Player.h"
#include "Platform.h"
#include"Enemy.h"
#include "Snowball.h"
using namespace std;
// Game: owns the window, runs the game loop
class Game {
public:
    Game();
    void run();

private:
    void processEvents();
    void update(float deltaTime);
    void render();

   
    void checkPlayerEnemyCollision();   // checks if player touches enemy
    void checkSnowballEnemyCollision();
    void checkRollingEnemyCollision(); // rolling snowball hits other enemies

    sf::RenderWindow mWindow;

    Player mPlayer1 ;
    Player mPlayer2;
     vector<Platform> mPlatforms;
     vector<Enemy*> mEnemies;  // Enemy* pointer for polymorphism
     vector<Snowball>mSnowballs;

    bool mShowHitboxes; // toggled by pressing H key


};