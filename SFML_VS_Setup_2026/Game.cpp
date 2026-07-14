#include "Game.h"
#include "AccountManager.h"




//  SHARED DRAW HELPERS  (login / register screens)


static void drawPanel(sf::RenderWindow& w,
    float x, float y, float width, float height,
    sf::Color fill = sf::Color(25, 28, 60),
    sf::Color outline = sf::Color(80, 120, 220))
{
    sf::RectangleShape panel(sf::Vector2f(width, height));
    panel.setPosition(x, y);
    panel.setFillColor(fill);
    panel.setOutlineColor(outline);
    panel.setOutlineThickness(2.f);
    w.draw(panel);
}

static void drawInputBox(sf::RenderWindow& w, sf::Font& font,
    const std::string& label, const std::string& content,
    float x, float y, float boxW, bool active, bool masked = false)
{
    sf::Text lbl;
    lbl.setFont(font);
    lbl.setString(label);
    lbl.setCharacterSize(24);
    lbl.setFillColor(active ? sf::Color(0, 80, 80) : sf::Color(60, 60, 80));
    lbl.setPosition(x, y-14.f);
    w.draw(lbl);

    sf::RectangleShape box(sf::Vector2f(boxW, 38.f));
    box.setPosition(x, y + 20.f);
    box.setFillColor(active ? sf::Color(40, 45, 90) : sf::Color(25, 28, 60));
    box.setOutlineColor(active ? sf::Color::Cyan : sf::Color(80, 80, 120));
    box.setOutlineThickness(2.f);
    w.draw(box);

    std::string display = masked
        ? std::string(content.size(), '*')
        : content;
    if (active) display += "_";

    sf::Text txt;
    txt.setFont(font);
    txt.setString(display);
    txt.setCharacterSize(24);
    txt.setFillColor(sf::Color::White);
    txt.setPosition(x + 8.f, y + 27.f);
    w.draw(txt);
}


//  CONSTRUCTOR


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
    mState(CHARACTER_SELECT), // Changescreen
    mLoginPlayerTurn(1),
    mTypingConfirm(false),
    mTypingUsername(true),
    mLoginMessage(""),
    mLoggedInUser(""),
    mLoginSuccess(false),
    mLoginState(LoginState::TYPING_USER),
    mP1CharIndex(0),
    mP2CharIndex(1),
    mP1Selected(false),
    mP2Selected(false),
    mMenuSelection(0),

    mPowerUpCount(0), mPowerUpCapacity(20),
    mSpeedBoostTimer1(0.f), mSpeedBoostTimer2(0.f),
    mBalloonTimer1(0.f), mBalloonTimer2(0.f),
    mSnowballPower1(false), mSnowballPower2(false),
    mDistanceBoost1(false), mDistanceBoost2(false),
    mGemCount1(0), mGemCount2(0), mBgLoaded(false),

    //boss
    mBossDefeated(false),
    mBossGemReward(0),


    mLeaderboardKeyHeld(false),
    mScoreSaved(false),
    mShopReturnState(PAUSED),
    mShopPlayerTurn(1)

{
    mPlatforms = new Platform[mPlatformCapacity];
    mEnemies = new Enemy * [mEnemyCapacity];
    mSnowballs = new Snowball[mSnowballCapacity];
    mPowerUps = new PowerUp[mPowerUpCapacity];

    mWindow.setFramerateLimit(60);

    mFont.loadFromFile("assets/Fonts/ps2.ttf");
    mHUD.loadFont("assets/Fonts/ps2.ttf");
    mLeaderboard.setFont(mFont);

    PowerUp::preloadAll();//load powerups

    // first image

    // background image for splash screen
    if (mSplashBgTexture.loadFromFile("assets/Images/startscreen.png")) {
        mSplashBgTexture.setSmooth(true);
        mSplashBgSprite.setTexture(mSplashBgTexture);
        mSplashBgSprite.setScale(
            800.f / mSplashBgTexture.getSize().x,
            660.f / mSplashBgTexture.getSize().y
        );
    }




    // background image in login
    if (mLoginBgTexture.loadFromFile("assets/Images/login.png")) {
        mLoginBgTexture.setSmooth(true);
        mLoginBgSprite.setTexture(mLoginBgTexture);
        mLoginBgSprite.setScale(
            800.f / mLoginBgTexture.getSize().x,
            660.f / mLoginBgTexture.getSize().y
        );
    }

    if (mMenuBgTexture.loadFromFile("assets/Images/menu.png")) {
        mMenuBgTexture.setSmooth(true);
        mMenuBgSprite.setTexture(mMenuBgTexture);
        mMenuBgSprite.setScale(
            800.f / mMenuBgTexture.getSize().x,
            660.f / mMenuBgTexture.getSize().y
        );
    }

    if (mLeaderboardBgTexture.loadFromFile("assets/Images/leaderboard.png")) {
        mLeaderboardBgTexture.setSmooth(true);
        mLeaderboardBgSprite.setTexture(mLeaderboardBgTexture);
        mLeaderboardBgSprite.setScale(
            800.f / mLeaderboardBgTexture.getSize().x,
            660.f / mLeaderboardBgTexture.getSize().y
        );
    }

    if (mShopBgTexture.loadFromFile("assets/Images/leaderboard.png")) {
        mShopBgTexture.setSmooth(true);
        mShopBgSprite.setTexture(mShopBgTexture);
        mShopBgSprite.setScale(
            800.f / mShopBgTexture.getSize().x,
            660.f / mShopBgTexture.getSize().y
        );
    }

    if (mCharSelectBgTexture.loadFromFile("assets/Images/character select.png")) {
        mCharSelectBgTexture.setSmooth(true);
        mCharSelectBgSprite.setTexture(mCharSelectBgTexture);
        mCharSelectBgSprite.setScale(
            800.f / mCharSelectBgTexture.getSize().x,
            660.f / mCharSelectBgTexture.getSize().y
        );
    }

    if (mGameOverBgTexture.loadFromFile("assets/Images/gameover.png")) {
        mGameOverBgTexture.setSmooth(true);
        mGameOverBgSprite.setTexture(mGameOverBgTexture);
        mGameOverBgSprite.setScale(
            800.f / mGameOverBgTexture.getSize().x,
            660.f / mGameOverBgTexture.getSize().y
        );
    }


    mCharacters[0] = { "Nick", sf::Color(100, 180, 255) };
    mCharacters[1] = { "Tom",  sf::Color(100, 255, 150) };
    mCharacters[2] = { "Rex",  sf::Color(220, 60, 60) };

    // load character select images — sprite textures (Tier 1 integration)

    if (mCharTextures[0].loadFromFile("assets/Images/blue.png")) {
        mCharTextures[0].setSmooth(true);
        mCharSprites[0].setTexture(mCharTextures[0]);
    }
    if (mCharTextures[1].loadFromFile("assets/Images/green.png")) {
        mCharTextures[1].setSmooth(true);
        mCharSprites[1].setTexture(mCharTextures[1]);
    }
    if (mCharTextures[2].loadFromFile("assets/Images/red.png")) {
        mCharTextures[2].setSmooth(true);
        mCharSprites[2].setTexture(mCharTextures[2]);
    }

    // background images for gameplay levels
    if (mBgTextures[0].loadFromFile("assets/Images/img1.jpg")) {
        mBgTextures[0].setSmooth(true);
        mBgSprites[0].setTexture(mBgTextures[0]);
        mBgSprites[0].setScale(
            800.f / mBgTextures[0].getSize().x,
            660.f / mBgTextures[0].getSize().y
        );
    }
    if (mBgTextures[1].loadFromFile("assets/Images/img2.jpg")) {
        mBgTextures[1].setSmooth(true);
        mBgSprites[1].setTexture(mBgTextures[1]);
        mBgSprites[1].setScale(
            800.f / mBgTextures[1].getSize().x,
            660.f / mBgTextures[1].getSize().y
        );
    }
    if (mBgTextures[2].loadFromFile("assets/Images/img3.jpg")) {
        mBgTextures[2].setSmooth(true);
        mBgSprites[2].setTexture(mBgTextures[2]);
        mBgSprites[2].setScale(
            800.f / mBgTextures[2].getSize().x,
            660.f / mBgTextures[2].getSize().y
        );
    }
    if (mBgTextures[3].loadFromFile("assets/Images/img4.jpg")) {
        mBgTextures[3].setSmooth(true);
        mBgSprites[3].setTexture(mBgTextures[3]);
        mBgSprites[3].setScale(
            800.f / mBgTextures[3].getSize().x,
            660.f / mBgTextures[3].getSize().y
        );
    }
    mBgLoaded = true;


}


