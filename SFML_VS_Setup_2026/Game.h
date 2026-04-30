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
using namespace std;

// all possible game screens
enum GameState {
    LOGIN,
    CHARACTER_SELECT,
    MAIN_MENU,
    PLAYING,
    LEVEL_COMPLETE,
    PAUSED,
    GAME_OVER,
    VICTORY,
    LEADERBOARD_SCREEN
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
    void updateLevelComplete(float deltaTime);
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
    float mLevelCompleteTimer; // how long level complete screen shows

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
    void saveScoreToLeaderboard();
    bool mLeaderboardKeyHeld;
    bool mScoreSaved;
};