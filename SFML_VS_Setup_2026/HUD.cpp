#include "HUD.h"

HUD::HUD() {
    // font loaded separately via loadFont()
}

bool HUD::loadFont(const std::string& fontPath) {
    if (!mFont.loadFromFile(fontPath)) {
        return false; // font failed to load
    }

    // setup all text objects after font is loaded

  

    // player 1 score — below label
    setupText(mScore1Text, 20, sf::Color::White, 9.f, 25.f);
    mScore1Text.setString("000000");

    // player 1 lives — below score
    setupText(mLives1Text, 18, sf::Color::Yellow, 10.f, 45.f);
    mLives1Text.setString("LIFE: 0");

 

    // player 2 score — below label
    setupText(mScore2Text, 20, sf::Color::White, 715.f, 25.f);
    mScore2Text.setString("000000");

    // player 2 lives — below score
    setupText(mLives2Text, 18, sf::Color::Yellow, 715.f, 45.f);
    mLives2Text.setString("LIFE: 0");

    // gemscount 
    setupText(mGemText1, 18, sf::Color(0, 255, 200), 9.f, 65.f);
    mGemText1.setString("GEMS: 0");

    setupText(mGemText2, 18, sf::Color(0, 255, 200), 714.f, 65.f);
    mGemText2.setString("GEMS: 0");


    // level indicator — top center
    setupText(mLevelText, 24, sf::Color(211, 211, 211), 360.f, 27.f);
    mLevelText.setString("LEVEL 1");

    

    return true;
}

void HUD::setupText(sf::Text& text, int size,
    sf::Color color, float x, float y) {
    text.setFont(mFont);
    text.setCharacterSize(size);
    text.setFillColor(color);
    text.setPosition(x, y);
}

void HUD::update(int score1, int lives1,
    int score2, int lives2,
    int level, int gems1, int gems2) {

    // update score — pad with zeros to 6 digits
    // like original arcade game display
    std::string s1 = std::to_string(score1);
    while (s1.size() < 6) s1 = "0" + s1; // pad left with zeros
    mScore1Text.setString(s1);

    std::string s2 = std::to_string(score2);
    while (s2.size() < 6) s2 = "0" + s2;
    mScore2Text.setString(s2);

    // update lives — show mLives - 1 as extra lives
    // because 3 total = 2 shown in HUD
    int extraLives1 = lives1 - 1;
    if (extraLives1 < 0) extraLives1 = 0;
    mLives1Text.setString("LIFE: " +
        std::to_string(extraLives1));

    int extraLives2 = lives2 - 1;
    if (extraLives2 < 0) extraLives2 = 0;
    mLives2Text.setString("LIFE: " +
        std::to_string(extraLives2));

    // update level
    mLevelText.setString("LEVEL " +
        std::to_string(level));

    // update gems
    mGemText1.setString("GEMS: " + std::to_string(gems1));
    mGemText2.setString("GEMS: " + std::to_string(gems2));
};


void HUD::draw(sf::RenderWindow& window) {


    // draw all text
    
    window.draw(mScore1Text);
    window.draw(mLives1Text);
   
    window.draw(mScore2Text);
    window.draw(mLives2Text);
    window.draw(mLevelText);
   
    // gems
    window.draw(mGemText1);
    window.draw(mGemText2);
}