//  DESTRUCTOR


Game::~Game() {
    for (int i = 0; i < mEnemyCount; i++)
        delete mEnemies[i];
    delete[] mEnemies;
    delete[] mPlatforms;
    delete[] mSnowballs;
    delete[]mPowerUps;
}


//  MAIN LOOP


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


//  PROCESS EVENTS


void Game::processEvents() {
    sf::Event event;
    while (mWindow.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            mWindow.close();

        // SPLASH -> LOGIN via ENTER (latest control scheme)
        if (mState == SPLASH && event.type == sf::Event::KeyPressed)
            if (event.key.code == sf::Keyboard::Enter)
                mState = LOGIN;

        // Route all login / register input first
        if (mState == LOGIN)    handleLoginEvent(event);
        if (mState == REGISTER) handleRegisterEvent(event);

        if (event.type == sf::Event::KeyPressed) {

            // Escape: pause / unpause
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


            // Hitbox toggle during gameplay
            if (mState == PLAYING &&
                event.key.code == sf::Keyboard::H)
                mShowHitboxes = !mShowHitboxes;

            // Main menu navigation
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
                        mCurrentLevel = 5; // changelevel
                        mScore1 = 0;
                        mScore2 = 0;
                        mScoreSaved = false;
                        mGameOver = false;
                        mPlayer1.resetLives();
                        mPlayer2.resetLives();
                        mLevelManager.selectBonusLevels();
                        loadCurrentLevel();
                        mState = PLAYING;
                    }
                    else if (mMenuSelection == 1) {
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

            // Character select navigation
            if (mState == CHARACTER_SELECT) {
                if (!mP1Selected) {
                    if (event.key.code == sf::Keyboard::Left)  mP1CharIndex--;
                    if (event.key.code == sf::Keyboard::Right) mP1CharIndex++;
                    if (mP1CharIndex < 0) mP1CharIndex = 2;
                    if (mP1CharIndex > 2) mP1CharIndex = 0;
                    if (event.key.code == sf::Keyboard::Down)
                        mP1Selected = true;
                }
                if (!mP2Selected) {
                    if (event.key.code == sf::Keyboard::A) mP2CharIndex--;
                    if (event.key.code == sf::Keyboard::D) mP2CharIndex++;
                    if (mP2CharIndex < 0) mP2CharIndex = 2;
                    if (mP2CharIndex > 2) mP2CharIndex = 0;
                    if (event.key.code == sf::Keyboard::S)
                        mP2Selected = true;
                }
                if (mP1Selected && mP2Selected) {
                    // hook up chosen sprite to each player (Tier 1 sprite integration)
                    mPlayer1.setCharacterSprite(mP1CharIndex);
                    mPlayer2.setCharacterSprite(mP2CharIndex);
                    mState = MAIN_MENU;
                }
            }

            // level complete — press enter to continue
            if (mState == LEVEL_COMPLETE &&
                event.key.code == sf::Keyboard::Return) {
                loadCurrentLevel();
                mState = PLAYING;
            }

            // Pause menu
            if (mState == PAUSED) {
                if (event.key.code == sf::Keyboard::R)
                    mState = PLAYING;
                if (event.key.code == sf::Keyboard::M)
                    mState = MAIN_MENU;
                if (event.key.code == sf::Keyboard::O) {
                    mShopPlayerTurn = 1;
                    mShop1.setSelectedIndex(0);
                    mShop2.setSelectedIndex(0);
                    mShopReturnState = PAUSED;
                    mState = SHOP_SCREEN;
                }
            }

            if (mState == SHOP_SCREEN) {
                Shop& activeShop = (mShopPlayerTurn == 1) ? mShop1 : mShop2;

                if (event.key.code == sf::Keyboard::Escape)
                    mState = mShopReturnState;

                // Tab switches between P1 and P2 shop
                if (event.key.code == sf::Keyboard::Tab)
                    mShopPlayerTurn = (mShopPlayerTurn == 1) ? 2 : 1;

                if (event.key.code == sf::Keyboard::Up) {
                    int idx = activeShop.getSelectedIndex() - 1;
                    if (idx < 0) idx = ITEM_COUNT - 1;
                    activeShop.setSelectedIndex(idx);
                }
                if (event.key.code == sf::Keyboard::Down) {
                    int idx = activeShop.getSelectedIndex() + 1;
                    if (idx >= ITEM_COUNT) idx = 0;
                    activeShop.setSelectedIndex(idx);
                }
                if (event.key.code == sf::Keyboard::Return) {
                    ShopItem item = (ShopItem)activeShop.getSelectedIndex();
                    if (activeShop.purchase(item)) {
                        applyShopPurchase(item, mShopPlayerTurn);
                        mGemCount1 = mShop1.getGems();
                        mGemCount2 = mShop2.getGems();
                    }
                    if (mShopPlayerTurn == 1)
                        mShop1.saveForPlayer(mP1Username);
                    else
                        mShop2.saveForPlayer(mP2Username);
                }
            }

            // Game over — press enter to go to menu
            if (mState == GAME_OVER &&
                event.key.code == sf::Keyboard::Return)
                mState = MAIN_MENU;

            // Victory — press enter to go to menu
            if (mState == VICTORY &&
                event.key.code == sf::Keyboard::Return)
                mState = MAIN_MENU;
        }
    }
}


//  UPDATE


void Game::update(float deltaTime) {
    switch (mState) {
    case SPLASH:                                          break;
    case LOGIN:              updateLogin();               break;
    case REGISTER:           updateRegister();            break;
    case CHARACTER_SELECT:   updateCharSelect();          break;
    case MAIN_MENU:          updateMainMenu();            break;
    case PLAYING:            updatePlaying(deltaTime);    break;
        // case LEVEL_COMPLETE:     updateLevelComplete(deltaTime); break;
    case PAUSED:             updatePaused();              break;
    case GAME_OVER:          updateGameOver();            break;
    case VICTORY:            updateVictory();             break;
    case LEADERBOARD_SCREEN: updateLeaderboard(deltaTime);  break;
    case SHOP_SCREEN:        updateShop(deltaTime);         break;
    }
}

void Game::updateLogin() {
    // handled in handleLoginEvent
}

void Game::updateRegister() {
    // handled in handleRegisterEvent
}

void Game::updateCharSelect() {
    // handled in processEvents
}

