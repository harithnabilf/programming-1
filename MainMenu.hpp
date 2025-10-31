#pragma once
#include <SFML/Graphics.hpp>
// Corrected to use .hpp
#include "types.hpp"

class MainMenu
{
public:
    MainMenu(float width, float height, sf::Font &font);
    void draw(sf::RenderTarget &target);
    void handleInput(sf::Event event, sf::Vector2f mousePos);
    bool isSinglePlayerClicked();
    bool isTwoPlayerClicked();
    bool isHowToPlayClicked();
    void reset();

private:
    // This part of the class was already correct
    sf::Font &font;
    sf::Text title;
    sf::Text singlePlayerButton;
    sf::Text twoPlayerButton;
    sf::Text howToPlayButton;
    sf::Text exitText;
    
    // Member variables for state
    bool singlePlayerClicked = false;
    bool twoPlayerClicked = false;
    bool howToPlayClicked = false;
};