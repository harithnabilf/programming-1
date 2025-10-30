#pragma once
#include <SFML/Graphics.hpp>
#include "types.hpp"

class GameOver
{
public:
    GameOver(sf::Font &font, float width, float height);
    void setWinner(int winnerId, GameMode mode);
    void draw(sf::RenderTarget &target);
    void handleInput(const sf::Event& event, sf::Vector2f mousePos);
    bool isRestartClicked() const;
    bool isMainMenuClicked() const;
    void reset();

private:
    sf::Font &font;
    sf::Text gameOverText;
    sf::Text winnerText;
    sf::Text restartText;
    sf::Text mainMenuText;

    bool restartClicked = false;
    bool mainMenuClicked = false;
};