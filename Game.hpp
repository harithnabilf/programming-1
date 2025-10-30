#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "types.hpp"

class Game
{
public:
    Game(sf::Font &font);
    void start(GameMode mode);
    void handleInput();
    void update();
    void draw(sf::RenderTarget &target);
    bool isGameOver() const;
    int getWinner() const;

private:
    void initializePlayers();
    void updatePlayerDirections();
    void updatePowerUps(float dt);
    void movePlayers();
    void checkCollisions();
    void checkPowerUpCollection();
    void spawnPowerUp();
    Direction calculateBotMove(const Player &bot, const Player &otherPlayer);
    bool isSafe(const Position &pos);
    Position getNextPosition(const Position &position, Direction direction) const;

    sf::Font &font;
    GameMode gameMode;
    std::vector<Player> players;
    std::vector<PowerUp> powerUps;
    bool gameOver = false;
    int winner = 0;
    sf::Clock clock;

    const int GRID_SIZE = 40;
    const float GAME_WIDTH = 800.0f;
    const float GAME_HEIGHT = 800.0f;
    const float TICK_RATE = 0.08f;
    const float POWERUP_DURATION_SECONDS = 7.0f;
    const float POWERUP_SPAWN_INTERVAL = 8.0f;
    const int MAX_POWERUPS = 3;

    float tickTimer = 0.0f;
    float powerUpSpawnTimer = 5.0f;
};