#include "Game.hpp" // Use .hpp for C++ header convention
#include <iostream>
#include <cmath>
#include <map>

//==================================================================================
// Constructor
// Initializes the game object. The main purpose here is to seed the random number
// generator, which will be used for placing power-ups.
//==================================================================================
Game::Game(sf::Font &font) : font(font) 
{
    // Seed the random number generator with the current time.
    // This ensures that power-up locations will be different each time the game is run.
    srand(time(0)); 
}

//==================================================================================
// start
// This function is called to begin a new game session. It resets all game variables
// to their default states.
//==================================================================================
void Game::start(GameMode mode)
{
    gameMode = mode;
    initializePlayers(); // Set up the player objects.
    powerUps.clear();    // Remove any power-ups from a previous game.
    gameOver = false;    // Reset the game over flag.
    winner = 0;          // Reset the winner (0 means no winner/draw).
    tickTimer = 0.0f;    // Reset the timer that controls game speed.
    powerUpSpawnTimer = 5.0f; // Schedule the first power-up to spawn in 5 seconds.
    clock.restart();     // Restart the main clock.
}

//==================================================================================
// initializePlayers
// Creates and configures the two player objects at the start of a game.
//==================================================================================
void Game::initializePlayers()
{
    players.clear(); // Clear any players from a previous game.

    // --- Configure Player 1 ---
    Player player1;
    player1.id = 1;
    player1.position = {GRID_SIZE / 4, GRID_SIZE / 2}; // Start on the left side.
    player1.direction = Direction::RIGHT; // Move right initially.
    player1.intendedDirection = Direction::RIGHT;
    player1.headColor = sf::Color(252, 165, 165); // Light Red
    player1.trailColor = sf::Color(153, 27, 27);  // Dark Red
    player1.isBot = false; // Player 1 is always human.
    player1.controlsReversed = false;
    players.push_back(player1);

    // --- Configure Player 2 ---
    Player player2;
    player2.id = 2;
    player2.position = {GRID_SIZE * 3 / 4, GRID_SIZE / 2}; // Start on the right side.
    player2.direction = Direction::LEFT; // Move left initially.
    player2.intendedDirection = Direction::LEFT;
    player2.headColor = sf::Color(147, 197, 253); // Light Blue
    player2.trailColor = sf::Color(30, 58, 138);   // Dark Blue
    // Player 2 is a bot only if the game mode is 'SINGLE'.
    player2.isBot = (gameMode == GameMode::SINGLE);
    player2.controlsReversed = false;
    players.push_back(player2);
}

//==================================================================================
// handleInput
// Checks for real-time keyboard presses to update the players' intended directions.
// This function is called every frame.
//==================================================================================
void Game::handleInput()
{
    // Player 1 controls (WASD)
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) players[0].intendedDirection = Direction::UP;
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) players[0].intendedDirection = Direction::DOWN;
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) players[0].intendedDirection = Direction::LEFT;
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) players[0].intendedDirection = Direction::RIGHT;

    // Player 2 controls (Arrow Keys), only enabled in two-player mode.
    if (gameMode == GameMode::TWO_PLAYER && !players[1].isBot)
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) players[1].intendedDirection = Direction::UP;
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) players[1].intendedDirection = Direction::DOWN;
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) players[1].intendedDirection = Direction::LEFT;
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) players[1].intendedDirection = Direction::RIGHT;
    }
}

//==================================================================================
// update
// The main game logic loop. This function is called every frame but only executes
// its logic (like moving players) at a fixed interval defined by TICK_RATE.
//==================================================================================
void Game::update()
{
    if (gameOver) return; // If the game is over, do nothing.

    // Accumulate the time that has passed since the last frame.
    float dt = clock.restart().asSeconds();
    tickTimer += dt;
    powerUpSpawnTimer -= dt;

    updatePowerUps(dt); // Update active power-up timers.

    // Check if it's time to spawn a new power-up.
    if (powerUpSpawnTimer <= 0) {
        spawnPowerUp();
        powerUpSpawnTimer = POWERUP_SPAWN_INTERVAL; // Reset the spawn timer.
    }

    // Check if enough time has passed to run a game "tick".
    if (tickTimer >= TICK_RATE) {
        tickTimer -= TICK_RATE; // Reset the tick timer.
        
        // Execute one turn of game logic.
        updatePlayerDirections();
        movePlayers();
        checkPowerUpCollection();
        checkCollisions();
    }
}