void Game::updateMainMenu() {
    // handled in processEvents
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
        float snowX = mPlayer2.isFacingRight() ? pos.x + 40.f : pos.x - 20.f;
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
    {
        mEnemies[i]->update(deltaTime, mPlatforms, mPlatformCount);

        // Mogera — collect spawned children
        Mogera* mogera = dynamic_cast<Mogera*>(mEnemies[i]);
        if (mogera != nullptr && mogera->hasNewChild()) {
            addEnemy(mogera->takeChild());
        }

        // Gamakichi — set player position for aiming
        Gamakichi* gama = dynamic_cast<Gamakichi*>(mEnemies[i]);
        if (gama != nullptr) {
            sf::Vector2f p1 = mPlayer1.getPosition();
            sf::Vector2f p2 = mPlayer2.getPosition();
            sf::Vector2f bPos = mEnemies[i]->getBounds().left > 0 ?
                sf::Vector2f(mEnemies[i]->getBounds().left,
                    mEnemies[i]->getBounds().top) :
                sf::Vector2f(400.f, 300.f);

            float d1 = (p1.x - bPos.x) * (p1.x - bPos.x) +
                (p1.y - bPos.y) * (p1.y - bPos.y);
            float d2 = (p2.x - bPos.x) * (p2.x - bPos.x) +
                (p2.y - bPos.y) * (p2.y - bPos.y);
            gama->setPlayerPos(d1 < d2 ? p1 : p2);

            // check rocket hits on players
            if (mPlayer1.isAlive() && !mPlayer1.isRespawning())
                if (gama->checkRocketHit(mPlayer1.getBounds()))
                    mPlayer1.loseLife();

            if (mPlayer2.isAlive() && !mPlayer2.isRespawning())
                if (gama->checkRocketHit(mPlayer2.getBounds()))
                    mPlayer2.loseLife();
        }
    }



    // collisions
    checkSnowballEnemyCollision();
    checkPlayerEnemyCollision();
    checkRollingEnemyCollision();
    checkKnifePlayerCollision();

    // remove dead enemies
    for (int i = mEnemyCount - 1; i >= 0; i--) {
        if (mEnemies[i]->isDead()) {

            // if this was a boss — award score and spawn gem drops
            Boss* boss = dynamic_cast<Boss*>(mEnemies[i]);
            if (boss != nullptr) {
                mScore1 += boss->getPoints() / 2;
                mScore2 += boss->getPoints() / 2;

                sf::FloatRect b = mEnemies[i]->getBounds();
                for (int g = 0; g < 8; g++) {
                    float dropX = b.left + (rand() % (int)b.width);
                    float dropY = b.top + (rand() % (int)b.height);
                    addPowerUp(PowerUp(dropX, dropY, GEM));
                }
            }

            delete mEnemies[i];
            for (int k = i; k < mEnemyCount - 1; k++)
                mEnemies[k] = mEnemies[k + 1];
            mEnemyCount--;
        }
    }

    mHUD.update(mScore1, mPlayer1.getLives(),
        mScore2, mPlayer2.getLives(),
        mCurrentLevel, mGemCount1, mGemCount2);


    checkLevelComplete();

    if (!mPlayer1.isAlive() && !mPlayer2.isAlive())
        mGameOver = true;
}



void Game::updatePaused() {
    // handled in processEvents
}

void Game::updateGameOver() {
    if (!mScoreSaved) {
        saveScoreToLeaderboard();
        saveShopsForPlayers();
        mScoreSaved = true;
    }
}

void Game::updateVictory() {
    if (!mScoreSaved) {
        saveScoreToLeaderboard();
        saveShopsForPlayers();
        mScoreSaved = true;
    }
}


//  RENDER


void Game::render() {

    mWindow.clear(sf::Color(20, 20, 40));

    switch (mState) {
    case SPLASH:           renderSplash();       break;
    case LOGIN:            renderLogin();        break;
    case REGISTER:         renderRegister();     break;
    case CHARACTER_SELECT: renderCharSelect();   break;
    case MAIN_MENU:        renderMainMenu();     break;
    case PLAYING:          renderPlaying();      break;
    case LEVEL_COMPLETE:   renderLevelComplete(); break;
    case PAUSED:           renderPaused();       break;
    case GAME_OVER:        renderGameOver();     break;
    case VICTORY:          renderVictory();      break;
    case LEADERBOARD_SCREEN: renderLeaderboard(); break;
    case SHOP_SCREEN:      renderShop();         break;
    }

    mWindow.display();
}


//  LEADERBOARD


void Game::saveScoreToLeaderboard() {
    if (!mP1Username.empty())
        mLeaderboard.addEntry(mP1Username, mScore1, mCurrentLevel);
    if (!mP2Username.empty())
        mLeaderboard.addEntry(mP2Username, mScore2, mCurrentLevel);
}

void Game::updateLeaderboard(float deltaTime) {
    if (mLeaderboardKeyHeld) {
        if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Escape) &&
            !sf::Keyboard::isKeyPressed(sf::Keyboard::BackSpace))
            mLeaderboardKeyHeld = false;
        return;
    }

    bool goBack = mLeaderboard.draw(mWindow, deltaTime);
    if (goBack) {
        mLeaderboardKeyHeld = true;
        mState = MAIN_MENU;
    }
}

void Game::renderLeaderboard() {
    mWindow.draw(mLeaderboardBgSprite);  // background first
    mLeaderboard.draw(mWindow, 0);       // leaderboard draws on top
}


//  HELPER — centered text


void Game::drawCenteredText(sf::Text& text, float y) {
    float x = (800.f - text.getGlobalBounds().width) / 2.f;
    text.setPosition(x, y);
    mWindow.draw(text);
}


//======================================================
// FIRST SPLASH SCREEN
//======================================================


void Game::renderSplash() {
    mWindow.draw(mSplashBgSprite);
}


//  LOGIN SCREEN  


