#include "Game.h"
<<<<<<< Updated upstream
=======
#include "AccountManager.h"

>>>>>>> Stashed changes
 
// CONSTRUCTOR
 

Game::Game()
    : mWindow(sf::VideoMode(800, 660), "Snow Bros", sf::Style::Close),
    mPlayer1(0),
    mPlayer2(1),
    mShowHitboxes(false),
    mGameOver(false),
    mScore1(0),
    mScore2(0),
    mCurrentLevel(1),
    mPlatformCount(0), mPlatformCapacity(20),
    mEnemyCount(0), mEnemyCapacity(20),
    mSnowballCount(0), mSnowballCapacity(20),
    mState(LOGIN),
    mTypingUsername(true),
    mLoginMessage(""),
    mLoggedInUser(""),
    mP1CharIndex(0),
    mP2CharIndex(1),
    mP1Selected(false),
    mP2Selected(false),
    mMenuSelection(0),
<<<<<<< Updated upstream
    mPowerUpCount(0), mPowerUpCapacity(20),
    mSpeedBoostTimer1(0.f), mSpeedBoostTimer2(0.f),
    mBalloonTimer1(0.f), mBalloonTimer2(0.f),
    mSnowballPower1(false), mSnowballPower2(false),
    mDistanceBoost1(false), mDistanceBoost2(false),
    mGemCount1(0), mGemCount2(0)
{
=======
    mLevelCompleteTimer(0.f),
    mLeaderboardKeyHeld(false),
    mScoreSaved(false) {
>>>>>>> Stashed changes

    mPlatforms = new Platform[mPlatformCapacity];
    mEnemies = new Enemy * [mEnemyCapacity];
    mSnowballs = new Snowball[mSnowballCapacity];
    mPowerUps = new PowerUp[mPowerUpCapacity];

    mWindow.setFramerateLimit(60);

    // load font
    mFont.loadFromFile(
        "assets/Fonts/ps2.ttf");
    mHUD.loadFont(
        "assets/Fonts/ps2.ttf");
    mLeaderboard.setFont(mFont);

    // setup 3 characters
    mCharacters[0] = { "Nick", sf::Color(100, 180, 255) };  // blue
    mCharacters[1] = { "Tom",  sf::Color(100, 255, 150) };  // green
    mCharacters[2] = { "Max",  sf::Color(255, 165, 0) };    // orange
}

 
// DESTRUCTOR
 

Game::~Game() {
    for (int i = 0; i < mEnemyCount; i++)
        delete mEnemies[i];
    delete[] mEnemies;
    delete[] mPlatforms;
    delete[] mSnowballs;
    delete[]mPowerUps;
}

 
// MAIN LOOP
 

void Game::run() {
    sf::Clock clock;
    while (mWindow.isOpen()) {
        float deltaTime = clock.restart().asSeconds();
        if (deltaTime > 0.05f) deltaTime = 0.05f;
        processEvents();
        update(deltaTime);
        render();
    }
}

 
// PROCESS EVENTS
 

void Game::processEvents() {
    sf::Event event;
    while (mWindow.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            mWindow.close();

        // handle text input for login screen
        if (mState == LOGIN &&
            event.type == sf::Event::TextEntered) {
            char c = (char)event.text.unicode;

            if (c == '\b') {
                // backspace
                if (mTypingUsername && !mUsernameInput.empty())
                    mUsernameInput.pop_back();
                else if (!mTypingUsername && !mPasswordInput.empty())
                    mPasswordInput.pop_back();
            }
           
            else if (c == '\r' || c == '\n') {
                if (mTypingUsername) {
                    mTypingUsername = false;
                }
                else {
                    PlayerAccount acc;
                    auto result = mAccountManager.tryLogin(
                        mUsernameInput, mPasswordInput, acc);

                    if (result == Account_Manager::Result::OK) {
                        mLoggedInUser = mUsernameInput;
                        mLoginMessage = "LOGIN SUCCESSFUL!";
                        mLoginSuccess = true;
                        mState = CHARACTER_SELECT;
                    }
                    else if (result == Account_Manager::Result::USER_NOT_FOUND) {
                        // new player — auto register
                        mAccountManager.registerUser(mUsernameInput, mPasswordInput);
                        mLoggedInUser = mUsernameInput;
                        mLoginMessage = "NEW ACCOUNT CREATED!";
                        mLoginSuccess = true;
                        mState = CHARACTER_SELECT;
                    }
                    else if (result == Account_Manager::Result::WRONG_PASSWORD) {
                        mLoginMessage = "WRONG PASSWORD. TRY AGAIN.";
                        mLoginSuccess = false;
                        mPasswordInput.clear();
                    }
                    else if (result == Account_Manager::Result::EMPTY_FIELD) {
                        mLoginMessage = "ENTER USERNAME AND PASSWORD.";
                        mLoginSuccess = false;
                    }
                }
            }
            else if (c >= 32 && c < 127) {
                // normal character
                if (mTypingUsername)
                    mUsernameInput += c;
                else
                    mPasswordInput += c;
            }
        }

        if (event.type == sf::Event::KeyPressed) {

            // escape closes game from anywhere
            if (event.key.code == sf::Keyboard::Escape) {
                if (mState == PLAYING)
                    mState = PAUSED;
                else if (mState == PAUSED)
                    mState = PLAYING;
               
            }

			// DEBUG to check power up functionality, remove before submission
            if (mState == PLAYING) {
                // DEBUG — press number keys to test power ups on player 1
                // REMOVE THESE BEFORE FINAL SUBMISSION
                if (event.key.code == sf::Keyboard::P)
                    applyPowerUp(GEM, 1);
                if (event.key.code == sf::Keyboard::O)
                    applyPowerUp(SPEED_BOOST, 1);
                if (event.key.code == sf::Keyboard::I)
                    applyPowerUp(SNOWBALL_POWER, 1);
                if (event.key.code == sf::Keyboard::U)
                    applyPowerUp(DISTANCE_BOOST, 1);
                if (event.key.code == sf::Keyboard::Y)
                    applyPowerUp(BALLOON_MODE, 1);
                if (event.key.code == sf::Keyboard::T)
                    applyPowerUp(EXTRA_LIFE, 1);

            }

			// End of debug of power up functionality
            


            // hitbox toggle during gameplay
            if (mState == PLAYING &&
                event.key.code == sf::Keyboard::H)
                mShowHitboxes = !mShowHitboxes;

            // main menu navigation
            if (mState == MAIN_MENU) {
                if (event.key.code == sf::Keyboard::Up ||
                    event.key.code == sf::Keyboard::W)
                    mMenuSelection--;
                if (event.key.code == sf::Keyboard::Down ||
                    event.key.code == sf::Keyboard::S)
                    mMenuSelection++;
                if (mMenuSelection < 0) mMenuSelection = 3;
                if (mMenuSelection > 3) mMenuSelection = 0;

                if (event.key.code == sf::Keyboard::Return) {
                    if (mMenuSelection == 0) {
                        // new game
						mCurrentLevel = 1; // you can start of from any level, through this
                        mScore1 = 0;
                        mScore2 = 0;
                        mScoreSaved = false;
                        mGameOver = false;        // ← reset game over flag
                        mPlayer1.resetLives();    // ← reset player lives
                        mPlayer2.resetLives();    // ← reset player lives
                        mLevelManager.selectBonusLevels();
                        loadCurrentLevel();
                        mState = PLAYING;
                    }
                    else if (mMenuSelection == 1) {
                        // continue — load saved progress
                        loadCurrentLevel();
                        mState = PLAYING;
                    }
                    else if (mMenuSelection == 2) {
                        mLeaderboardKeyHeld = true;
                        mState = LEADERBOARD_SCREEN;
                    }
                    else if (mMenuSelection == 3) {
                        mWindow.close();
                    }
                }
            }

            // character select navigation
            if (mState == CHARACTER_SELECT) {
                // player 1 — arrow keys
                if (!mP1Selected) {
                    if (event.key.code == sf::Keyboard::Left)
                        mP1CharIndex--;
                    if (event.key.code == sf::Keyboard::Right)
                        mP1CharIndex++;
                    if (mP1CharIndex < 0) mP1CharIndex = 2;
                    if (mP1CharIndex > 2) mP1CharIndex = 0;
                    if (event.key.code == sf::Keyboard::Down)
                        mP1Selected = true;
                }

                // player 2 — WASD
                if (!mP2Selected) {
                    if (event.key.code == sf::Keyboard::A)
                        mP2CharIndex--;
                    if (event.key.code == sf::Keyboard::D)
                        mP2CharIndex++;
                    if (mP2CharIndex < 0) mP2CharIndex = 2;
                    if (mP2CharIndex > 2) mP2CharIndex = 0;
                    if (event.key.code == sf::Keyboard::S)
                        mP2Selected = true;
                }

                // both selected — go to main menu
                if (mP1Selected && mP2Selected)
                    mState = MAIN_MENU;
            }

            // level complete — press enter to continue
            if (mState == LEVEL_COMPLETE &&
                event.key.code == sf::Keyboard::Return) {
                loadCurrentLevel();
                mState = PLAYING;
            }

            // pause menu
            if (mState == PAUSED) {
                if (event.key.code == sf::Keyboard::R)
                    mState = PLAYING;
                if (event.key.code == sf::Keyboard::M)
                    mState = MAIN_MENU;
            }

            // game over — press enter to go to menu
            if (mState == GAME_OVER &&
                event.key.code == sf::Keyboard::Return)
                mState = MAIN_MENU;

            // victory — press enter to go to menu
            if (mState == VICTORY &&
                event.key.code == sf::Keyboard::Return)
                mState = MAIN_MENU;
        }
    }
}

 
// UPDATE
 

void Game::update(float deltaTime) {
    switch (mState) {
<<<<<<< Updated upstream
    case LOGIN:            updateLogin(); break;
    case CHARACTER_SELECT: updateCharSelect(); break;
    case MAIN_MENU:        updateMainMenu(); break;
    case PLAYING:          updatePlaying(deltaTime); break;
    case PAUSED:           updatePaused(); break;
    case GAME_OVER:        updateGameOver(); break;
    case VICTORY:          updateVictory(); break;
=======
    case LOGIN:              updateLogin(); break;
    case CHARACTER_SELECT:   updateCharSelect(); break;
    case MAIN_MENU:          updateMainMenu(); break;
    case PLAYING:            updatePlaying(deltaTime); break;
    case LEVEL_COMPLETE:     updateLevelComplete(deltaTime); break;
    case PAUSED:             updatePaused(); break;
    case GAME_OVER:          updateGameOver(); break;
    case VICTORY:            updateVictory(); break;
    case LEADERBOARD_SCREEN: updateLeaderboard(deltaTime); break;
>>>>>>> Stashed changes
    }
}

void Game::updateLogin() {
    // nothing to update — handled in 

}

void Game::updateCharSelect() {
    // nothing to update — handled in processEvents
}

void Game::updateMainMenu() {
    // nothing to update — handled in processEvents
}

void Game::updatePlaying(float deltaTime) {
    // update power ups
    for (int i = 0; i < mPowerUpCount; i++)
        mPowerUps[i].update(deltaTime);

    // check if player collected power up
    checkPowerUpCollection();

    // update active power up timers
    updatePowerUpEffects(deltaTime);

    if (mGameOver) {
        mState = GAME_OVER;
        return;
    }

    mPlayer1.update(deltaTime, mPlatforms, mPlatformCount);
    mPlayer2.update(deltaTime, mPlatforms, mPlatformCount);

    // player 1 throw
    if (mPlayer1.wantsToThrow()) {
     
        sf::Vector2f pos = mPlayer1.getPosition();
        float snowX = mPlayer1.isFacingRight() ?
            pos.x + 40.f : pos.x - 20.f;
        float snowY = pos.y + 17.f;
        Snowball s(snowX, snowY, mPlayer1.isFacingRight(), 0);
		if (mDistanceBoost1) s.setMaxDistance(800.f); // distance boost active? :: increase snowball range
        addSnowball(s);
    }

    // player 2 throw
    if (mPlayer2.wantsToThrow()) {
        sf::Vector2f pos = mPlayer2.getPosition();
        float snowX = mPlayer2.isFacingRight() ?
            pos.x + 40.f : pos.x - 20.f;
        float snowY = pos.y + 17.f;
        Snowball s(snowX, snowY, mPlayer2.isFacingRight(), 1);
        if (mDistanceBoost2) s.setMaxDistance(800.f);
        addSnowball(s);
    }

    // update snowballs
    for (int i = 0; i < mSnowballCount; i++)
        mSnowballs[i].update(deltaTime);

    // remove expired snowballs
    for (int i = mSnowballCount - 1; i >= 0; i--) {
        if (mSnowballs[i].isExpired()) {
            for (int k = i; k < mSnowballCount - 1; k++)
                mSnowballs[k] = mSnowballs[k + 1];
            mSnowballCount--;
        }
    }

    // update enemies
    for (int i = 0; i < mEnemyCount; i++)
        mEnemies[i]->update(deltaTime, mPlatforms, mPlatformCount);

    // collisions
    checkSnowballEnemyCollision();
    checkPlayerEnemyCollision();
    checkRollingEnemyCollision();
    checkKnifePlayerCollision();

    // remove dead enemies
    for (int i = mEnemyCount - 1; i >= 0; i--) {
        if (mEnemies[i]->isDead()) {
            delete mEnemies[i];
            for (int k = i; k < mEnemyCount - 1; k++)
                mEnemies[k] = mEnemies[k + 1];
            mEnemyCount--;
        }
    }

    // update HUD
    mHUD.update(mScore1, mPlayer1.getLives(),
        mScore2, mPlayer2.getLives(),
        mCurrentLevel, mGemCount1, mGemCount2);

    // check level complete
    checkLevelComplete();

    // check game over
    if(!mPlayer1.isAlive() && !mPlayer2.isAlive())
        mGameOver = true;
}


void Game::updatePaused() {
    // nothing — handled in processEvents
}

void Game::updateGameOver() {
    if (!mScoreSaved) {
        saveScoreToLeaderboard();
        mScoreSaved = true;
    }
}


void Game::updateVictory() {
    if (!mScoreSaved) {
        saveScoreToLeaderboard();
        mScoreSaved = true;
    }
}

 
// RENDER
 

void Game::render() {
    if (mState == LEADERBOARD_SCREEN) return;
    mWindow.clear(sf::Color(20, 20, 40));

    switch (mState) {
    case LOGIN:            renderLogin(); break;
    case CHARACTER_SELECT: renderCharSelect(); break;
    case MAIN_MENU:        renderMainMenu(); break;
    case PLAYING:          renderPlaying(); break;
    case LEVEL_COMPLETE:   renderLevelComplete(); break;
    case PAUSED:           renderPaused(); break;
    case GAME_OVER:        renderGameOver(); break;
    case VICTORY:          renderVictory(); break;
    }

    mWindow.display();
}

//======================================================
//leaderboard update
void Game::saveScoreToLeaderboard() {
    int best = (mScore1 >= mScore2) ? mScore1 : mScore2;
    mLeaderboard.addEntry(mLoggedInUser, best, mCurrentLevel);
}

void Game::updateLeaderboard(float deltaTime) {
    // keep drawing while the key that opened this screen is held
    // so the same key-press doesn't instantly close it
    if (mLeaderboardKeyHeld) {
        if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Escape) &&
            !sf::Keyboard::isKeyPressed(sf::Keyboard::BackSpace))
            mLeaderboardKeyHeld = false;

        mWindow.clear(sf::Color(5, 10, 35));
        mLeaderboard.draw(mWindow, deltaTime);
        mWindow.display();
        return;
    }

    mWindow.clear(sf::Color(5, 10, 35));
    bool goBack = mLeaderboard.draw(mWindow, deltaTime);
    mWindow.display();

    if (goBack) {
        mLeaderboardKeyHeld = true;   // debounce the back key
        mState = MAIN_MENU;
    }
}

