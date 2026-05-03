#pragma once
#include <SFML/Graphics.hpp>
#include "Player.h"
#include "Platform.h"
#include "Enemy.h"
#include "Snowball.h"
#include "HUD.h"
#include "LevelManager.h"
#include "AccountManager.h"
#include "Leaderboard.h"
#include <fstream>
#include <string>
<<<<<<< Updated upstream
#include "PowerUp.h"
=======
using namespace std;
>>>>>>> Stashed changes

// all possible game screens
enum GameState {
    LOGIN,
    CHARACTER_SELECT,
    MAIN_MENU,
    PLAYING,
    LEVEL_COMPLETE,
    PAUSED,
    GAME_OVER,
};

// character data — name and color for now
// sprite added later
struct Character {
    string name;
    sf::Color color;
};

class Game {
public:
    Game();
    ~Game();
    void run();

private:
    //
   
    void handleLoginEvent(const sf::Event& event);

    //---login check---
    
    Account_Manager mAccountManager;
    PlayerAccount  mCurrentPlayer;
    bool mLoginSuccess = false;

    // --- LOGIN ---
    enum class LoginState { TYPING_USER, TYPING_PASS, DONE };
    LoginState  mLoginState = LoginState::TYPING_USER;
    string mUsernameInput;
    string mPasswordInput;
    string mLoginMessage;
    string mLoggedInUser;
  
    bool mTypingUsername;


   

    // --- GAME LOOP ---
    void processEvents();
    void update(float deltaTime);
    void render();

    // --- SCREEN UPDATES ---
    void updateLeaderboard(float deltaTime);
    void updateLogin();
    void updateCharSelect();
    void updateMainMenu();
    void updatePlaying(float deltaTime);
    void updatePaused();
    void updateGameOver();
    void updateVictory();

    // --- SCREEN RENDERS ---
    void renderLogin();
    void renderCharSelect();
    void renderMainMenu();
    void renderPlaying();
    void renderLevelComplete();
    void renderPaused();
    void renderGameOver();
    void renderVictory();

    // --- GAMEPLAY HELPERS ---
    void checkSnowballEnemyCollision();
    void checkPlayerEnemyCollision();
    void checkRollingEnemyCollision();
    void checkKnifePlayerCollision();
    void loadCurrentLevel();
    void checkLevelComplete();

    // helper to draw centered text
    void drawCenteredText(sf::Text& text, float y);

    // --- WINDOW ---
    sf::RenderWindow mWindow;
    sf::Font mFont;           // shared font for all screens
    GameState mState;         // current screen

    // --- PLAYERS ---
    Player mPlayer1; 
    Player mPlayer2;

    // --- DYNAMIC ARRAYS ---
    Platform* mPlatforms;
    int mPlatformCount;
    int mPlatformCapacity;

    Enemy** mEnemies;
    int mEnemyCount;
    int mEnemyCapacity;

    Snowball* mSnowballs;
    int mSnowballCount;
    int mSnowballCapacity;

    // --- GAME DATA ---
    bool mShowHitboxes;
    int mScore1;
    int mScore2;
    bool mGameOver;
    int mCurrentLevel;
    
    // --- SYSTEMS ---
    Leaderboard  mLeaderboard;
    HUD mHUD;
    LevelManager mLevelManager;

            // true = typing username, false = password
  
    // --- CHARACTER SELECT ---
    Character mCharacters[3];     // 3 available characters
    int mP1CharIndex;             // which character player 1 selected
    int mP2CharIndex;             // which character player 2 selected
    bool mP1Selected;             // player 1 confirmed selection
    bool mP2Selected;             // player 2 confirmed selection

    // --- MAIN MENU ---
    int mMenuSelection;           // which menu item is highlighted

    // --- HELPERS ---
    void addPlatform(Platform p);
    void addEnemy(Enemy* e);
    void addSnowball(Snowball s);
    bool checkLogin(const std::string& user, const std::string& pass);
    void saveLogin(const std::string& user, const std::string& pass);
<<<<<<< Updated upstream

    // power ups
    PowerUp* mPowerUps;
    int mPowerUpCount;
    int mPowerUpCapacity;

    // active power up effects on players
    float mSpeedBoostTimer1;      // player 1 speed boost remaining
    float mSpeedBoostTimer2;      // player 2 speed boost remaining
    float mBalloonTimer1;         // player 1 balloon mode remaining
    float mBalloonTimer2;         // player 2 balloon mode remaining
    bool mSnowballPower1;         // player 1 one hit encase
    bool mSnowballPower2;         // player 2 one hit encase
    bool mDistanceBoost1;         // player 1 full screen snowball
    bool mDistanceBoost2;         // player 2 full screen snowball

    int mGemCount1;               // player 1 gems
    int mGemCount2;               // player 2 gems

    // helper functions
    void addPowerUp(PowerUp p);
    void spawnPowerUp(float x, float y);
    void checkPowerUpCollection();
    void updatePowerUpEffects(float deltaTime);

    void applyPowerUp(PowerUpType type, int player);
    


=======
    void saveScoreToLeaderboard();
    bool mLeaderboardKeyHeld;
    bool mScoreSaved;
>>>>>>> Stashed changes
};