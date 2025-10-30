#include "Game.hpp"
#include <iostream>
#include <cmath>
#include <map>

Game::Game(sf::Font &font) : font(font) { srand(time(0)); }

void Game::start(GameMode mode)
{
    gameMode = mode;
    initializePlayers();
    powerUps.clear();
    gameOver = false;
    winner = 0;
    tickTimer = 0.0f;
    powerUpSpawnTimer = 5.0f;
    clock.restart();
}

void Game::initializePlayers()
{
    players.clear();
    Player player1;
    player1.id = 1;
    player1.position = {GRID_SIZE / 4, GRID_SIZE / 2};
    player1.direction = Direction::RIGHT;
    player1.intendedDirection = Direction::RIGHT;
    player1.headColor = sf::Color(252, 165, 165);
    player1.trailColor = sf::Color(153, 27, 27);
    player1.isBot = false;
    player1.controlsReversed = false;
    players.push_back(player1);

    Player player2;
    player2.id = 2;
    player2.position = {GRID_SIZE * 3 / 4, GRID_SIZE / 2};
    player2.direction = Direction::LEFT;
    player2.intendedDirection = Direction::LEFT;
    player2.headColor = sf::Color(147, 197, 253);
    player2.trailColor = sf::Color(30, 58, 138);
    player2.isBot = (gameMode == GameMode::SINGLE);
    player2.controlsReversed = false;
    players.push_back(player2);
}

void Game::handleInput()
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) players[0].intendedDirection = Direction::UP;
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) players[0].intendedDirection = Direction::DOWN;
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) players[0].intendedDirection = Direction::LEFT;
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) players[0].intendedDirection = Direction::RIGHT;

    if (gameMode == GameMode::TWO_PLAYER && !players[1].isBot)
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) players[1].intendedDirection = Direction::UP;
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) players[1].intendedDirection = Direction::DOWN;
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) players[1].intendedDirection = Direction::LEFT;
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) players[1].intendedDirection = Direction::RIGHT;
    }
}

void Game::update()
{
    if (gameOver) return;
    float dt = clock.restart().asSeconds();
    tickTimer += dt;
    powerUpSpawnTimer -= dt;
    updatePowerUps(dt);
    if (powerUpSpawnTimer <= 0) {
        spawnPowerUp();
        powerUpSpawnTimer = POWERUP_SPAWN_INTERVAL;
    }
    if (tickTimer >= TICK_RATE) {
        tickTimer -= TICK_RATE;
        updatePlayerDirections();
        movePlayers();
        checkPowerUpCollection();
        checkCollisions();
    }
}

void Game::updatePowerUps(float dt) {
    for (auto& player : players) {
        if (player.activePowerUp) {
            player.activePowerUp->duration -= dt;
            if (player.activePowerUp->duration <= 0) {
                if (player.activePowerUp->type == PowerUpType::CONTROL_SWAP) {
                    player.controlsReversed = false;
                }
                player.activePowerUp.reset();
            }
        }
    }
}

void Game::updatePlayerDirections() {
    static const std::map<Direction, Direction> oppositeMap = {{Direction::UP, Direction::DOWN}, {Direction::DOWN, Direction::UP}, {Direction::LEFT, Direction::RIGHT}, {Direction::RIGHT, Direction::LEFT}};
    static const std::map<Direction, Direction> reversedMap = {{Direction::UP, Direction::DOWN}, {Direction::DOWN, Direction::UP}, {Direction::LEFT, Direction::RIGHT}, {Direction::RIGHT, Direction::LEFT}};
    for (auto& p : players) {
        if (p.isBot) p.intendedDirection = calculateBotMove(p, players[p.id - 1 == 0 ? 1 : 0]);
        Direction finalIntendedDir = p.intendedDirection;
        if(p.controlsReversed) finalIntendedDir = reversedMap.at(p.intendedDirection);
        if (oppositeMap.at(p.direction) != finalIntendedDir) p.direction = finalIntendedDir;
    }
}