//===================================================================

// helper — draws text centered horizontally at given Y
void Game::drawCenteredText(sf::Text& text, float y) {
    float x = (800.f - text.getGlobalBounds().width) / 2.f;
    text.setPosition(x, y);
    mWindow.draw(text);
}

 
// LOGIN SCREEN
 
// ---- renderLogin() ----------------------------------------------
void Game::renderLogin() {

    // Title
    sf::Text title;
    title.setFont(mFont);
    title.setString("SNOW BROS");
    title.setCharacterSize(40);
    title.setFillColor(sf::Color::Cyan);
    drawCenteredText(title, 80.f);

    // Username label
    sf::Text userLabel;
    userLabel.setFont(mFont);
    userLabel.setString("USERNAME:");
    userLabel.setCharacterSize(16);
    userLabel.setFillColor(sf::Color::White);
    userLabel.setPosition(150.f, 220.f);
    mWindow.draw(userLabel);

    bool typingUser = (mLoginState == LoginState::TYPING_USER);

    sf::RectangleShape userBox(sf::Vector2f(400.f, 35.f));
    userBox.setPosition(150.f, 245.f);
    userBox.setFillColor(typingUser ? sf::Color(60, 60, 100) : sf::Color(40, 40, 60));
    userBox.setOutlineColor(typingUser ? sf::Color::Cyan : sf::Color::White);
    userBox.setOutlineThickness(2.f);
    mWindow.draw(userBox);

    sf::Text userText;
    userText.setFont(mFont);
    userText.setString(mUsernameInput + (typingUser ? "_" : ""));
    userText.setCharacterSize(14);
    userText.setFillColor(sf::Color::White);
    userText.setPosition(160.f, 252.f);
    mWindow.draw(userText);

    // Password label
    sf::Text passLabel;
    passLabel.setFont(mFont);
    passLabel.setString("PASSWORD:");
    passLabel.setCharacterSize(16);
    passLabel.setFillColor(sf::Color::White);
    passLabel.setPosition(150.f, 310.f);
    mWindow.draw(passLabel);

    bool typingPass = (mLoginState == LoginState::TYPING_PASS);

    sf::RectangleShape passBox(sf::Vector2f(400.f, 35.f));
    passBox.setPosition(150.f, 335.f);
    passBox.setFillColor(typingPass ? sf::Color(60, 60, 100) : sf::Color(40, 40, 60));
    passBox.setOutlineColor(typingPass ? sf::Color::Cyan : sf::Color::White);
    passBox.setOutlineThickness(2.f);
    mWindow.draw(passBox);

    std::string stars(mPasswordInput.size(), '*');
    sf::Text passText;
    passText.setFont(mFont);
    passText.setString(stars + (typingPass ? "_" : ""));
    passText.setCharacterSize(14);
    passText.setFillColor(sf::Color::White);
    passText.setPosition(160.f, 342.f);
    mWindow.draw(passText);

    // Hints
    sf::Text hint;
    hint.setFont(mFont);
    hint.setString("PRESS ENTER TO CONFIRM EACH FIELD");
    hint.setCharacterSize(10);
    hint.setFillColor(sf::Color(150, 150, 150));
    drawCenteredText(hint, 400.f);

    sf::Text newUser;
    newUser.setFont(mFont);
    newUser.setString("NEW USER? JUST TYPE AND PRESS ENTER");
    newUser.setCharacterSize(10);
    newUser.setFillColor(sf::Color(150, 150, 150));
    drawCenteredText(newUser, 425.f);

    // Message box — red for errors, green for success
    if (!mLoginMessage.empty()) {
        if (!mLoginSuccess) {
            // Dark red box with red outline for errors
            sf::RectangleShape errorBox(sf::Vector2f(600.f, 40.f));
            errorBox.setPosition(80.f, 460.f);
            errorBox.setFillColor(sf::Color(80, 0, 0));
            errorBox.setOutlineColor(sf::Color::Red);
            errorBox.setOutlineThickness(2.f);
            mWindow.draw(errorBox);
        }

        sf::Text msg;
        msg.setFont(mFont);
        msg.setString(mLoginMessage);
        msg.setCharacterSize(14);
        msg.setFillColor(mLoginSuccess ? sf::Color::Green : sf::Color::Red);
        drawCenteredText(msg, 473.f);
    }
}