//==================================================================================
// updatePowerUps
// Decrements the duration of any active power-ups on players.
//==================================================================================
void Game::updatePowerUps(float dt) {
    for (auto& player : players) {
        if (player.activePowerUp) { // Check if the player has a power-up.
            player.activePowerUp->duration -= dt; // Reduce its remaining time.
            if (player.activePowerUp->duration <= 0) {
                // If the power-up has expired, revert its effects.
                if (player.activePowerUp->type == PowerUpType::CONTROL_SWAP) {
                    player.controlsReversed = false;
                }
                player.activePowerUp.reset(); // Remove the power-up.
            }
        }
    }
}

//==================================================================================
// updatePlayerDirections
// Finalizes the player's actual direction for the current tick based on their
// intended direction, preventing illegal 180-degree turns.
//==================================================================================
void Game::updatePlayerDirections() {
    // A map to quickly find the opposite of a direction.
    static const std::map<Direction, Direction> oppositeMap = {{Direction::UP, Direction::DOWN}, {Direction::DOWN, Direction::UP}, {Direction::LEFT, Direction::RIGHT}, {Direction::RIGHT, Direction::LEFT}};
    static const std::map<Direction, Direction> reversedMap = {{Direction::UP, Direction::DOWN}, {Direction::DOWN, Direction::UP}, {Direction::LEFT, Direction::RIGHT}, {Direction::RIGHT, Direction::LEFT}};

    for (auto& p : players) {
        // If this player is a bot, calculate its move.
        if (p.isBot) p.intendedDirection = calculateBotMove(p, players[p.id - 1 == 0 ? 1 : 0]);
        
        Direction finalIntendedDir = p.intendedDirection;
        // If the 'Control Swap' power-up is active, reverse the intended direction.
        if(p.controlsReversed) finalIntendedDir = reversedMap.at(p.intendedDirection);
        
        // Only change direction if the new direction is not the direct opposite of the current one.
        if (oppositeMap.at(p.direction) != finalIntendedDir) {
            p.direction = finalIntendedDir;
        }
    }
}

//==================================================================================
// movePlayers
// Updates each player's position based on their current direction and handles
// screen wrapping.
//==================================================================================
void Game::movePlayers()
{
    for (auto &player : players) {
        // Add the player's current position to the front of their trail.
        player.trail.push_front(player.position);
        
        // Check for the 'Speed' power-up. If active, move two steps instead of one.
        int steps = (player.activePowerUp && player.activePowerUp->type == PowerUpType::SPEED) ? 2 : 1;
        
        for(int i = 0; i < steps; ++i) {
            // Calculate the next grid position.
            player.position = getNextPosition(player.position, player.direction);
            
            // Handle screen wrapping (teleporting to the opposite side).
            if (player.position.x < 0) player.position.x = GRID_SIZE - 1;
            else if (player.position.x >= GRID_SIZE) player.position.x = 0;
            if (player.position.y < 0) player.position.y = GRID_SIZE - 1;
            else if (player.position.y >= GRID_SIZE) player.position.y = 0;
        }
    }
}

//==================================================================================
// checkPowerUpCollection
// Checks if any player's head has moved onto a power-up's location.
//==================================================================================
void Game::checkPowerUpCollection() {
    for (auto& p : players) {
        auto it = powerUps.begin();
        while (it != powerUps.end()) {
            if (p.position == it->position) { // If a player is on a power-up...
                Player& opponent = players[p.id == 1 ? 1 : 0];
                // Apply the effect based on the power-up type.
                switch (it->type) {
                    case PowerUpType::SPEED: opponent.activePowerUp = ActivePowerUp{it->type, POWERUP_DURATION_SECONDS}; break;
                    case PowerUpType::CONTROL_SWAP: opponent.activePowerUp = ActivePowerUp{it->type, POWERUP_DURATION_SECONDS}; opponent.controlsReversed = true; break;
                    case PowerUpType::TRAIL_BREAKER: p.activePowerUp = ActivePowerUp{it->type, POWERUP_DURATION_SECONDS}; break;
                }
                it = powerUps.erase(it); // Remove the collected power-up from the game.
            } else {
                ++it; // Move to the next power-up.
            }
        }
    }
}

