#include "MainMenu.hpp"

void centerMenuTextOrigin(sf::Text &text) {
    sf::FloatRect bounds = text.getLocalBounds();
    text.setOrigin(bounds.left + bounds.width / 2.0f, bounds.top + bounds.height / 2.0f);
}

MainMenu::MainMenu(float width, float height, sf::Font &font) : font(font)
{
    float centerX = width / 2.0f;

    // Game Title
    title.setFont(font);
    title.setString("PHOTRON");
    title.setCharacterSize(100);
    title.setFillColor(sf::Color::White);
    title.setStyle(sf::Text::Bold);
    centerMenuTextOrigin(title);
    title.setPosition(sf::Vector2f(centerX, height / 4.0f));

    // Single Player Button
    singlePlayerButton.setFont(font);
    singlePlayerButton.setString("1 Player");
    singlePlayerButton.setCharacterSize(40);
    singlePlayerButton.setFillColor(sf::Color::Red);
    centerMenuTextOrigin(singlePlayerButton);
    singlePlayerButton.setPosition(sf::Vector2f(centerX, height / 2.0f - 50));

    // Two Players Button
    twoPlayerButton.setFont(font);
    twoPlayerButton.setString("2 Players");
    twoPlayerButton.setCharacterSize(40);
    twoPlayerButton.setFillColor(sf::Color::Blue);
    centerMenuTextOrigin(twoPlayerButton);
    twoPlayerButton.setPosition(sf::Vector2f(centerX, height / 2.0f + 50));

    // How to Play Button
    howToPlayButton.setFont(font);
    howToPlayButton.setString("How To Play");
    howToPlayButton.setCharacterSize(24);
    howToPlayButton.setFillColor(sf::Color(200, 200, 200));
    centerMenuTextOrigin(howToPlayButton);
    howToPlayButton.setPosition(sf::Vector2f(centerX, height / 2.0f + 150));

    // Exit Text
    exitText.setFont(font);
    exitText.setString("Press ESC to Exit");
    exitText.setCharacterSize(20);
    exitText.setFillColor(sf::Color(150, 150, 150));
    centerMenuTextOrigin(exitText);
    exitText.setPosition(sf::Vector2f(centerX, howToPlayButton.getPosition().y + 50));
}

void MainMenu::draw(sf::RenderTarget &target)
{
    target.draw(title);
    target.draw(singlePlayerButton);
    target.draw(twoPlayerButton);
    target.draw(howToPlayButton);
    target.draw(exitText);
}

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