// ---- handleLoginEvent() -----------------------------------------
// Call this from your event loop:
//   case GameState::LOGIN:
//       handleLoginEvent(event);
//       break;

void Game::handleLoginEvent(const sf::Event& event) {

    if (event.type == sf::Event::TextEntered) {
        if (event.text.unicode >= 32 && event.text.unicode < 127) {
            char c = static_cast<char>(event.text.unicode);
            if (mLoginState == LoginState::TYPING_USER)
                mUsernameInput += c;
            else if (mLoginState == LoginState::TYPING_PASS)
                mPasswordInput += c;
        }
    }

    if (event.type == sf::Event::KeyPressed) {

        // Backspace
        if (event.key.code == sf::Keyboard::BackSpace) {
            if (mLoginState == LoginState::TYPING_USER && !mUsernameInput.empty())
                mUsernameInput.pop_back();
            else if (mLoginState == LoginState::TYPING_PASS && !mPasswordInput.empty())
                mPasswordInput.pop_back();
            mLoginMessage.clear();
        }

        // Tab — switch field
        if (event.key.code == sf::Keyboard::Tab) {
            mLoginState = (mLoginState == LoginState::TYPING_USER)
                ? LoginState::TYPING_PASS
                : LoginState::TYPING_USER;
            mLoginMessage.clear();
        }

        // Enter — advance or submit
        if (event.key.code == sf::Keyboard::Enter) {

            if (mLoginState == LoginState::TYPING_USER) {
                mLoginState = LoginState::TYPING_PASS;
                mLoginMessage.clear();
            }
            else if (mLoginState == LoginState::TYPING_PASS) {

                PlayerAccount acc;
                Account_Manager::Result result =
                    mAccountManager.tryLogin(mUsernameInput, mPasswordInput, acc);

                if (result == Account_Manager::Result::OK) {
                    mCurrentPlayer = acc;
                    mLoginSuccess = true;
                    mLoginMessage = "WELCOME BACK, " + acc.username + "!";
                    mLoginState = LoginState::DONE;
                    mState = GameState::MAIN_MENU; // <-- change to your state

                }
                else if (result == Account_Manager::Result::USER_NOT_FOUND) {
                    // New player — auto register then log in
                    Account_Manager::Result reg =
                        mAccountManager.registerUser(mUsernameInput, mPasswordInput);

                    if (reg == Account_Manager::Result::OK) {
                        mAccountManager.tryLogin(mUsernameInput, mPasswordInput, mCurrentPlayer);
                        mLoginSuccess = true;
                        mLoginMessage = "ACCOUNT CREATED! WELCOME, " + mUsernameInput + "!";
                        mLoginState = LoginState::DONE;
                        mState = GameState::MAIN_MENU; // <-- change to your state
                    }
                    else {
                        mLoginSuccess = false;
                        mLoginMessage = "REGISTRATION FAILED. TRY AGAIN.";
                    }
                }
                else if (result == Account_Manager::Result::WRONG_PASSWORD) {
                    mLoginSuccess = false;
                    mLoginMessage = "WRONG PASSWORD. TRY AGAIN.";
                    mPasswordInput.clear();
                }
                else if (result == Account_Manager::Result::EMPTY_FIELD) {
                    mLoginSuccess = false;
                    mLoginMessage = "USERNAME AND PASSWORD REQUIRED.";
                }
            }
        }
    }
}

 
// CHARACTER SELECT
 

