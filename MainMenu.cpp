#include "MainMenu.hpp"

void centerMenuTextOrigin(sf::Text &text) {
    sf::FloatRect bounds = text.getLocalBounds();
    text.setOrigin(bounds.left + bounds.width / 2.0f, bounds.top + bounds.height / 2.0f);
}

MainMenu::MainMenu(float width, float height, sf::Font &font) : font(font)
{
    float centerX = width / 2.0f;

    title.setFont(font);
    title.setString("TRON");
    title.setCharacterSize(100);
    title.setFillColor(sf::Color::White);
    title.setStyle(sf::Text::Bold);
    centerMenuTextOrigin(title);
    title.setPosition(sf::Vector2f(centerX, height / 4.0f));

    singlePlayerButton.setFont(font);
    singlePlayerButton.setString("1 Player");
    singlePlayerButton.setCharacterSize(40);
    singlePlayerButton.setFillColor(sf::Color::Red);
    centerMenuTextOrigin(singlePlayerButton);
    singlePlayerButton.setPosition(sf::Vector2f(centerX, height / 2.0f - 50));

    twoPlayerButton.setFont(font);
    twoPlayerButton.setString("2 Players");
    twoPlayerButton.setCharacterSize(40);
    twoPlayerButton.setFillColor(sf::Color::Blue);
    centerMenuTextOrigin(twoPlayerButton);
    twoPlayerButton.setPosition(sf::Vector2f(centerX, height / 2.0f + 50));

    howToPlayButton.setFont(font);
    howToPlayButton.setString("How To Play");
    howToPlayButton.setCharacterSize(24);
    howToPlayButton.setFillColor(sf::Color(200, 200, 200));
    centerMenuTextOrigin(howToPlayButton);
    howToPlayButton.setPosition(sf::Vector2f(centerX, height / 2.0f + 150));
}

// Updated to draw to any RenderTarget (window or texture)
void MainMenu::draw(sf::RenderTarget &target)
{
    target.draw(title);
    target.draw(singlePlayerButton);
    target.draw(twoPlayerButton);
    target.draw(howToPlayButton);
}

// Updated to use the pre-calculated mouse position
void MainMenu::handleInput(sf::Event event, sf::Vector2f mousePos)
{
    singlePlayerButton.setFillColor(sf::Color::Red);
    twoPlayerButton.setFillColor(sf::Color::Blue);
    howToPlayButton.setFillColor(sf::Color(200, 200, 200));

    if (singlePlayerButton.getGlobalBounds().contains(mousePos)) {
        singlePlayerButton.setFillColor(sf::Color::White);
    }
    if (twoPlayerButton.getGlobalBounds().contains(mousePos)) {
        twoPlayerButton.setFillColor(sf::Color::White);
    }
    if (howToPlayButton.getGlobalBounds().contains(mousePos)) {
        howToPlayButton.setFillColor(sf::Color::White);
    }
    
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
    {
        if (singlePlayerButton.getGlobalBounds().contains(mousePos)) singlePlayerClicked = true;
        else if (twoPlayerButton.getGlobalBounds().contains(mousePos)) twoPlayerClicked = true;
        else if (howToPlayButton.getGlobalBounds().contains(mousePos)) howToPlayClicked = true;
    }
}

bool MainMenu::isSinglePlayerClicked() { return singlePlayerClicked; }
bool MainMenu::isTwoPlayerClicked() { return twoPlayerClicked; }
bool MainMenu::isHowToPlayClicked() { return howToPlayClicked; }

void MainMenu::reset() {
    singlePlayerClicked = false;
    twoPlayerClicked = false;
    howToPlayClicked = false;
}