void Game::movePlayers()
{
    for (auto &player : players) {
        player.trail.push_front(player.position);
        int steps = (player.activePowerUp && player.activePowerUp->type == PowerUpType::SPEED) ? 2 : 1;
        for(int i = 0; i < steps; ++i) {
            player.position = getNextPosition(player.position, player.direction);
            if (player.position.x < 0) player.position.x = GRID_SIZE - 1;
            else if (player.position.x >= GRID_SIZE) player.position.x = 0;
            if (player.position.y < 0) player.position.y = GRID_SIZE - 1;
            else if (player.position.y >= GRID_SIZE) player.position.y = 0;
        }
    }
}

void Game::checkPowerUpCollection() {
    for (auto& p : players) {
        auto it = powerUps.begin();
        while (it != powerUps.end()) {
            if (p.position == it->position) {
                Player& opponent = players[p.id == 1 ? 1 : 0];
                switch (it->type) {
                    case PowerUpType::SPEED: opponent.activePowerUp = ActivePowerUp{it->type, POWERUP_DURATION_SECONDS}; break;
                    case PowerUpType::CONTROL_SWAP: opponent.activePowerUp = ActivePowerUp{it->type, POWERUP_DURATION_SECONDS}; opponent.controlsReversed = true; break;
                    case PowerUpType::TRAIL_BREAKER: p.activePowerUp = ActivePowerUp{it->type, POWERUP_DURATION_SECONDS}; break;
                }
                it = powerUps.erase(it);
            } else {
                ++it;
            }
        }
    }
}

void Game::checkCollisions()
{
    std::vector<int> losers;
    for (auto& p : players) {
        for (const auto& other_p : players) {
            for (const auto& trail_pos : other_p.trail) {
                if (p.position == trail_pos) {
                    if (p.activePowerUp && p.activePowerUp->type == PowerUpType::TRAIL_BREAKER) p.activePowerUp.reset();
                    else losers.push_back(p.id);
                }
            }
        }
    }
    if (players[0].position == players[1].position) { losers.push_back(1); losers.push_back(2); }
    if (!losers.empty()) {
        gameOver = true;
        if (losers.size() > 1 || (losers.front() == 1 && losers.back() == 2)) winner = 0;
        else if (losers.front() == 1) winner = 2;
        else winner = 1;
    }
}

void Game::spawnPowerUp()
{
    if (powerUps.size() >= MAX_POWERUPS) return;
    Position spawnPos;
    do { spawnPos = {rand() % GRID_SIZE, rand() % GRID_SIZE}; } while (!isSafe(spawnPos));
    PowerUp newPowerUp;
    newPowerUp.position = spawnPos;
    newPowerUp.id = "p" + std::to_string(time(0));
    int type = rand() % 3;
    if (type == 0) newPowerUp.type = PowerUpType::SPEED;
    else if (type == 1) newPowerUp.type = PowerUpType::CONTROL_SWAP;
    else newPowerUp.type = PowerUpType::TRAIL_BREAKER;
    powerUps.push_back(newPowerUp);
}

Direction Game::calculateBotMove(const Player &bot, const Player &otherPlayer)
{
    static const std::map<Direction, Direction> oppositeMap = {{Direction::UP, Direction::DOWN}, {Direction::DOWN, Direction::UP}, {Direction::LEFT, Direction::RIGHT}, {Direction::RIGHT, Direction::LEFT}};
    std::vector<Direction> possibleMoves = {Direction::UP, Direction::DOWN, Direction::LEFT, Direction::RIGHT};
    Position nextPos = getNextPosition(bot.position, bot.direction);
    if(isSafe(nextPos)) return bot.direction;
    for (Direction move : possibleMoves) {
        if (move == oppositeMap.at(bot.direction)) continue;
        nextPos = getNextPosition(bot.position, move);
        if (isSafe(nextPos)) return move;
    }
    return bot.direction;
}