void Game::renderCharSelect() {
    sf::Text title;
    title.setFont(mFont);
    title.setString("SELECT CHARACTER");
    title.setCharacterSize(24);
    title.setFillColor(sf::Color::Yellow);
    drawCenteredText(title, 50.f);

    // draw 3 characters
    for (int i = 0; i < 3; i++) {
        float x = 150.f + i * 200.f;

        // character box
        sf::RectangleShape box(sf::Vector2f(120.f, 140.f));
        box.setPosition(x, 150.f);
        box.setFillColor(sf::Color(40, 40, 70));
        box.setOutlineThickness(3.f);

        // highlight selected characters
        if (i == mP1CharIndex && i == mP2CharIndex)
            box.setOutlineColor(sf::Color::White);
        else if (i == mP1CharIndex)
            box.setOutlineColor(sf::Color::Cyan);
        else if (i == mP2CharIndex)
            box.setOutlineColor(sf::Color::Green);
        else
            box.setOutlineColor(sf::Color(80, 80, 80));

        mWindow.draw(box);

        // character visual
        sf::RectangleShape charVisual(sf::Vector2f(60.f, 80.f));
        charVisual.setPosition(x + 30.f, 160.f);
        charVisual.setFillColor(mCharacters[i].color);
        mWindow.draw(charVisual);

        // character name
        sf::Text name;
        name.setFont(mFont);
        name.setString(mCharacters[i].name);
        name.setCharacterSize(12);
        name.setFillColor(sf::Color::White);
        name.setPosition(x + 10.f, 255.f);
        mWindow.draw(name);
    }

    // player 1 instructions
    sf::Text p1inst;
    p1inst.setFont(mFont);
    p1inst.setString("P1: LEFT/RIGHT TO SELECT, DOWN TO CONFIRM");
    p1inst.setCharacterSize(9);
    p1inst.setFillColor(mP1Selected ?
        sf::Color::Green : sf::Color::Cyan);
    drawCenteredText(p1inst, 320.f);

    if (mP1Selected) {
        sf::Text p1done;
        p1done.setFont(mFont);
        p1done.setString("P1 READY: " + mCharacters[mP1CharIndex].name);
        p1done.setCharacterSize(12);
        p1done.setFillColor(sf::Color::Green);
        drawCenteredText(p1done, 345.f);
    }

    // player 2 instructions
    sf::Text p2inst;
    p2inst.setFont(mFont);
    p2inst.setString("P2: A/D TO SELECT, S TO CONFIRM");
    p2inst.setCharacterSize(9);
    p2inst.setFillColor(mP2Selected ?
        sf::Color::Green : sf::Color(100, 255, 150));
    drawCenteredText(p2inst, 380.f);

    if (mP2Selected) {
        sf::Text p2done;
        p2done.setFont(mFont);
        p2done.setString("P2 READY: " + mCharacters[mP2CharIndex].name);
        p2done.setCharacterSize(12);
        p2done.setFillColor(sf::Color(100, 255, 150));
        drawCenteredText(p2done, 405.f);
    }
}

 
// MAIN MENU
 

