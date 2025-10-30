#pragma once
#include <SFML/System/Vector2.hpp>
#include <list>
#include <string>
#include <optional>

enum class GameState
{
    MENU,
    PLAYING,
    GAME_OVER,
    HOW_TO_PLAY
};

enum class GameMode
{
    SINGLE,
    TWO_PLAYER
};

enum class Direction
{
    UP,
    DOWN,
    LEFT,
    RIGHT
};

struct Position
{
    int x;
    int y;

    bool operator==(const Position& other) const {
        return x == other.x && y == other.y;
    }
};

enum class PowerUpType
{
    SPEED,
    CONTROL_SWAP,
    TRAIL_BREAKER
};

struct ActivePowerUp {
    PowerUpType type;
    float duration; // In seconds
};

struct PowerUp
{
    std::string id;
    PowerUpType type;
    Position position;
};

struct Player
{
    int id;
    Position position;
    Direction direction;
    Direction intendedDirection;
    std::list<Position> trail;
    sf::Color headColor;    // Color for the cycle's head
    sf::Color trailColor;   // Color for the cycle's trail
    bool isBot;
    std::optional<ActivePowerUp> activePowerUp;
    bool controlsReversed;
};