void Game::renderLogin() {

    // Background image fills the whole window
    mWindow.draw(mLoginBgSprite);

    std::string tag = (mLoginPlayerTurn == 1) ? "PLAYER 1 LOGIN" : "PLAYER 2 LOGIN";
    sf::Color tagColor = (mLoginPlayerTurn == 1)
        ? sf::Color(100, 200, 255)
        : sf::Color(100, 255, 160);

    sf::RectangleShape tagBar(sf::Vector2f(300.f, 45.f));
    tagBar.setPosition(250.f, 120.f);
    tagBar.setFillColor(mLoginPlayerTurn == 1
        ? sf::Color(0, 60, 120, 180)
        : sf::Color(0, 100, 60, 180));
    tagBar.setOutlineColor(tagColor);
    tagBar.setOutlineThickness(1.5f);
    mWindow.draw(tagBar);

    sf::Text tagText;
    tagText.setFont(mFont);
    tagText.setString(tag);
    tagText.setCharacterSize(28);
    tagText.setFillColor(tagColor);
    drawCenteredText(tagText, 126.f);  

    

    // ── Input fields ─────────────────────────────────────────────
  
    bool typingUser = (mLoginState == LoginState::TYPING_USER);
    bool typingPass = (mLoginState == LoginState::TYPING_PASS);

    drawInputBox(mWindow, mFont, "USERNAME",
        mUsernameInput, 180.f, 230.f, 440.f, typingUser);
    drawInputBox(mWindow, mFont, "PASSWORD",
        mPasswordInput, 180.f, 335.f, 440.f, typingPass, true);

    // ── Hint text ────────────────────────────────────────────────
      sf::Text hint;
    hint.setFont(mFont);
    hint.setString("ENTER: confirm / login      TAB: switch field");
    hint.setCharacterSize(20);
    hint.setFillColor(sf::Color(255, 255, 255));
    drawCenteredText(hint, 440.f);

    // ── Create account button ─────────────────────────────────────

    drawPanel(mWindow, 180.f, 515.f, 440.f, 50.f,
        sf::Color(20, 50, 30), sf::Color(60, 180, 80));

    sf::Text newAccLabel;
    newAccLabel.setFont(mFont);
    newAccLabel.setString("NO ACCOUNT?");
    newAccLabel.setCharacterSize(18);
    newAccLabel.setFillColor(sf::Color(160, 220, 160));
    drawCenteredText(newAccLabel, 488.f);  

    sf::Text newAccBtn;
    newAccBtn.setFont(mFont);
    newAccBtn.setString("[ C ]  CREATE NEW ACCOUNT");
    newAccBtn.setCharacterSize(20);
    newAccBtn.setFillColor(sf::Color(100, 255, 120));
    drawCenteredText(newAccBtn, 525.f);     


    // ── Message box ───────────────────────────────────────────────
   
    if (!mLoginMessage.empty()) {
        sf::Color boxFill = mLoginSuccess ? sf::Color(0, 60, 20) : sf::Color(70, 0, 0);
        sf::Color boxLine = mLoginSuccess ? sf::Color(60, 200, 80) : sf::Color(220, 50, 50);
        sf::Color txtColor = mLoginSuccess ? sf::Color(100, 255, 130) : sf::Color(255, 80, 80);

        drawPanel(mWindow, 100.f, 600.f, 600.f, 40.f, boxFill, boxLine);

        sf::Text msg;
        msg.setFont(mFont);
        msg.setString(mLoginMessage);
        msg.setCharacterSize(20);
        msg.setFillColor(txtColor);
        drawCenteredText(msg, 608.f);      
    }
}


//  REGISTER SCREEN/CreateAcc


void Game::renderRegister() {

    // Background image fills the whole window
    mWindow.draw(mLoginBgSprite);

    // ── Title ────────────────────────────────────────────────────
   
    sf::Text title;
    title.setFont(mFont);
    title.setString("CREATE ACCOUNT");
    title.setCharacterSize(56);
    title.setFillColor(sf::Color::White);
    drawCenteredText(title, 90.f);

    // ── Player tag bar ───────────────────────────────────────────

    std::string tag = (mLoginPlayerTurn == 1)
        ? "SETTING UP PLAYER 1" : "SETTING UP PLAYER 2";
    sf::Color tagColor = (mLoginPlayerTurn == 1)
        ? sf::Color(100, 200, 255) : sf::Color(100, 255, 160);

    sf::RectangleShape tagBar(sf::Vector2f(340.f, 40.f));
    tagBar.setPosition(230.f, 182.f);
    tagBar.setFillColor(sf::Color(75, 0, 130));
    tagBar.setOutlineColor(tagColor);
    tagBar.setOutlineThickness(1.5f);
    mWindow.draw(tagBar);

    sf::Text tagTxt;
    tagTxt.setFont(mFont);
    tagTxt.setString(tag);
    tagTxt.setCharacterSize(24);
    tagTxt.setFillColor(tagColor);
    drawCenteredText(tagTxt, 187.f);        



    // ── Input fields ─────────────────────────────────────────────

    bool onUser = (mLoginState == LoginState::TYPING_USER && !mTypingConfirm);
    bool onPass = (mLoginState == LoginState::TYPING_PASS && !mTypingConfirm);
    bool onConfirm = mTypingConfirm;

    drawInputBox(mWindow, mFont, "CHOOSE USERNAME",
        mUsernameInput, 180.f, 262.f, 440.f, onUser);
    drawInputBox(mWindow, mFont, "CHOOSE PASSWORD",
        mPasswordInput, 180.f, 345.f, 440.f, onPass, true);
    drawInputBox(mWindow, mFont, "CONFIRM PASSWORD",
        mConfirmPassInput, 180.f, 428.f, 440.f, onConfirm, true);

    // ── Hint text ────────────────────────────────────────────────
 
    sf::Text hint;
    hint.setFont(mFont);
    hint.setString("ENTER: Next     BACKSPACE: Delete");
    hint.setCharacterSize(20);
    hint.setFillColor(sf::Color::White);
    drawCenteredText(hint, 518.f);

    // ── Back button ───────────────────────────────────────────────
    
    sf::Text backBtn;
    backBtn.setFont(mFont);
    backBtn.setString("[ESCAPE]: Back");
    backBtn.setCharacterSize(20);
    backBtn.setFillColor(sf::Color(255, 255, 255));
    backBtn.setPosition(10.f, 10.f);      
    mWindow.draw(backBtn);



    // ── Message box ───────────────────────────────────────────────
   
    if (!mLoginMessage.empty()) {
        sf::Color boxFill = mLoginSuccess ? sf::Color(0, 60, 20) : sf::Color(70, 0, 0);
        sf::Color boxLine = mLoginSuccess ? sf::Color(60, 200, 80) : sf::Color(220, 50, 50);
        sf::Color txtColor = mLoginSuccess ? sf::Color(100, 255, 130) : sf::Color(255, 80, 80);

        drawPanel(mWindow, 180.f, 588.f, 440.f, 40.f, boxFill, boxLine);

        sf::Text msg;
        msg.setFont(mFont);
        msg.setString(mLoginMessage);
        msg.setCharacterSize(20);
        msg.setFillColor(txtColor);
        drawCenteredText(msg, 599.f);       
    }
}


//  handleLoginEvent


void Game::handleLoginEvent(const sf::Event& event) {

    if (event.type == sf::Event::TextEntered) {
        unsigned int u = event.text.unicode;
        if (u == '\b') {
            if (mLoginState == LoginState::TYPING_USER && !mUsernameInput.empty())
                mUsernameInput.pop_back();
            else if (mLoginState == LoginState::TYPING_PASS && !mPasswordInput.empty())
                mPasswordInput.pop_back();
            mLoginMessage.clear();
        }
        else if (u >= 32 && u < 127) {
            char c = static_cast<char>(u);
            if (mLoginState == LoginState::TYPING_USER)
                mUsernameInput += c;
            else if (mLoginState == LoginState::TYPING_PASS)
                mPasswordInput += c;
        }
    }

    if (event.type == sf::Event::KeyPressed) {

        // C — go to register screen
        if (event.key.code == sf::Keyboard::C) {
            mUsernameInput.clear();
            mPasswordInput.clear();
            mConfirmPassInput.clear();
            mLoginMessage.clear();
            mLoginState = LoginState::TYPING_USER;
            mTypingConfirm = false;
            mState = REGISTER;
            return;
        }

        // Tab — switch field
        if (event.key.code == sf::Keyboard::Tab) {
            mLoginState = (mLoginState == LoginState::TYPING_USER)
                ? LoginState::TYPING_PASS : LoginState::TYPING_USER;
            mLoginMessage.clear();
        }

        // Enter — advance or attempt login
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
                    mLoginSuccess = true;

                    if (mLoginPlayerTurn == 1) {
                        mP1Username = acc.username;
                        mP1Account = acc;
                        mLoginMessage = "WELCOME " + acc.username + "!  P2, YOUR TURN...";
                        mUsernameInput.clear();
                        mPasswordInput.clear();
                        mLoginState = LoginState::TYPING_USER;
                        mLoginPlayerTurn = 2;
                    }
                    else {
                        if (acc.username == mP1Username) {
                            mLoginSuccess = false;
                            mLoginMessage = "P2 MUST USE A DIFFERENT ACCOUNT THAN P1!";
                            mPasswordInput.clear();
                            mLoginState = LoginState::TYPING_USER;
                            mUsernameInput.clear();
                            return;
                        }
                        mP2Username = acc.username;
                        mP2Account = acc;
                        mLoggedInUser = mP1Username;
                        mLoginMessage = "WELCOME " + acc.username + "!";
                        mLoginState = LoginState::DONE;
                        mShop1.loadForPlayer(mP1Username);
                        mShop2.loadForPlayer(mP2Username);
                        mGemCount1 = mShop1.getGems();
                        mGemCount2 = mShop2.getGems();

                        mState = CHARACTER_SELECT;
                    }
                }
                else if (result == Account_Manager::Result::USER_NOT_FOUND) {
                    mLoginSuccess = false;
                    mLoginMessage = "ACCOUNT NOT FOUND. PRESS [C] TO CREATE ONE.";
                    mPasswordInput.clear();
                    mLoginState = LoginState::TYPING_USER;
                }
                else if (result == Account_Manager::Result::WRONG_PASSWORD) {
                    mLoginSuccess = false;
                    mLoginMessage = "WRONG PASSWORD. TRY AGAIN.";
                    mPasswordInput.clear();
                }
                else if (result == Account_Manager::Result::EMPTY_FIELD) {
                    mLoginSuccess = false;
                    mLoginMessage = "ENTER USERNAME AND PASSWORD.";
                }
            }
        }
    }
}