void Game::renderMainMenu() {
    // title
    sf::Text title;
    title.setFont(mFont);
    title.setString("SNOW BROS");
    title.setCharacterSize(48);
    title.setFillColor(sf::Color::Cyan);
    drawCenteredText(title, 80.f);

    // subtitle
    sf::Text sub;
    sub.setFont(mFont);
    sub.setString("NICK & TOM");
    sub.setCharacterSize(20);
    sub.setFillColor(sf::Color::White);
    drawCenteredText(sub, 150.f);

    // logged in user
    sf::Text user;
    user.setFont(mFont);
    user.setString("PLAYER: " + mLoggedInUser);
    user.setCharacterSize(12);
    user.setFillColor(sf::Color::Yellow);
    drawCenteredText(user, 200.f);

    // menu items
    std::string items[4] = {
        "NEW GAME",
        "CONTINUE",
        "LEADERBOARD",
        "EXIT"
    };

    for (int i = 0; i < 4; i++) {
        sf::Text item;
        item.setFont(mFont);
        item.setString(items[i]);
        item.setCharacterSize(20);

        if (i == mMenuSelection) {
            item.setFillColor(sf::Color::Yellow);
            // draw arrow
            sf::Text arrow;
            arrow.setFont(mFont);
            arrow.setString(">");
            arrow.setCharacterSize(20);
            arrow.setFillColor(sf::Color::Yellow);
            arrow.setPosition(250.f, 290.f + i * 60.f);
            mWindow.draw(arrow);
        }
        else {
            item.setFillColor(sf::Color::White);
        }

        drawCenteredText(item, 290.f + i * 60.f);
    }

    // instructions
    sf::Text hint;
    hint.setFont(mFont);
    hint.setString("UP/DOWN TO NAVIGATE   ENTER TO SELECT");
    hint.setCharacterSize(10);
    hint.setFillColor(sf::Color(150, 150, 150));
    drawCenteredText(hint, 580.f);
}

 
// PLAYING
 

void Game::renderPlaying() {
    for (int i = 0; i < mPlatformCount; i++)
        mPlatforms[i].draw(mWindow, mShowHitboxes);

    for (int i = 0; i < mEnemyCount; i++)
        mEnemies[i]->draw(mWindow, mShowHitboxes);

    for (int i = 0; i < mSnowballCount; i++)
        mSnowballs[i].draw(mWindow, mShowHitboxes);

    for (int i = 0; i < mPowerUpCount; i++)
        mPowerUps[i].draw(mWindow, mShowHitboxes);


    mPlayer1.draw(mWindow, mShowHitboxes);
    mPlayer2.draw(mWindow, mShowHitboxes);

    mHUD.draw(mWindow);
}

 
// LEVEL COMPLETE
 

