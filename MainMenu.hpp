#pragma once
#include <SFML/Graphics.hpp>
#include "types.hpp"

class MainMenu
{
public:
    MainMenu(float width, float height, sf::Font &font);
    void draw(sf::RenderTarget &target);
    // Updated handleInput signature
    void handleInput(sf::Event event, sf::Vector2f mousePos);
    bool isSinglePlayerClicked();
    bool isTwoPlayerClicked();
    bool isHowToPlayClicked();
    void reset();

private:
    sf::Font &font;
    sf::Text title;
    sf::Text singlePlayerButton;
    sf::Text twoPlayerButton;
    sf::Text howToPlayButton;

    bool singlePlayerClicked = false;
    bool twoPlayerClicked = false;
    bool howToPlayClicked = false;
};