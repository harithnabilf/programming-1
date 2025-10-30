#include "GameOver.hpp"
#include <string>

GameOver::GameOver(sf::Font &font, float width, float height) : font(font)
{
    gameOverText.setFont(font);
    gameOverText.setString("Game Over");
    gameOverText.setCharacterSize(80);
    gameOverText.setStyle(sf::Text::Bold);
    gameOverText.setFillColor(sf::Color::White);

    winnerText.setFont(font);
    winnerText.setCharacterSize(50);

    restartText.setFont(font);
    restartText.setString("Play Again");
    restartText.setCharacterSize(30);
    restartText.setFillColor(sf::Color::White);

    mainMenuText.setFont(font);
    mainMenuText.setString("Main Menu");
    mainMenuText.setCharacterSize(30);
    mainMenuText.setFillColor(sf::Color::White);

    sf::FloatRect goRect = gameOverText.getLocalBounds();
    gameOverText.setOrigin(goRect.left + goRect.width / 2.0f, goRect.top + goRect.height / 2.0f);
    gameOverText.setPosition(width / 2.0f, height / 3.0f);

    sf::FloatRect rtRect = restartText.getLocalBounds();
    restartText.setOrigin(rtRect.left + rtRect.width / 2.0f, rtRect.top + rtRect.height / 2.0f);
    restartText.setPosition(width / 2.0f, height * 2.0f / 3.0f);

    sf::FloatRect mmRect = mainMenuText.getLocalBounds();
    mainMenuText.setOrigin(mmRect.left + mmRect.width / 2.0f, mmRect.top + mmRect.height / 2.0f);
    mainMenuText.setPosition(width / 2.0f, restartText.getPosition().y + 60);
}

void GameOver::setWinner(int winnerId, GameMode mode)
{
    if (winnerId == 0) {
        winnerText.setString("Draw!");
        winnerText.setFillColor(sf::Color::White);
    } else {
        if (mode == GameMode::SINGLE) {
            winnerText.setString(winnerId == 1 ? "You Win!" : "Bot Wins!");
        } else {
            winnerText.setString("Player " + std::to_string(winnerId) + " Wins!");
        }
        winnerText.setFillColor(winnerId == 1 ? sf::Color(252, 165, 165) : sf::Color(147, 197, 253));
    }
    sf::FloatRect wtRect = winnerText.getLocalBounds();
    winnerText.setOrigin(wtRect.left + wtRect.width / 2.0f, wtRect.top + wtRect.height / 2.0f);
    winnerText.setPosition(gameOverText.getPosition().x, gameOverText.getPosition().y + 100);
}

void GameOver::draw(sf::RenderTarget &target)
{
    target.draw(gameOverText);
    target.draw(winnerText);
    target.draw(restartText);
    target.draw(mainMenuText);
}

void GameOver::handleInput(const sf::Event& event, sf::Vector2f mousePos) {
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        if (restartText.getGlobalBounds().contains(mousePos)) restartClicked = true;
        else if (mainMenuText.getGlobalBounds().contains(mousePos)) mainMenuClicked = true;
    }
}

bool GameOver::isRestartClicked() const { return restartClicked; }
bool GameOver::isMainMenuClicked() const { return mainMenuClicked; }

void GameOver::reset() {
    restartClicked = false;
    mainMenuClicked = false;
}