//  handleRegisterEvent


void Game::handleRegisterEvent(const sf::Event& event) {

    if (event.type == sf::Event::TextEntered) {
        unsigned int u = event.text.unicode;
        if (u == '\b') {
            if (!mTypingConfirm) {
                if (mLoginState == LoginState::TYPING_USER && !mUsernameInput.empty())
                    mUsernameInput.pop_back();
                else if (mLoginState == LoginState::TYPING_PASS && !mPasswordInput.empty())
                    mPasswordInput.pop_back();
            }
            else {
                if (!mConfirmPassInput.empty())
                    mConfirmPassInput.pop_back();
            }
            mLoginMessage.clear();
        }
        else if (u >= 32 && u < 127) {
            char c = static_cast<char>(u);
            if (!mTypingConfirm) {
                if (mLoginState == LoginState::TYPING_USER)
                    mUsernameInput += c;
                else if (mLoginState == LoginState::TYPING_PASS)
                    mPasswordInput += c;
            }
            else {
                mConfirmPassInput += c;
            }
        }
    }

    if (event.type == sf::Event::KeyPressed) {

        // Escape — back to login
        if (event.key.code == sf::Keyboard::Escape) {
            mUsernameInput.clear();
            mPasswordInput.clear();
            mConfirmPassInput.clear();
            mLoginMessage.clear();
            mLoginState = LoginState::TYPING_USER;
            mTypingConfirm = false;
            mState = LOGIN;
            return;
        }

        // Enter — advance fields or submit
        if (event.key.code == sf::Keyboard::Enter) {

            if (mLoginState == LoginState::TYPING_USER && !mTypingConfirm) {
                if (mUsernameInput.empty()) {
                    mLoginSuccess = false;
                    mLoginMessage = "USERNAME CANNOT BE EMPTY.";
                    return;
                }
                mLoginState = LoginState::TYPING_PASS;
                mLoginMessage.clear();
            }
            else if (mLoginState == LoginState::TYPING_PASS && !mTypingConfirm) {
                if (mPasswordInput.empty()) {
                    mLoginSuccess = false;
                    mLoginMessage = "PASSWORD CANNOT BE EMPTY.";
                    return;
                }
                mTypingConfirm = true;
                mLoginMessage.clear();
            }
            else if (mTypingConfirm) {

                if (mConfirmPassInput != mPasswordInput) {
                    mLoginSuccess = false;
                    mLoginMessage = "PASSWORDS DO NOT MATCH. TRY AGAIN.";
                    mConfirmPassInput.clear();
                    return;
                }

                Account_Manager::Result reg =
                    mAccountManager.registerUser(mUsernameInput, mPasswordInput);

                if (reg == Account_Manager::Result::OK) {
                    mLoginSuccess = true;
                    mLoginMessage = "ACCOUNT CREATED! LOGGING YOU IN...";

                    PlayerAccount acc;
                    mAccountManager.tryLogin(mUsernameInput, mPasswordInput, acc);

                    if (mLoginPlayerTurn == 1) {
                        mP1Username = acc.username;
                        mP1Account = acc;
                        mUsernameInput.clear();
                        mPasswordInput.clear();
                        mConfirmPassInput.clear();
                        mLoginState = LoginState::TYPING_USER;
                        mTypingConfirm = false;
                        mLoginPlayerTurn = 2;
                        mState = LOGIN;
                    }
                    else {
                        if (acc.username == mP1Username) {
                            mLoginSuccess = false;
                            mLoginMessage = "P2 MUST USE A DIFFERENT ACCOUNT THAN P1!";
                            mLoginState = LoginState::TYPING_USER;
                            mUsernameInput.clear();
                            mPasswordInput.clear();
                            mConfirmPassInput.clear();
                            mTypingConfirm = false;
                            return;
                        }
                        mP2Username = acc.username;
                        mP2Account = acc;
                        mLoggedInUser = mP1Username;
                        mLoginState = LoginState::DONE;
                        mShop1.loadForPlayer(mP1Username);
                        mShop2.loadForPlayer(mP2Username);
                        mState = CHARACTER_SELECT;
                    }
                }
                else if (reg == Account_Manager::Result::USER_ALREADY_EXISTS) {
                    mLoginSuccess = false;
                    mLoginMessage = "USERNAME TAKEN. CHOOSE ANOTHER.";
                    mUsernameInput.clear();
                    mPasswordInput.clear();
                    mConfirmPassInput.clear();
                    mLoginState = LoginState::TYPING_USER;
                    mTypingConfirm = false;
                }
                else if (reg == Account_Manager::Result::DB_FULL) {
                    mLoginSuccess = false;
                    mLoginMessage = "SERVER FULL. CONTACT ADMIN.";
                }
            }
        }
    }
}


//  CHARACTER SELECT