void Game::renderLevelComplete() {
    // show game still in background
    renderPlaying();

    // dark overlay
    sf::RectangleShape overlay(sf::Vector2f(800.f, 660.f));
    overlay.setFillColor(sf::Color(0, 0, 0, 150));
    mWindow.draw(overlay);

    sf::Text title;
    title.setFont(mFont);
    title.setString("LEVEL COMPLETE!");
    title.setCharacterSize(36);
    title.setFillColor(sf::Color::Yellow);
    drawCenteredText(title, 200.f);

    sf::Text score;
    score.setFont(mFont);
    score.setString("P1 SCORE: " + std::to_string(mScore1));
    score.setCharacterSize(18);
    score.setFillColor(sf::Color::Cyan);
    drawCenteredText(score, 290.f);

    sf::Text score2;
    score2.setFont(mFont);
    score2.setString("P2 SCORE: " + std::to_string(mScore2));
    score2.setCharacterSize(18);
    score2.setFillColor(sf::Color(100, 255, 150));
    drawCenteredText(score2, 330.f);

    sf::Text next;
    next.setFont(mFont);
    next.setString("Press ENTER to continue");
    next.setCharacterSize(14);
    next.setFillColor(sf::Color::White);
    drawCenteredText(next, 420.f);
}

 
// PAUSE MENU
 

void Game::renderPaused() {
    // show game in background
    renderPlaying();

    // dark overlay
    sf::RectangleShape overlay(sf::Vector2f(800.f, 660.f));
    overlay.setFillColor(sf::Color(0, 0, 0, 170));
    mWindow.draw(overlay);

    sf::Text title;
    title.setFont(mFont);
    title.setString("PAUSED");
    title.setCharacterSize(40);
    title.setFillColor(sf::Color::Yellow);
    drawCenteredText(title, 200.f);

    sf::Text resume;
    resume.setFont(mFont);
    resume.setString("R - RESUME");
    resume.setCharacterSize(20);
    resume.setFillColor(sf::Color::White);
    drawCenteredText(resume, 310.f);

    sf::Text menu;
    menu.setFont(mFont);
    menu.setString("M - MAIN MENU");
    menu.setCharacterSize(20);
    menu.setFillColor(sf::Color::White);
    drawCenteredText(menu, 370.f);

}

 
// GAME OVER
 

void Game::renderGameOver() {
    sf::Text title;
    title.setFont(mFont);
    title.setString("GAME OVER");
    title.setCharacterSize(48);
    title.setFillColor(sf::Color::Red);
    drawCenteredText(title, 150.f);

    sf::Text s1;
    s1.setFont(mFont);
    s1.setString("P1 FINAL SCORE: " + std::to_string(mScore1));
    s1.setCharacterSize(18);
    s1.setFillColor(sf::Color::Cyan);
    drawCenteredText(s1, 280.f);

    sf::Text s2;
    s2.setFont(mFont);
    s2.setString("P2 FINAL SCORE: " + std::to_string(mScore2));
    s2.setCharacterSize(18);
    s2.setFillColor(sf::Color(100, 255, 150));
    drawCenteredText(s2, 320.f);

    sf::Text level;
    level.setFont(mFont);
    level.setString("LEVEL REACHED: " + std::to_string(mCurrentLevel));
    level.setCharacterSize(18);
    level.setFillColor(sf::Color::White);
    drawCenteredText(level, 380.f);

    sf::Text hint;
    hint.setFont(mFont);
    hint.setString("PRESS ENTER TO RETURN TO MENU");
    hint.setCharacterSize(14);
    hint.setFillColor(sf::Color(150, 150, 150));
    drawCenteredText(hint, 480.f);
}

 
// VICTORY
 

void Game::renderVictory() {
    sf::Text title;
    title.setFont(mFont);
    title.setString("YOU WIN!");
    title.setCharacterSize(48);
    title.setFillColor(sf::Color::Yellow);
    drawCenteredText(title, 150.f);

    sf::Text congrats;
    congrats.setFont(mFont);
    congrats.setString("CONGRATULATIONS!");
    congrats.setCharacterSize(20);
    congrats.setFillColor(sf::Color::White);
    drawCenteredText(congrats, 230.f);

    sf::Text s1;
    s1.setFont(mFont);
    s1.setString("P1 SCORE: " + std::to_string(mScore1));
    s1.setCharacterSize(18);
    s1.setFillColor(sf::Color::Cyan);
    drawCenteredText(s1, 310.f);

    sf::Text s2;
    s2.setFont(mFont);
    s2.setString("P2 SCORE: " + std::to_string(mScore2));
    s2.setCharacterSize(18);
    s2.setFillColor(sf::Color(100, 255, 150));
    drawCenteredText(s2, 350.f);

    sf::Text hint;
    hint.setFont(mFont);
    hint.setString("PRESS ENTER TO RETURN TO MENU");
    hint.setCharacterSize(14);
    hint.setFillColor(sf::Color(150, 150, 150));
    drawCenteredText(hint, 480.f);
}

// LEVEL MANAGEMENT


void Game::loadCurrentLevel() {
    for (int i = 0; i < mEnemyCount; i++) {
        delete mEnemies[i];
        mEnemies[i] = nullptr;
    }
    mEnemyCount = 0;
    mPlatformCount = 0;

    bool success = mLevelManager.loadLevel(
        mCurrentLevel,
        mPlatforms, mPlatformCount,
        mEnemies, mEnemyCount);


    if (!success || mPlatformCount == 0) {
        mPlatforms[0] = Platform(0.f, 615.f, 800.f, 20.f);
        mPlatformCount = 1;
    }

    mPowerUpCount = 0;
    mSnowballPower1 = false;
    mSnowballPower2 = false;
    mDistanceBoost1 = false;
    mDistanceBoost2 = false;

	
}

void Game::checkLevelComplete() {
    if (mEnemyCount == 0 && mPowerUpCount == 0) {

        if (mCurrentLevel >= mLevelManager.getTotalLevels()) {
            mState = VICTORY;
            return;
        }
        mCurrentLevel++;
        mState = LEVEL_COMPLETE;
        
      
    }
}


// COLLISION CHECKS


