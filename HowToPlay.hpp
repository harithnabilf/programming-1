#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

class HowToPlay
{
public:
    HowToPlay(sf::Font &font, float width, float height);
    void draw(sf::RenderTarget &target);
    void handleInput(const sf::Event& event, sf::Vector2f mousePos);
    bool shouldClose() const;
    void reset();

private:
    void createKey(const sf::String& text, sf::Vector2f position);
    void createPowerUp(const std::string& name, const std::string& description, sf::Color color, float yPos);

    sf::Font& font;
    sf::Text controlsTitle;
    sf::Text player1Label;
    sf::Text player2Label;
    sf::Text powerUpsLabel;
    sf::RectangleShape separator;
    sf::Text mainMenuButton;

    std::vector<sf::RectangleShape> keyShapes;
    std::vector<sf::Text> keyTexts;

    std::vector<sf::CircleShape> powerUpIcons;
    std::vector<sf::Text> powerUpTexts;

    bool closeScreen = false;
};