#pragma once
#include <SFML/Graphics.hpp>
#include "Player.h"
#include "Platform.h"
#include"Enemy.h"
#include "Snowball.h"
#include"HUD.h"
#include"LevelManager.h"

// Game: owns the window, runs the game loop
class Game {
public:
    Game();
    void run();
	~Game();

private:
    void processEvents();
    void update(float deltaTime);
    void render();

   
    void checkPlayerEnemyCollision();   // checks if player touches enemy
    void checkSnowballEnemyCollision();
    void checkRollingEnemyCollision(); // rolling snowball hits other enemies
	void checkKnifePlayerCollision();   // knife hits player

    // internal helpers to manage dynamic collections (used by Game.cpp)
    void addPlatform(Platform p);
    void addEnemy(Enemy* e);
    void addSnowball(Snowball s);

    sf::RenderWindow mWindow;

    Player mPlayer1 ;
    Player mPlayer2;
  
    // platforms
    Platform* mPlatforms;
    int mPlatformCount;
    int mPlatformCapacity;

    // enemies
    Enemy** mEnemies;
    int mEnemyCount;
    int mEnemyCapacity;

    // snowballs
    Snowball* mSnowballs;
    int mSnowballCount;
    int mSnowballCapacity;


    bool mShowHitboxes; // toggled by pressing H key

    int mScore1;
    int mScore2;
    bool mGameOver;

    HUD mHUD;
    int mCurrentLevel;

    LevelManager mLevelManager;
	void loadCurrentLevel();
	void checkLevelComplete();


};