bool Game::isSafe(const Position &pos)
{
    Position testPos = pos;
    if (testPos.x < 0) testPos.x = GRID_SIZE - 1; else if (testPos.x >= GRID_SIZE) testPos.x = 0;
    if (testPos.y < 0) testPos.y = GRID_SIZE - 1; else if (testPos.y >= GRID_SIZE) testPos.y = 0;
    for (const auto &p : players) {
        for (const auto &trailPos : p.trail) if (testPos == trailPos) return false;
        if(testPos == p.position) return false;
    }
    return true;
}

Position Game::getNextPosition(const Position &position, Direction direction) const
{
    Position nextPos = position;
    switch (direction) {
        case Direction::UP: nextPos.y--; break;
        case Direction::DOWN: nextPos.y++; break;
        case Direction::LEFT: nextPos.x--; break;
        case Direction::RIGHT: nextPos.x++; break;
    }
    return nextPos;
}

void Game::draw(sf::RenderTarget &target)
{
    // --- THIS IS THE NEW CODE ---
    // Draw a border around the game area for better visibility.
    float thickness = 3.0f;
    sf::RectangleShape border;
    // Position and size the border to be just inside the 800x800 area
    border.setSize(sf::Vector2f(GAME_WIDTH - thickness * 2, GAME_HEIGHT - thickness * 2));
    border.setPosition(thickness, thickness);
    border.setFillColor(sf::Color::Transparent);
    border.setOutlineColor(sf::Color(100, 100, 100)); // A subtle dark grey
    border.setOutlineThickness(thickness);
    target.draw(border);
    // --- END OF NEW CODE ---

    float cellSize = GAME_WIDTH / GRID_SIZE;
    for (const auto &player : players) {
        for (const auto &trailPos : player.trail) {
            sf::RectangleShape trailSegment(sf::Vector2f(cellSize, cellSize));
            trailSegment.setPosition(trailPos.x * cellSize, trailPos.y * cellSize);
            trailSegment.setFillColor(player.trailColor);
            target.draw(trailSegment);
        }
    }
    for (const auto &powerUp : powerUps) {
        sf::Color color;
        if (powerUp.type == PowerUpType::SPEED) color = sf::Color::Yellow;
        else if (powerUp.type == PowerUpType::CONTROL_SWAP) color = sf::Color::Green;
        else color = sf::Color(138, 43, 226);
        sf::CircleShape glow(cellSize * 0.8f);
        glow.setOrigin(glow.getRadius(), glow.getRadius());
        glow.setPosition(powerUp.position.x * cellSize + cellSize / 2.f, powerUp.position.y * cellSize + cellSize / 2.f);
        glow.setFillColor(sf::Color(color.r, color.g, color.b, 80));
        target.draw(glow);
        sf::CircleShape powerUpShape(cellSize / 2.0f);
        powerUpShape.setPosition(powerUp.position.x * cellSize, powerUp.position.y * cellSize);
        powerUpShape.setFillColor(color);
        target.draw(powerUpShape);
    }
    for (const auto &player : players) {
        sf::RectangleShape glow(sf::Vector2f(cellSize, cellSize));
        glow.setScale(1.5f, 1.5f);
        glow.setOrigin(glow.getSize() / 2.f);
        glow.setPosition(player.position.x * cellSize + cellSize / 2.f, player.position.y * cellSize + cellSize / 2.f);
        glow.setFillColor(sf::Color(player.headColor.r, player.headColor.g, player.headColor.b, 100));
        target.draw(glow);
        sf::RectangleShape playerHead(sf::Vector2f(cellSize, cellSize));
        playerHead.setPosition(player.position.x * cellSize, player.position.y * cellSize);
        playerHead.setFillColor(player.headColor);
        target.draw(playerHead);
    }
}

bool Game::isGameOver() const { return gameOver; }
int Game::getWinner() const { return winner; }