void Game::checkSnowballEnemyCollision() {
    for (int i = 0; i < mSnowballCount; i++) {
        if (mSnowballs[i].isExpired()) continue;
        for (int j = 0; j < mEnemyCount; j++) {
            if (mSnowballs[i].getBounds().intersects(
                mEnemies[j]->getBounds())) {
               
                // if player has snowball power — instant encase
                bool p1Power = mSnowballs[i].getPlayerIndex() == 0 && mSnowballPower1;
                bool p2Power = mSnowballs[i].getPlayerIndex() == 1 && mSnowballPower2;

                if (p1Power || p2Power)
                    mEnemies[j]->instantEncase();
                else
                    mEnemies[j]->takeDamage();


                mSnowballs[i].setExpired();
                break;
            }
        }
    }
}

void Game::checkPlayerEnemyCollision() {
    for (int i = 0; i < mEnemyCount; i++) {
        if (mEnemies[i]->isRolling()) continue;

        if (mEnemies[i]->isFullyEncased()) {
			if (mPlayer1.isAlive() && 
                mPlayer1.getBounds().intersects(
                    mEnemies[i]->getBounds()) &&
                sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
                mEnemies[i]->startRolling(mPlayer1.isFacingRight(), 0);
                mScore1 += mEnemies[i]->getPoints();
            }
			if (mPlayer2.isAlive() &&
                mPlayer2.getBounds().intersects(
                    mEnemies[i]->getBounds()) &&
                sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
                mEnemies[i]->startRolling(mPlayer2.isFacingRight(), 1);
                mScore2 += mEnemies[i]->getPoints();
            }
            continue;
        }

        if (mEnemies[i]->isPartiallyEncased()) continue;

        if (mPlayer1.isAlive() && !mPlayer1.isRespawning() &&
            !mPlayer1.isInBalloonMode()&&
            mPlayer1.getBounds().intersects(mEnemies[i]->getBounds()))
            mPlayer1.loseLife();

        if (mPlayer2.isAlive() && !mPlayer2.isRespawning() &&
            !mPlayer2.isInBalloonMode()&&
            mPlayer2.getBounds().intersects(mEnemies[i]->getBounds()))
            mPlayer2.loseLife();
    }
}

void Game::checkRollingEnemyCollision() {


    for (int i = 0; i < mEnemyCount; i++) {
        if (!mEnemies[i]->isRolling()) continue;
        if (mEnemies[i]->isDead()) continue;

        for (int j = 0; j < mEnemyCount; j++) {
            if (i == j) continue;
            if (mEnemies[j]->isRolling()) continue;
            if (mEnemies[j]->isDead()) continue;

            if (!mEnemies[i]->getBounds().intersects(
                mEnemies[j]->getBounds())) continue;

            if (mEnemies[j]->isFullyEncased()) {
                mEnemies[j]->startRolling(
                    mEnemies[i]->isRollingRight(),
                    mEnemies[i]->getKickedByPlayer());
                break;
            }
            else {

                // power spawns
                sf::FloatRect bounds = mEnemies[j]->getBounds();
                spawnPowerUp(bounds.left, bounds.top);

                mEnemies[j]->setDead();
                int points = mEnemies[j]->getPoints();
                if (mEnemies[i]->getKickedByPlayer() == 0)
                    mScore1 += points;
                else
                    mScore2 += points;
                break;
            }
        }
    }
}

void Game::checkKnifePlayerCollision() {
    for (int i = 0; i < mEnemyCount; i++) {
        Tornado* tornado = dynamic_cast<Tornado*>(mEnemies[i]);
        if (tornado == nullptr) continue;
        if (!tornado->isKnifeActive()) continue;

        sf::Vector2f p1 = mPlayer1.getPosition();
        sf::Vector2f p2 = mPlayer2.getPosition();
        sf::Vector2f tPos = sf::Vector2f(
            mEnemies[i]->getBounds().left,
            mEnemies[i]->getBounds().top);

        if (mPlayer1.isAlive() && mPlayer2.isAlive()) {
            float dist1 = (p1.x - tPos.x) * (p1.x - tPos.x) +
                (p1.y - tPos.y) * (p1.y - tPos.y);
            float dist2 = (p2.x - tPos.x) * (p2.x - tPos.x) +
                (p2.y - tPos.y) * (p2.y - tPos.y);
            tornado->setNearestPlayerPos(dist1 < dist2 ? p1 : p2);
        }
        else if (mPlayer1.isAlive())
            tornado->setNearestPlayerPos(p1);
        else if (mPlayer2.isAlive())
            tornado->setNearestPlayerPos(p2);

        if (mPlayer1.isAlive() && !mPlayer1.isRespawning() &&
            tornado->getKnifeBounds().intersects(mPlayer1.getBounds()))
            mPlayer1.loseLife();

        if (mPlayer2.isAlive() && !mPlayer2.isRespawning() &&
            tornado->getKnifeBounds().intersects(mPlayer2.getBounds()))
            mPlayer2.loseLife();
    }
}

 
// LOGIN / FILE SYSTEM
 

bool Game::checkLogin(const std::string& user,
    const std::string& pass) {
    std::ifstream file(
        "E:/Semester 2/OOP/LAB/SNOWBROS/SFML_VS_Setup_2026/data/users.txt");
    if (!file.is_open()) return false;

    std::string storedUser, storedPass;
    while (file >> storedUser >> storedPass) {
        if (storedUser == user && storedPass == pass) {
            file.close();
            return true;
        }
    }
    file.close();
    return false;
}

void Game::saveLogin(const std::string& user,
    const std::string& pass) {
    std::ofstream file(
        "E:/Semester 2/OOP/LAB/SNOWBROS/SFML_VS_Setup_2026/data/users.txt",
        std::ios::app); // append mode
    if (file.is_open()) {
        file << user << " " << pass << "\n";
        file.close();
    }
}

 
// DYNAMIC ARRAY HELPERS
 