void Game::renderCharSelect() {

    mWindow.draw(mCharSelectBgSprite);

    // p1name — polished header
    sf::Text p1name;
    p1name.setFont(mFont);
    p1name.setString("P1: " + mP1Username);
    p1name.setCharacterSize(18);
    p1name.setFillColor(sf::Color(135, 206, 235));
    p1name.setPosition(190.f, 255.f);
    mWindow.draw(p1name);

    // p2name — polished header
    sf::Text p2name;
    p2name.setFont(mFont);
    p2name.setString("P2: " + mP2Username);
    p2name.setCharacterSize(18);
    p2name.setFillColor(sf::Color(150, 255, 150));
    p2name.setPosition(430.f, 255.f);
    mWindow.draw(p2name);

    for (int i = 0; i < 3; i++) {
        float x = 150.f + i * 200.f;
        float boxW = 120.f;
        float boxH = 160.f;
        float boxY = 330.f;

        // draw outline box behind the image
        sf::RectangleShape box(sf::Vector2f(boxW, boxH));
        box.setPosition(x, boxY);
        box.setFillColor(sf::Color(40, 40, 70));
        box.setOutlineThickness(3.f);

        if (i == mP1CharIndex && i == mP2CharIndex)
            box.setOutlineColor(sf::Color::White);
        else if (i == mP1CharIndex)
            box.setOutlineColor(sf::Color::Cyan);
        else if (i == mP2CharIndex)
            box.setOutlineColor(sf::Color::Green);
        else
            box.setOutlineColor(sf::Color(80, 80, 120));

        mWindow.draw(box);

        // draw character sprite scaled to fit the box
        if (mCharTextures[i].getSize().x > 0) {
            sf::Vector2u texSize = mCharTextures[i].getSize();
            float scaleX = boxW / texSize.x;
            float scaleY = boxH / texSize.y;
            mCharSprites[i].setScale(scaleX, scaleY);
            mCharSprites[i].setPosition(x, boxY);
            mWindow.draw(mCharSprites[i]);
        }

        // character name below the box
        sf::Text name;
        name.setFont(mFont);
        name.setString(mCharacters[i].name);
        name.setCharacterSize(14);
        name.setFillColor(sf::Color::White);
        float nameX = x + (boxW - name.getGlobalBounds().width) / 2.f;
        name.setPosition(nameX, boxY + boxH + 8.f);
        mWindow.draw(name);
    }

    // p1inst — text swaps to "READY" + turns green once confirmed
    sf::Text p1inst;
    p1inst.setFont(mFont);
    p1inst.setString(mP1Selected
        ? "P1 READY: " + mCharacters[mP1CharIndex].name
        : "P1: LEFT/RIGHT TO SELECT, DOWN TO CONFIRM");
    p1inst.setCharacterSize(13);
    p1inst.setFillColor(mP1Selected ? sf::Color(150, 255, 150) : sf::Color(255, 255, 255));
    drawCenteredText(p1inst, 525.f);

    // p2inst — text swaps to "READY" + turns green once confirmed
    sf::Text p2inst;
    p2inst.setFont(mFont);
    p2inst.setString(mP2Selected
        ? "P2 READY: " + mCharacters[mP2CharIndex].name
        : "P2: A/D TO SELECT, S TO CONFIRM");
    p2inst.setCharacterSize(13);
    p2inst.setFillColor(mP2Selected ? sf::Color(150, 255, 150) : sf::Color(255, 255, 255));
    drawCenteredText(p2inst, 570.f);
}


//  MAIN MENU