//==================================================================================
// checkCollisions
// The core function that determines if a player has lost the game.
//==================================================================================
void Game::checkCollisions()
{
    std::vector<int> losers; // A list to hold the IDs of players who lost this tick.

    // Loop through each player to check for collisions.
    for (auto& p : players) {
        // Check against every segment of every trail on the board.
        for (const auto& other_p : players) {
            for (const auto& trail_pos : other_p.trail) {
                if (p.position == trail_pos) { // A collision is found.
                    // Check for the 'Trail Breaker' power-up.
                    if (p.activePowerUp && p.activePowerUp->type == PowerUpType::TRAIL_BREAKER) {
                        p.activePowerUp.reset(); // Use up the power-up and survive.
                    } else {
                        losers.push_back(p.id); // No power-up, player loses.
                    }
                }
            }
        }
    }
    
    // Check for a head-on collision (both players on the same square).
    if (players[0].position == players[1].position) { 
        losers.push_back(1); 
        losers.push_back(2); 
    }

    // If the 'losers' list is not empty, the game is over.
    if (!losers.empty()) {
        gameOver = true;
        // Determine the winner.
        if (losers.size() > 1) { // If more than one player lost, it's a draw.
             winner = 0; 
        } else if (losers.front() == 1) { // If player 1 is the only loser...
            winner = 2; // ...player 2 wins.
        } else { // Otherwise, player 2 is the only loser...
            winner = 1; // ...so player 1 wins.
        }
    }
}

//==================================================================================
// spawnPowerUp
// Creates a new power-up at a random, unoccupied location.
//==================================================================================
void Game::spawnPowerUp()
{
    if (powerUps.size() >= MAX_POWERUPS) return; // Don't spawn if max is reached.
    
    Position spawnPos;
    // Keep generating random positions until an empty one is found.
    do { 
        spawnPos = {rand() % GRID_SIZE, rand() % GRID_SIZE}; 
    } while (!isSafe(spawnPos));

    PowerUp newPowerUp;
    newPowerUp.position = spawnPos;
    newPowerUp.id = "p" + std::to_string(time(0));
    // Randomly select one of the three power-up types.
    int type = rand() % 3;
    if (type == 0) newPowerUp.type = PowerUpType::SPEED;
    else if (type == 1) newPowerUp.type = PowerUpType::CONTROL_SWAP;
    else newPowerUp.type = PowerUpType::TRAIL_BREAKER;
    
    powerUps.push_back(newPowerUp);
}

//==================================================================================
// calculateBotMove
// A simple AI for the bot player that prioritizes survival above all else.
//==================================================================================
Direction Game::calculateBotMove(const Player &bot, const Player &otherPlayer)
{
    static const std::map<Direction, Direction> oppositeMap = {{Direction::UP, Direction::DOWN}, {Direction::DOWN, Direction::UP}, {Direction::LEFT, Direction::RIGHT}, {Direction::RIGHT, Direction::LEFT}};
    std::vector<Direction> possibleMoves = {Direction::UP, Direction::DOWN, Direction::LEFT, Direction::RIGHT};
    
    // First, check if continuing straight is a safe move.
    Position nextPos = getNextPosition(bot.position, bot.direction);
    if(isSafe(nextPos)) {
        return bot.direction;
    }

    // If going straight is not safe, check the other possible directions.
    for (Direction move : possibleMoves) {
        if (move == oppositeMap.at(bot.direction)) continue; // Skip 180-degree turns.
        
        nextPos = getNextPosition(bot.position, move);
        if (isSafe(nextPos)) {
            return move; // Return the first safe move found.
        }
    }

    return bot.direction; // If no safe moves are found, continue straight (and lose).
}

