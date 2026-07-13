#pragma once
#include <SFML/Graphics.hpp>

// HUD: displays score, lives and level on screen
// uses Press Start 2P font for arcade style look
class HUD {
public:
    HUD();

    // load font from file
    bool loadFont(const std::string& fontPath);

    // update HUD values every frame (gems = shared pool)
    void update(int score1, int lives1,
        int score2, int lives2,
        int level, int gems1, int gems2);

    // draw HUD on top of everything
    void draw(sf::RenderWindow& window);


private:
    sf::Font mFont;

    // player 1 — top left
    sf::Text mScore1Text;
    sf::Text mLives1Text;
    
    sf::Text mGemText1;

    // player 2 — top right
    sf::Text mScore2Text;
    sf::Text mLives2Text;
   
    sf::Text mGemText2;

    // level — top center
    sf::Text mLevelText;

   

    // helper to setup text properties
    void setupText(sf::Text& text, int size,
        sf::Color color, float x, float y);
};