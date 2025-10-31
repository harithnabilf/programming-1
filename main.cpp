#include <SFML/Graphics.hpp>
#include "types.hpp"
#include "MainMenu.hpp"
#include "Game.hpp"
#include "GameOver.hpp"
#include "HowToPlay.hpp"

// Define the fixed resolution of your game. All drawing will happen on a texture of this size.
const float GAME_WIDTH = 800.0f;
const float GAME_HEIGHT = 800.0f;

int main()
{
    // --- NEW RENDER-TO-TEXTURE STRATEGY ---

    // 1. Create the final display window in fullscreen.
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Photron", sf::Style::Fullscreen);
    window.setFramerateLimit(60);
    window.setMouseCursorVisible(true); // Ensure cursor is visible

    // 2. Create an off-screen texture that will act as our "game screen".
    // All game objects will be drawn to this texture, not the window.
    sf::RenderTexture gameTexture;
    if (!gameTexture.create(GAME_WIDTH, GAME_HEIGHT)) {
        return -1; // Error
    }

    // 3. Create a sprite to display the contents of our game texture.
    sf::Sprite gameSprite(gameTexture.getTexture());

    // 4. Calculate scaling and positioning to center the gameSprite on the window.
    // This is done once and handles the letterboxing.
    float windowWidth = static_cast<float>(window.getSize().x);
    float windowHeight = static_cast<float>(window.getSize().y);
    float windowRatio = windowWidth / windowHeight;
    float gameRatio = GAME_WIDTH / GAME_HEIGHT;
    float scale = 1.0f;
    sf::Vector2f position(0, 0);

    if (windowRatio > gameRatio) {
        // Window is wider than game (letterbox on sides)
        scale = windowHeight / GAME_HEIGHT;
        position.x = (windowWidth - (GAME_WIDTH * scale)) / 2.0f;
    } else {
        // Window is taller than game (letterbox on top/bottom)
        scale = windowWidth / GAME_WIDTH;
        position.y = (windowHeight - (GAME_HEIGHT * scale)) / 2.0f;
    }
    gameSprite.setScale(scale, scale);
    gameSprite.setPosition(position);

    // Load the font.
    sf::Font font;
    if (!font.loadFromFile("pixel_font.ttf")) return -1;
    
    // Initialize all game states.
    GameState gameState = GameState::MENU;
    GameMode gameMode = GameMode::SINGLE;
    int winner = 0;
    MainMenu mainMenu(GAME_WIDTH, GAME_HEIGHT, font);
    Game game(font);
    GameOver gameOver(font, GAME_WIDTH, GAME_HEIGHT);
    HowToPlay howToPlay(font, GAME_WIDTH, GAME_HEIGHT);
    
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed) window.close();

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                window.close();
            }

            // --- MOUSE COORDINATE FIX ---
            // We must now convert window mouse coordinates to game texture coordinates.
            sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
            sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos); // This gives coords relative to the window
            
            // Now, transform them to be relative to our centered gameSprite
            worldPos.x = (worldPos.x - gameSprite.getPosition().x) / gameSprite.getScale().x;
            worldPos.y = (worldPos.y - gameSprite.getPosition().y) / gameSprite.getScale().y;
            
            // Pass the corrected coordinates and the original event to the input handlers.
            if (gameState == GameState::MENU) {
                mainMenu.handleInput(event, worldPos);
            } else if (gameState == GameState::GAME_OVER) {
                gameOver.handleInput(event, worldPos);
            } else if (gameState == GameState::HOW_TO_PLAY) {
                howToPlay.handleInput(event, worldPos);
            }
        }

        // --- State Transitions ---
        if (gameState == GameState::MENU) {
            if (mainMenu.isSinglePlayerClicked()) {
                gameState = GameState::PLAYING;
                gameMode = GameMode::SINGLE;
                game.start(gameMode);
            } else if (mainMenu.isTwoPlayerClicked()) {
                gameState = GameState::PLAYING;
                gameMode = GameMode::TWO_PLAYER;
                game.start(gameMode);
            } else if (mainMenu.isHowToPlayClicked()) {
                gameState = GameState::HOW_TO_PLAY;
            }
            mainMenu.reset();
        }

        if (gameState == GameState::PLAYING && game.isGameOver()) {
            gameState = GameState::GAME_OVER;
            winner = game.getWinner();
            gameOver.setWinner(winner, gameMode);
        }

        if (gameState == GameState::GAME_OVER) {
            if (gameOver.isRestartClicked()) {
                gameState = GameState::PLAYING;
                game.start(gameMode);
            } else if (gameOver.isMainMenuClicked()) {
                gameState = GameState::MENU;
            }
            gameOver.reset();
        }
        
        if (gameState == GameState::HOW_TO_PLAY && howToPlay.shouldClose()) {
            gameState = GameState::MENU;
            howToPlay.reset();
        }

        // --- Drawing Logic ---

        // 1. Update game logic (for PLAYING state)
        if (gameState == GameState::PLAYING) {
            game.handleInput(); // Keyboard input doesn't need coordinate mapping
            game.update();
        }

        // 2. Draw everything to the off-screen texture
        gameTexture.clear(sf::Color::Black);
        if (gameState == GameState::MENU) {
            mainMenu.draw(gameTexture);
        } else if (gameState == GameState::PLAYING) {
            game.draw(gameTexture);
        } else if (gameState == GameState::GAME_OVER) {
            gameOver.draw(gameTexture);
        } else if (gameState == GameState::HOW_TO_PLAY) {
            howToPlay.draw(gameTexture);
        }
        gameTexture.display(); // Finalize the texture

        // 3. Draw the final texture sprite to the main window
        window.clear(sf::Color::Black); // Clear window to create black bars
        window.draw(gameSprite);
        window.display();
    }
    return 0;
}