//==================================================================================
// isSafe
// Utility function to check if a given grid position is currently unoccupied.
//==================================================================================
bool Game::isSafe(const Position &pos)
{
    Position testPos = pos;
    // Account for screen wrapping in the check.
    if (testPos.x < 0) testPos.x = GRID_SIZE - 1; else if (testPos.x >= GRID_SIZE) testPos.x = 0;
    if (testPos.y < 0) testPos.y = GRID_SIZE - 1; else if (testPos.y >= GRID_SIZE) testPos.y = 0;

    // Check against all trail segments.
    for (const auto &p : players) {
        for (const auto &trailPos : p.trail) if (testPos == trailPos) return false;
        // Also check against the other player's head.
        if(testPos == p.position) return false;
    }
    return true; // The position is safe.
}

//==================================================================================
// getNextPosition
// A pure utility function to calculate a new position based on a direction.
//==================================================================================
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

//==================================================================================
// draw
// Renders all visible game elements to the specified render target.
//==================================================================================
void Game::draw(sf::RenderTarget &target)
{
    // Draw a border around the play area.
    float thickness = 3.0f;
    sf::RectangleShape border;
    border.setSize(sf::Vector2f(GAME_WIDTH - thickness * 2, GAME_HEIGHT - thickness * 2));
    border.setPosition(thickness, thickness);
    border.setFillColor(sf::Color::Transparent);
    border.setOutlineColor(sf::Color(100, 100, 100));
    border.setOutlineThickness(thickness);
    target.draw(border);

    float cellSize = GAME_WIDTH / GRID_SIZE;

    // Draw all player trails.
    for (const auto &player : players) {
        for (const auto &trailPos : player.trail) {
            sf::RectangleShape trailSegment(sf::Vector2f(cellSize, cellSize));
            trailSegment.setPosition(trailPos.x * cellSize, trailPos.y * cellSize);
            trailSegment.setFillColor(player.trailColor);
            target.draw(trailSegment);
        }
    }

    // Draw all power-ups.
    for (const auto &powerUp : powerUps) {
        sf::Color color;
        if (powerUp.type == PowerUpType::SPEED) color = sf::Color::Yellow;
        else if (powerUp.type == PowerUpType::CONTROL_SWAP) color = sf::Color::Green;
        else color = sf::Color(138, 43, 226); // Purple

        // Draw a semi-transparent glow effect behind the power-up.
        sf::CircleShape glow(cellSize * 0.8f);
        glow.setOrigin(glow.getRadius(), glow.getRadius());
        glow.setPosition(powerUp.position.x * cellSize + cellSize / 2.f, powerUp.position.y * cellSize + cellSize / 2.f);
        glow.setFillColor(sf::Color(color.r, color.g, color.b, 80));
        target.draw(glow);

        // Draw the solid power-up icon.
        sf::CircleShape powerUpShape(cellSize / 2.0f);
        powerUpShape.setPosition(powerUp.position.x * cellSize, powerUp.position.y * cellSize);
        powerUpShape.setFillColor(color);
        target.draw(powerUpShape);
    }

    // Draw all player heads on top of the trails.
    for (const auto &player : players) {
        // Draw a glow effect for the head.
        sf::RectangleShape glow(sf::Vector2f(cellSize, cellSize));
        glow.setScale(1.5f, 1.5f);
        glow.setOrigin(glow.getSize() / 2.f);
        glow.setPosition(player.position.x * cellSize + cellSize / 2.f, player.position.y * cellSize + cellSize / 2.f);
        glow.setFillColor(sf::Color(player.headColor.r, player.headColor.g, player.headColor.b, 100));
        target.draw(glow);

        // Draw the solid player head.
        sf::RectangleShape playerHead(sf::Vector2f(cellSize, cellSize));
        playerHead.setPosition(player.position.x * cellSize, player.position.y * cellSize);
        playerHead.setFillColor(player.headColor);
        target.draw(playerHead);
    }
}

//==================================================================================
// Getters
// Public functions that allow other parts of the program (like main.cpp) to
// read the state of the game.
//==================================================================================
bool Game::isGameOver() const { return gameOver; }
int Game::getWinner() const { return winner; }