void Game::addPlatform(Platform p) {
    if (mPlatformCount == mPlatformCapacity) {
        mPlatformCapacity *= 2;
        Platform* temp = new Platform[mPlatformCapacity];
        for (int i = 0; i < mPlatformCount; i++)
            temp[i] = mPlatforms[i];
        delete[] mPlatforms;
        mPlatforms = temp;
    }
    mPlatforms[mPlatformCount++] = p;
}

void Game::addEnemy(Enemy* e) {
    if (mEnemyCount == mEnemyCapacity) {
        mEnemyCapacity *= 2;
        Enemy** temp = new Enemy * [mEnemyCapacity];
        for (int i = 0; i < mEnemyCount; i++)
            temp[i] = mEnemies[i];
        delete[] mEnemies;
        mEnemies = temp;
    }
    mEnemies[mEnemyCount++] = e;
}

void Game::addSnowball(Snowball s) {
    if (mSnowballCount == mSnowballCapacity) {
        mSnowballCapacity *= 2;
        Snowball* temp = new Snowball[mSnowballCapacity];
        for (int i = 0; i < mSnowballCount; i++)
            temp[i] = mSnowballs[i];
        delete[] mSnowballs;
        mSnowballs = temp;
    }
    mSnowballs[mSnowballCount++] = s;
}
// power ups functions
void Game::addPowerUp(PowerUp p) {
    if (mPowerUpCount == mPowerUpCapacity) {
        mPowerUpCapacity *= 2;
        PowerUp* temp = new PowerUp[mPowerUpCapacity];
        for (int i = 0; i < mPowerUpCount; i++)
            temp[i] = mPowerUps[i];
        delete[] mPowerUps;
        mPowerUps = temp;
    }
    mPowerUps[mPowerUpCount++] = p;
}

void Game::spawnPowerUp(float x, float y) {
    // randomly pick power up type
    // gem appears more often than others
    int roll = rand() % 9;
    PowerUpType type;

    if (roll < 4)       type = GEM;
    else if (roll < 5)  type = EXTRA_LIFE;
    else if (roll < 6)  type = SPEED_BOOST;
    else if (roll < 7)  type = SNOWBALL_POWER;
    else if (roll < 8)  type = DISTANCE_BOOST;
    else                type = BALLOON_MODE;

    addPowerUp(PowerUp(x, y, type));
}

void Game::checkPowerUpCollection() {
    for (int i = 0; i < mPowerUpCount; i++) {
        if (mPowerUps[i].isCollected() ||
            mPowerUps[i].isExpired()) continue;

        // check player 1 collision
        if (mPlayer1.isAlive() &&
            mPlayer1.getBounds().intersects(
                mPowerUps[i].getBounds())) {
            mPowerUps[i].collect();
            applyPowerUp(mPowerUps[i].getType(), 1);
        }

        // check player 2 collision
        else if (mPlayer2.isAlive() &&
            mPlayer2.getBounds().intersects(
                mPowerUps[i].getBounds())) {
            mPowerUps[i].collect();
            applyPowerUp(mPowerUps[i].getType(), 2);
        }
    }

    // remove collected or expired power ups
    for (int i = mPowerUpCount - 1; i >= 0; i--) {
        if (mPowerUps[i].isCollected() ||
            mPowerUps[i].isExpired()) {
            for (int k = i; k < mPowerUpCount - 1; k++)
                mPowerUps[k] = mPowerUps[k + 1];
            mPowerUpCount--;
        }
    }
}

void Game::applyPowerUp(PowerUpType type, int player) {
    if (player == 1) {
        switch (type) {
        case GEM:
            mGemCount1 += 10;
         
            break;
        case SPEED_BOOST:
            mSpeedBoostTimer1 = 15.f;
            mPlayer1.setSpeedBoost(true);
            break;
        case SNOWBALL_POWER:
            mSnowballPower1 = true;
            break;
        case DISTANCE_BOOST:
            mDistanceBoost1 = true;
            break;
        case BALLOON_MODE:
            mBalloonTimer1 = 10.f;
            mPlayer1.setBalloonMode(true);
            break;
        case EXTRA_LIFE:
            mPlayer1.addLife();
            break;
        }
    }
    else {
        switch (type) {
        case GEM:
            mGemCount2 += 10;
         
            break;
        case SPEED_BOOST:
            mSpeedBoostTimer2 = 15.f;
            mPlayer2.setSpeedBoost(true);
            break;
        case SNOWBALL_POWER:
            mSnowballPower2 = true;
            break;
        case DISTANCE_BOOST:
            mDistanceBoost2 = true;
            break;
        case BALLOON_MODE:
            mBalloonTimer2 = 10.f;
            mPlayer2.setBalloonMode(true);
            break;
        case EXTRA_LIFE:
            mPlayer2.addLife();
            break;
        }
    }
}

void Game::updatePowerUpEffects(float deltaTime) {
    // count down speed boost timers
    if (mSpeedBoostTimer1 > 0) {
        mSpeedBoostTimer1 -= deltaTime;
        if (mSpeedBoostTimer1 <= 0) {
            mSpeedBoostTimer1 = 0.f;
            mPlayer1.setSpeedBoost(false);
        }
    }
    if (mSpeedBoostTimer2 > 0) {
        mSpeedBoostTimer2 -= deltaTime;
        if (mSpeedBoostTimer2 <= 0) {
            mSpeedBoostTimer2 = 0.f;
            mPlayer2.setSpeedBoost(false);
        }
    }

    // count down balloon timers
    if (mBalloonTimer1 > 0) {
        mBalloonTimer1 -= deltaTime;
        if (mBalloonTimer1 <= 0) {
            mBalloonTimer1 = 0.f;
            mPlayer1.setBalloonMode(false);
        }
    }
    if (mBalloonTimer2 > 0) {
        mBalloonTimer2 -= deltaTime;
        if (mBalloonTimer2 <= 0) {
            mBalloonTimer2 = 0.f;
            mPlayer2.setBalloonMode(false);
        }
    }
}