void Game::renderMainMenu() {

    mWindow.draw(mMenuBgSprite);

    sf::Text title;
    title.setFont(mFont);
    title.setString("SNOW BROS");
    title.setCharacterSize(48);
    title.setFillColor(sf::Color::Red);
    drawCenteredText(title, 80.f);



    sf::Text players;
    players.setFont(mFont);
    players.setString(mP1Username + "  vs  " + mP2Username);
    players.setCharacterSize(22);
    players.setFillColor(sf::Color::Yellow);
    drawCenteredText(players, 200.f);

    std::string items[4] = { "NEW GAME", "CONTINUE", "LEADERBOARD", "EXIT" };

    for (int i = 0; i < 4; i++) {
        sf::Text item;
        item.setFont(mFont);
        item.setString(items[i]);
        item.setCharacterSize(20);

        if (i == mMenuSelection) {
            item.setFillColor(sf::Color::Yellow);
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

    sf::Text hint;
    hint.setFont(mFont);
    hint.setString("UP/DOWN TO NAVIGATE   ENTER TO SELECT");
    hint.setCharacterSize(10);
    hint.setFillColor(sf::Color(150, 150, 150));
    drawCenteredText(hint, 580.f);
}


//  PLAYING


void Game::renderPlaying() {
    drawBackground();
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

    sf::Text p1tag;
    p1tag.setFont(mFont);
    p1tag.setString(mP1Username);
    p1tag.setCharacterSize(10);
    p1tag.setFillColor(sf::Color::Cyan);
    p1tag.setPosition(10.f, 5.f);
    mWindow.draw(p1tag);

    sf::Text p2tag;
    p2tag.setFont(mFont);
    p2tag.setString(mP2Username);
    p2tag.setCharacterSize(10);
    p2tag.setFillColor(sf::Color(100, 255, 150));
    p2tag.setPosition(720.f, 5.f);
    mWindow.draw(p2tag);
}


//  LEVEL COMPLETE


void Game::renderLevelComplete() {
    renderPlaying();

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
    score.setString(mP1Username + ": " + std::to_string(mScore1));
    score.setCharacterSize(18);
    score.setFillColor(sf::Color::Cyan);
    drawCenteredText(score, 290.f);

    sf::Text score2;
    score2.setFont(mFont);
    score2.setString(mP2Username + ": " + std::to_string(mScore2));
    score2.setCharacterSize(18);
    score2.setFillColor(sf::Color(100, 255, 150));
    drawCenteredText(score2, 330.f);

    sf::Text gemsBonus;
    gemsBonus.setFont(mFont);
    gemsBonus.setString("P1 GEMS: " + std::to_string(mShop1.getGems()) +
        "   P2 GEMS: " + std::to_string(mShop2.getGems()));
    gemsBonus.setCharacterSize(13);
    gemsBonus.setFillColor(sf::Color(255, 215, 0));
    drawCenteredText(gemsBonus, 380.f);

    sf::Text next;
    next.setFont(mFont);
    next.setString("Press ENTER to continue");
    next.setCharacterSize(14);
    next.setFillColor(sf::Color::White);
    drawCenteredText(next, 420.f);
}


//  PAUSED


void Game::renderPaused() {
    renderPlaying();

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

    sf::Text shopHint;
    shopHint.setFont(mFont);
    shopHint.setString("O - OPEN SHOP");
    shopHint.setCharacterSize(16);
    shopHint.setFillColor(sf::Color(255, 215, 0));
    drawCenteredText(shopHint, 430.f);

    sf::Text gemsPaused;
    gemsPaused.setFont(mFont);
    gemsPaused.setString("P1 GEMS: " + std::to_string(mGemCount1) +
        "   P2 GEMS: " + std::to_string(mGemCount2));
    gemsPaused.setCharacterSize(13);
    gemsPaused.setFillColor(sf::Color(100, 200, 255));
    drawCenteredText(gemsPaused, 468.f);
}


//  GAME OVER


void Game::renderGameOver() {
    mWindow.draw(mGameOverBgSprite);

 

    sf::Text s1;
    s1.setFont(mFont);
    s1.setString(mP1Username + " SCORE: " + std::to_string(mScore1));
    s1.setCharacterSize(36);
    s1.setFillColor(sf::Color::Cyan);
    drawCenteredText(s1, 220.f);

    sf::Text s2;
    s2.setFont(mFont);
    s2.setString(mP2Username + " SCORE: " + std::to_string(mScore2));
    s2.setCharacterSize(36);
    s2.setFillColor(sf::Color(144,238,144));
    drawCenteredText(s2, 320.f);

    sf::Text level;
    level.setFont(mFont);
    level.setString("LEVEL REACHED: " + std::to_string(mCurrentLevel));
    level.setCharacterSize(34);
    level.setFillColor(sf::Color::White);
    drawCenteredText(level, 430.f);

    
}


//  VICTORY


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
    s1.setString(mP1Username + " SCORE: " + std::to_string(mScore1));
    s1.setCharacterSize(18);
    s1.setFillColor(sf::Color::Cyan);
    drawCenteredText(s1, 310.f);

    sf::Text s2;
    s2.setFont(mFont);
    s2.setString(mP2Username + " SCORE: " + std::to_string(mScore2));
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


//  LEVEL MANAGEMENT


void Game::loadCurrentLevel() {
    for (int i = 0; i < mEnemyCount; i++) {
        delete mEnemies[i];
        mEnemies[i] = nullptr;
    }
    mEnemyCount = 0;
    mPlatformCount = 0;

    bool success = mLevelManager.loadLevel(
        mCurrentLevel, mPlatforms, mPlatformCount, mEnemies, mEnemyCount);

    if (!success || mPlatformCount == 0) {
        mPlatforms[0] = Platform(0.f, 615.f, 800.f, 20.f);
        mPlatformCount = 1;
    }


    mPowerUpCount = 0;
    mSnowballPower1 = false;
    mSnowballPower2 = false;
    mDistanceBoost1 = false;
    mDistanceBoost2 = false;

    mShop1.onNewLevel();
    mShop2.onNewLevel();
}

void Game::checkLevelComplete() {
    if (mEnemyCount == 0 && mPowerUpCount == 0) {
        if (mCurrentLevel >= mLevelManager.getTotalLevels()) {
            mState = VICTORY;
            return;
        }
        mShop1.addGems(10);
        mShop2.addGems(10);
        mGemCount1 += 10;
        mGemCount2 += 10;
        mCurrentLevel++;
        mState = LEVEL_COMPLETE;

    }
}


//  COLLISION CHECKS


void Game::checkSnowballEnemyCollision() {
    for (int i = 0; i < mSnowballCount; i++) {
        if (mSnowballs[i].isExpired()) continue;
        for (int j = 0; j < mEnemyCount; j++) {
            if (mSnowballs[i].getBounds().intersects(mEnemies[j]->getBounds()))
            {
                Boss* boss = dynamic_cast<Boss*>(mEnemies[j]);
                

                if (boss != nullptr) {
                    boss->takeBossHit();
                }
               
                else {
                    bool p1Power = mSnowballs[i].getPlayerIndex() == 0 && mSnowballPower1;
                    bool p2Power = mSnowballs[i].getPlayerIndex() == 1 && mSnowballPower2;
                    if (p1Power || p2Power)
                        mEnemies[j]->instantEncase();
                    else
                        mEnemies[j]->takeDamage();
                }

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
                mPlayer1.getBounds().intersects(mEnemies[i]->getBounds()) &&
                sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
                mEnemies[i]->startRolling(mPlayer1.isFacingRight(), 0);
                mScore1 += mEnemies[i]->getPoints();
            }
            if (mPlayer2.isAlive() &&
                mPlayer2.getBounds().intersects(mEnemies[i]->getBounds()) &&
                sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
                mEnemies[i]->startRolling(mPlayer2.isFacingRight(), 1);
                mScore2 += mEnemies[i]->getPoints();
            }
            continue;
        }

        if (mEnemies[i]->isPartiallyEncased()) continue;

        if (mPlayer1.isAlive() && !mPlayer1.isRespawning() && !mPlayer1.isInBalloonMode() &&
            mPlayer1.getBounds().intersects(mEnemies[i]->getBounds()))
            mPlayer1.loseLife();

        if (mPlayer2.isAlive() && !mPlayer2.isRespawning() && !mPlayer2.isInBalloonMode() &&
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

            // rolling snowball cannot kill boss — skip it
            Boss* boss = dynamic_cast<Boss*>(mEnemies[j]);
            if (boss != nullptr) continue;


            if (!mEnemies[i]->getBounds().intersects(mEnemies[j]->getBounds()))
                continue;

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
                bool isBoss = (points >= 200);
                if (isBoss) {
                    if (mEnemies[i]->getKickedByPlayer() == 0)
                        mShop1.addGems(200);
                    else
                        mShop2.addGems(200);
                }
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


//  LOGIN FILE HELPERS  (legacy — kept for compatibility)


bool Game::checkLogin(const std::string& user, const std::string& pass) {
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

void Game::saveLogin(const std::string& user, const std::string& pass) {
    std::ofstream file(
        "E:/Semester 2/OOP/LAB/SNOWBROS/SFML_VS_Setup_2026/data/users.txt",
        std::ios::app);
    if (file.is_open()) {
        file << user << " " << pass << "\n";
        file.close();
    }
}


//  DYNAMIC ARRAY HELPERS


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
            mShop1.addGems(10);

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
            mShop2.addGems(10);

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


//  SHOP HELPERS


void Game::loadShopsForPlayers() {
    mShop1.loadForPlayer(mP1Username);
    mShop2.loadForPlayer(mP2Username);
}

void Game::saveShopsForPlayers() {
    mShop1.saveForPlayer(mP1Username);
    mShop2.saveForPlayer(mP2Username);
}

void Game::updateShop(float deltaTime) {
    mShop1.updateSpeedBoost(deltaTime);
    mShop1.updateBalloonMode(deltaTime);
    mShop2.updateSpeedBoost(deltaTime);
    mShop2.updateBalloonMode(deltaTime);
}

//------------shop purchase--------------------

void Game::applyShopPurchase(ShopItem item, int player) {
    switch (item) {
    case ITEM_EXTRA_LIFE:
        if (player == 1) mPlayer1.addLife();
        else             mPlayer2.addLife();
        break;
    case ITEM_SPEED_BOOST:
        if (player == 1)
        {
            mSpeedBoostTimer1 = 30.f;
            mPlayer1.setSpeedBoost(true);
        }
        else {
            mSpeedBoostTimer2 = 30.f;
            mPlayer2.setSpeedBoost(true);
        }
        break;
    case ITEM_SNOWBALL_POWER:
        if (player == 1)
            mSnowballPower1 = true;
        else
            mSnowballPower2 = true;
        break;
    case ITEM_DISTANCE_BOOST:
        if (player == 1)
            mDistanceBoost1 = true;
        else
            mDistanceBoost2 = true;
        break;
    case ITEM_BALLOON_MODE:
        if (player == 1) {
            mBalloonTimer1 = 30.f;
            mPlayer1.setBalloonMode(true);
        }
        else {
            mBalloonTimer2 = 30.f;
            mPlayer2.setBalloonMode(true);
        }
        break;
    default: break;
    }
}


void Game::renderShop() {

    mWindow.draw(mShopBgSprite);






    // ── Title panel ─────────────────────────────────────────────
    sf::RectangleShape titlePanel(sf::Vector2f(500.f, 50.f));
    titlePanel.setPosition(150.f, 18.f);
    titlePanel.setFillColor(sf::Color(15, 25, 70));
    titlePanel.setOutlineColor(sf::Color(80, 160, 255));
    titlePanel.setOutlineThickness(2.f);
    mWindow.draw(titlePanel);

    for (int cx = 0; cx < 2; cx++) {
        sf::CircleShape corner(6.f, 4);
        corner.setFillColor(sf::Color(255, 215, 0));
        corner.setPosition(cx == 0 ? 152.f : 637.f, 21.f);
        mWindow.draw(corner);
    }

    sf::Text title;
    title.setFont(mFont);
    title.setString(" SNOWBROS SHOP ");
    title.setCharacterSize(22);
    title.setFillColor(sf::Color(255, 220, 40));
    drawCenteredText(title, 26.f);

    std::string activeUser = (mShopPlayerTurn == 1) ? mP1Username : mP2Username;
    sf::Text sub;
    sub.setFont(mFont);
    sub.setString("PLAYER: " + activeUser + "    [TAB] = SWITCH PLAYER");
    sub.setCharacterSize(10);
    sub.setFillColor(mShopPlayerTurn == 1 ?
        sf::Color(100, 200, 255) : sf::Color(100, 255, 160));
    drawCenteredText(sub, 74.f);

    // ── Gem balance badge ────────────────────────────────────────
    sf::RectangleShape gemBadge(sf::Vector2f(280.f, 40.f));
    gemBadge.setPosition(260.f, 92.f);
    gemBadge.setFillColor(sf::Color(30, 25, 10));
    gemBadge.setOutlineColor(sf::Color(255, 215, 0));
    gemBadge.setOutlineThickness(2.f);
    mWindow.draw(gemBadge);

    sf::CircleShape gemIcon(8.f, 4);
    gemIcon.setFillColor(sf::Color(100, 220, 255));
    gemIcon.setOutlineColor(sf::Color(200, 240, 255));
    gemIcon.setOutlineThickness(1.5f);
    gemIcon.setPosition(274.f, 100.f);
    gemIcon.setRotation(45.f);
    mWindow.draw(gemIcon);

    sf::Text gemsLabel;
    gemsLabel.setFont(mFont);
    Shop& activeShop = (mShopPlayerTurn == 1) ? mShop1 : mShop2;
    gemsLabel.setString("GEMS:  " + std::to_string(activeShop.getGems()));
    gemsLabel.setCharacterSize(15);
    gemsLabel.setFillColor(sf::Color(255, 215, 0));
    gemsLabel.setPosition(298.f, 101.f);
    mWindow.draw(gemsLabel);

    // ── Earn-rates info bar ──────────────────────────────────────
    sf::RectangleShape earnBar(sf::Vector2f(640.f, 24.f));
    earnBar.setPosition(80.f, 140.f);
    earnBar.setFillColor(sf::Color(15, 20, 55));
    earnBar.setOutlineColor(sf::Color(40, 80, 160));
    earnBar.setOutlineThickness(1.f);
    mWindow.draw(earnBar);

    sf::Text earnInfo;
    earnInfo.setFont(mFont);
    earnInfo.setString("EARN:  boss kill +200   level complete +20");
    earnInfo.setCharacterSize(9);
    earnInfo.setFillColor(sf::Color(120, 180, 255));
    earnInfo.setPosition(90.f, 148.f);
    mWindow.draw(earnInfo);

    // ── Item rows ────────────────────────────────────────────────
    sf::Color tagColors[ITEM_COUNT] = {
        sf::Color(255, 100, 120),
        sf::Color(100, 220, 255),
        sf::Color(180, 120, 255),
        sf::Color(100, 255, 180),
        sf::Color(255, 200, 80),
    };

    for (int i = 0; i < ITEM_COUNT; i++) {
        const ShopItemInfo& info = activeShop.getItemInfo((ShopItem)i);
        bool selected = (activeShop.getSelectedIndex() == i);
        bool canAfford = (activeShop.getGems() >= info.cost);

        float rowY = 172.f + i * 82.f;

        sf::RectangleShape row(sf::Vector2f(640.f, 72.f));
        row.setPosition(80.f, rowY);
        row.setFillColor(selected ? sf::Color(20, 40, 80) : sf::Color(13, 15, 40));
        row.setOutlineColor(selected ? tagColors[i] : sf::Color(35, 45, 90));
        row.setOutlineThickness(selected ? 2.f : 1.f);
        mWindow.draw(row);

        sf::RectangleShape accent(sf::Vector2f(5.f, 72.f));
        accent.setPosition(80.f, rowY);
        accent.setFillColor(sf::Color(tagColors[i].r, tagColors[i].g, tagColors[i].b,
            selected ? 255 : 100));
        mWindow.draw(accent);

        if (selected) {
            sf::Text arrow;
            arrow.setFont(mFont);
            arrow.setString(">");
            arrow.setCharacterSize(16);
            arrow.setFillColor(tagColors[i]);
            arrow.setPosition(90.f, rowY + 12.f);
            mWindow.draw(arrow);
        }

        sf::Text nameText;
        nameText.setFont(mFont);
        nameText.setString(info.name);
        nameText.setCharacterSize(13);
        nameText.setFillColor(selected ? tagColors[i] : sf::Color(220, 220, 220));
        nameText.setPosition(108.f, rowY + 8.f);
        mWindow.draw(nameText);

        sf::Text effText;
        effText.setFont(mFont);
        effText.setCharacterSize(9);
        effText.setFillColor(sf::Color(150, 160, 190));
        effText.setPosition(108.f, rowY + 32.f);
        mWindow.draw(effText);

        sf::Color costFill = canAfford ? sf::Color(20, 50, 20) : sf::Color(50, 15, 15);
        sf::Color costLine = canAfford ? sf::Color(80, 200, 80) : sf::Color(180, 60, 60);
        sf::Color costTxt = canAfford ? sf::Color(100, 255, 100) : sf::Color(220, 80, 80);

        sf::RectangleShape costBox(sf::Vector2f(110.f, 36.f));
        costBox.setPosition(598.f, rowY + 18.f);
        costBox.setFillColor(costFill);
        costBox.setOutlineColor(costLine);
        costBox.setOutlineThickness(1.5f);
        mWindow.draw(costBox);

        sf::CircleShape cGem(5.f, 4);
        cGem.setFillColor(sf::Color(100, 200, 255));
        cGem.setPosition(603.f, 26.f + rowY);
        cGem.setRotation(45.f);
        mWindow.draw(cGem);

        sf::Text costText;
        costText.setFont(mFont);
        costText.setString(std::to_string(info.cost) + " GEM");
        costText.setCharacterSize(11);
        costText.setFillColor(costTxt);
        costText.setPosition(615.f, rowY + 26.f);
        mWindow.draw(costText);
    }

    // ── Feedback message ─────────────────────────────────────────
    const std::string& lastMsg = activeShop.getLastMessage();
    if (!lastMsg.empty()) {
        bool isBad = (lastMsg.find("NOT") != std::string::npos);
        sf::RectangleShape msgBox(sf::Vector2f(600.f, 34.f));
        msgBox.setPosition(100.f, 589.f);
        msgBox.setFillColor(isBad ? sf::Color(60, 10, 10) : sf::Color(10, 50, 20));
        msgBox.setOutlineColor(isBad ? sf::Color(220, 60, 60) : sf::Color(60, 200, 80));
        msgBox.setOutlineThickness(1.5f);
        mWindow.draw(msgBox);

        sf::Text msgTxt;
        msgTxt.setFont(mFont);
        msgTxt.setString(lastMsg);
        msgTxt.setCharacterSize(12);
        msgTxt.setFillColor(isBad ? sf::Color(255, 100, 100) : sf::Color(100, 255, 130));
        drawCenteredText(msgTxt, 598.f);
    }

    // ── Controls hint ─────────────────────────────────────────────
    sf::Text hint;
    hint.setFont(mFont);
    hint.setString("UP/DOWN: select    ENTER: buy    ESC: back");
    hint.setCharacterSize(9);
    hint.setFillColor(sf::Color(80, 90, 130));
    drawCenteredText(hint, 638.f);
}

// bg helper function
void Game::drawBackground() {
    if (!mBgLoaded) return;

    int bgIndex = 0;

    if (mCurrentLevel >= 1 && mCurrentLevel <= 4)
        bgIndex = 0;  // img1
    else if (mCurrentLevel == 5)
        bgIndex = 1;  // img2
    else if (mCurrentLevel >= 6 && mCurrentLevel <= 9)
        bgIndex = 2;  // img3
    else if (mCurrentLevel == 10)
        bgIndex = 3;  // img4

    mWindow.draw(mBgSprites[bgIndex]);
}