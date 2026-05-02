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
#include "Shop.h"
#include <fstream>
#include <string>
using namespace std;

// all possible game screens
enum GameState {
    LOGIN,
    REGISTER,
    CHARACTER_SELECT,
    MAIN_MENU,
    PLAYING,
    LEVEL_COMPLETE,
    PAUSED,
    GAME_OVER,
    VICTORY,
    LEADERBOARD_SCREEN,
    SHOP_SCREEN
};

// character data
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

    // ---------------------------------------------------------------
    // LOGIN SYSTEM
    // ---------------------------------------------------------------

    sf::Texture mLoginBgTexture;
    sf::Sprite  mLoginBgSprite;

    sf::Texture mMenuBgTexture;
    sf::Sprite  mMenuBgSprite;

    sf::Texture mLeaderboardBgTexture;
    sf::Sprite  mLeaderboardBgSprite;

    sf::Texture mShopBgTexture;
    sf::Sprite  mShopBgSprite;

    sf::Texture mCharSelectBgTexture;
    sf::Sprite  mCharSelectBgSprite;

    // Which player is currently logging in (1 or 2)
    int mLoginPlayerTurn;        // 1 = P1 logging in, 2 = P2 logging in

    // Per-player login data
    std::string mP1Username;
    std::string mP2Username;
    PlayerAccount mP1Account;
    PlayerAccount mP2Account;

    // Tracks which field is active on login/register screens
    enum class LoginState { TYPING_USER, TYPING_PASS, DONE };
    LoginState  mLoginState = LoginState::TYPING_USER;

    // Shared input buffers (reused for both players and register screen)
    std::string mUsernameInput;
    std::string mPasswordInput;
    std::string mConfirmPassInput;   // for register screen
    bool        mTypingConfirm;      // true = cursor in confirm field

    // Message shown at bottom of login/register screens
    std::string mLoginMessage;
    bool        mLoginSuccess = false;

    // For leaderboard display (best username = P1 or P2 winner)
    std::string mLoggedInUser;

    // Kept for old checkLogin / saveLogin helpers (unused but declared)
    bool mTypingUsername;

    // Account manager — reads/writes accounts.dat
    Account_Manager mAccountManager;

    // --- SHOP ---
    Shop mShop;   // single shared shop for both players
    void renderShop();
    void updateShop(float deltaTime);
    GameState mShopReturnState;   // PAUSED or LEVEL_COMPLETE


    //for calling

    void loadShopsForPlayers();   // loads shared shop for both players
    void saveShopsForPlayers();   // saves shared shop for both players


    // ---------------------------------------------------------------
    // EVENT HANDLERS
    // ---------------------------------------------------------------
    void handleLoginEvent(const sf::Event& event);
    void handleRegisterEvent(const sf::Event& event);

    // ---------------------------------------------------------------
    // GAME LOOP
    // ---------------------------------------------------------------
    void processEvents();
    void update(float deltaTime);
    void render();

    // ---------------------------------------------------------------
    // SCREEN UPDATES
    // ---------------------------------------------------------------
    void updateLeaderboard(float deltaTime);
    void updateLogin();
    void updateRegister();
    void updateCharSelect();
    void updateMainMenu();
    void updatePlaying(float deltaTime);
    void updateLevelComplete(float deltaTime);
    void updatePaused();
    void updateGameOver();
    void updateVictory();

    // ---------------------------------------------------------------
    // SCREEN RENDERS
    // ---------------------------------------------------------------
    void renderLogin();
    void renderRegister();
    void renderCharSelect();
    void renderMainMenu();
    void renderPlaying();
    void renderLevelComplete();
    void renderPaused();
    void renderGameOver();
    void renderVictory();
    void renderLeaderboard();

    // ---------------------------------------------------------------
    // GAMEPLAY HELPERS
    // ---------------------------------------------------------------
    void checkSnowballEnemyCollision();
    void checkPlayerEnemyCollision();
    void checkRollingEnemyCollision();
    void checkKnifePlayerCollision();
    void loadCurrentLevel();
    void checkLevelComplete();
    void drawCenteredText(sf::Text& text, float y);

    // ---------------------------------------------------------------
    // WINDOW / FONT
    // ---------------------------------------------------------------
    sf::RenderWindow mWindow;
    sf::Font         mFont;
    GameState        mState;

    // ---------------------------------------------------------------
    // PLAYERS
    // ---------------------------------------------------------------
    Player mPlayer1;
    Player mPlayer2;

    // ---------------------------------------------------------------
    // DYNAMIC ARRAYS
    // ---------------------------------------------------------------
    Platform* mPlatforms;
    int mPlatformCount;
    int mPlatformCapacity;

    Enemy** mEnemies;
    int mEnemyCount;
    int mEnemyCapacity;

    Snowball* mSnowballs;
    int mSnowballCount;
    int mSnowballCapacity;

    // ---------------------------------------------------------------
    // GAME DATA
    // ---------------------------------------------------------------
    bool  mShowHitboxes;
    int   mScore1;
    int   mScore2;
    bool  mGameOver;
    int   mCurrentLevel;
    float mLevelCompleteTimer;

    // ---------------------------------------------------------------
    // SYSTEMS
    // ---------------------------------------------------------------
    Leaderboard  mLeaderboard;
    HUD          mHUD;
    LevelManager mLevelManager;

    // ---------------------------------------------------------------
    // CHARACTER SELECT
    // ---------------------------------------------------------------
    Character mCharacters[3];
    int  mP1CharIndex;
    int  mP2CharIndex;
    bool mP1Selected;
    bool mP2Selected;

    // ---------------------------------------------------------------
    // MAIN MENU
    // ---------------------------------------------------------------
    int mMenuSelection;

    // ---------------------------------------------------------------
    // HELPERS
    // ---------------------------------------------------------------
    void addPlatform(Platform p);
    void addEnemy(Enemy* e);
    void addSnowball(Snowball s);
    bool checkLogin(const std::string& user, const std::string& pass);
    void saveLogin(const std::string& user, const std::string& pass);
    void saveScoreToLeaderboard();
    bool mLeaderboardKeyHeld;
    bool mScoreSaved;
};