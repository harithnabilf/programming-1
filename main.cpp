#include <SFML/Graphics.hpp>
#include <vector>                                   //to store all info of a player (including coordinates)
#include <list>                                     //to store coordinates only
#include <string>
#include <iostream>                                 // External library (col 1) ; STL (col 2-9)
#include <cstdlib>
#include <ctime>
#include <map>

// 2) DECLARE VARIABLES 
const float GAME_WIDTH = 800.0f;
const float GAME_HEIGHT = 800.0f;

enum class GameState { PLAYING, GAME_OVER };
enum class Direction { UP, DOWN, LEFT, RIGHT };   // Define/customize inputs for Key

struct Position {
    int x;
    int y;
    bool operator==(const Position &other) const {
        return x == other.x && y == other.y;
    }
};

struct Player {
    int id;
    Position position;
    Direction direction;
    Direction intendedDirection;
    std::list<Position> trail;           // 1D array to store x,y coordinates 
    sf::Color headColor;
    sf::Color trailColor;
};

//User-defined functions

// 2) Graphics part(Text) to assign/define Displayed Text when game ends
class GameOver {
    public:
        GameOver(sf::Font &font, float width, float height) : font(font) {
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

            exitText.setFont(font);
            exitText.setString("Exit");
            exitText.setCharacterSize(30);
            exitText.setFillColor(sf::Color::White);

            sf::FloatRect goRect = gameOverText.getLocalBounds();
            gameOverText.setOrigin(goRect.left + goRect.width / 2.0f, goRect.top + goRect.height / 2.0f);
            gameOverText.setPosition(width / 2.0f, height / 3.0f);

            sf::FloatRect rtRect = restartText.getLocalBounds();
            restartText.setOrigin(rtRect.left + rtRect.width / 2.0f, rtRect.top + rtRect.height / 2.0f);
            restartText.setPosition(width / 2.0f, height * 2.0f / 3.0f);

            sf::FloatRect exRect = exitText.getLocalBounds();
            exitText.setOrigin(exRect.left + exRect.width / 2.0f, exRect.top + exRect.height / 2.0f);
            exitText.setPosition(width / 2.0f, restartText.getPosition().y + 60);
        }

        // 2) User-defined functions : Graphics to assign/define winner Text for 9)
        void setWinner(int winnerId) {
        //SELECTION BODY , define required result
            if (winnerId == 0) {
                winnerText.setString("Draw!");
                winnerText.setFillColor(sf::Color::White);
            } else {
                winnerText.setString("Player " + std::to_string(winnerId) + " Wins!");
                winnerText.setFillColor(winnerId == 1 ? sf::Color(252, 165, 165) : sf::Color(147, 197, 253));
            }
        //Graphics part
            sf::FloatRect wtRect = winnerText.getLocalBounds();
            winnerText.setOrigin(wtRect.left + wtRect.width / 2.0f, wtRect.top + wtRect.height / 2.0f);
            winnerText.setPosition(gameOverText.getPosition().x, gameOverText.getPosition().y + 100);
        }
        //User-defined functions: Graphics part
        void draw(sf::RenderTarget &target) {
            target.draw(gameOverText);
            target.draw(winnerText);
            target.draw(restartText);
            target.draw(exitText);
        }

    
        // User-defined Function: to detect button pressed by user (mouse)
        void handleInput(const sf::Event& event, sf::Vector2f mousePos) {
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                if (restartText.getGlobalBounds().contains(mousePos)) {
                    restartClicked = true;
                }
                else if (exitText.getGlobalBounds().contains(mousePos)) {
                    exitClicked = true;
                }
            }
        }

        // User-defined functions: Assign (bool) value (continue/end) after game ends
        bool isRestartClicked() const { 
            return restartClicked; 
        }

        bool isExitClicked() const { 
            return exitClicked; 
        }

        //User-defined functions: Reinitialise value (continue / end) after game ends
        void reset() {
            restartClicked = false;
            exitClicked = false;
        }

    private:
        sf::Font &font;
        sf::Text gameOverText;
        sf::Text winnerText;
        sf::Text restartText;
        sf::Text exitText;
        bool restartClicked = false;
        bool exitClicked = false;
};



class Game {
public:
    Game(sf::Font &font) : font(font) { 
        srand(time(0)); 
    }
    
    // 3) Assign/Initialise value 
    void start() {
        initializePlayers();
        gameOver = false;
        winner = 0;
        tickTimer = 0.0f;
        clock.restart();
    }


    // 4) READ INPUT (Key pressed)  6) SELECTION BODY 
    void handleInput() {
        // Player 1 Input (by WASD Key)
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)){
            players[0].intendedDirection = Direction::UP;
        }   
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)){
            players[0].intendedDirection = Direction::DOWN;
        } 
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)){
            players[0].intendedDirection = Direction::LEFT;
        }       
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)){
            players[0].intendedDirection = Direction::RIGHT;
        } 
            
        // Player 2 Input (by Arrows)
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            players[1].intendedDirection = Direction::UP;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            players[1].intendedDirection = Direction::DOWN;
        } 
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            players[1].intendedDirection = Direction::LEFT;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            players[1].intendedDirection = Direction::RIGHT;
        }
    }

private:
    // 6) SELECTION BODY for Output based on key pressed by respective players (Calculate next position)
    Position getNextPosition(const Position &position, Direction direction) const {
        Position nextPos = position;
        
        // Attempt to Use pointers for x and y coordinates
        int* ptrX = &nextPos.x;
        int* ptrY = &nextPos.y;
        
        switch (direction) {
            case Direction::UP : 
                *ptrY = *ptrY - 1; 
                break;
            case Direction::DOWN : 
                *ptrY = *ptrY + 1; 
                break;
            case Direction::LEFT : 
                *ptrX = *ptrX - 1; 
                break;
            case Direction::RIGHT : 
                *ptrX = *ptrX + 1; 
                break;
        }
        return nextPos;
    }

    //To avoid invalid movement: change opposite direction on same line (No overlapping of trail of player itself)
    void updatePlayerDirections() {
        static const std::map<Direction, Direction> oppositeMap = {
            {Direction::UP, Direction::DOWN}, {Direction::DOWN, Direction::UP}, 
            {Direction::LEFT, Direction::RIGHT}, {Direction::RIGHT, Direction::LEFT}
        };

        //only valid movement is considered
        for (auto& p : players) {
            if (oppositeMap.at(p.direction) != p.intendedDirection) {
                p.direction = p.intendedDirection;
            }
        }
    }

// 3) ASSIGN/STORE initial position value to trails array
    void initializePlayers() {
        players.clear();
        
        Player p1;
        p1.id = 1; 
        int* p1x = &p1.position.x;
        int* p1y = &p1.position.y;
        *p1x = GRID_SIZE / 4;       
        *p1y = GRID_SIZE / 2;       
        p1.direction = Direction::RIGHT; p1.intendedDirection = Direction::RIGHT;
        p1.headColor = sf::Color(252, 165, 165); p1.trailColor = sf::Color(153, 27, 27);
        players.push_back(p1);

        Player p2;
        p2.id = 2; 
        int* p2x = &p2.position.x;
        int* p2y = &p2.position.y;
        *p2x = GRID_SIZE * 3 / 4;    
        *p2y = GRID_SIZE / 2;       
        p2.direction = Direction::LEFT; p2.intendedDirection = Direction::LEFT;
        p2.headColor = sf::Color(147, 197, 253); p2.trailColor = sf::Color(30, 58, 138);
        players.push_back(p2);
    }

    //User-defined Functions : 7) ASSIGN/STORE current position value to trails array
    void movePlayers() {
        for (auto &player : players) {
            player.trail.push_front(player.position);
            player.position = getNextPosition(player.position, player.direction);
            
            
            int* ptrX = &player.position.x;
            int* ptrY = &player.position.y;
            
            //Recalculate position by considering the case when trails exceed screen size
            if (*ptrX < 0) {
                *ptrX = GRID_SIZE - 1;
            }
            else if (*ptrX >= GRID_SIZE) {
                *ptrX = 0;
            }

            if (*ptrY < 0) {
                *ptrY = GRID_SIZE - 1;
            }
            else if (*ptrY >= GRID_SIZE) {
                *ptrY = 0;
            }
        }
    }
    

    //User-defined Functions: 8) Detect Collision
    void checkCollisions() {
        std::vector<int> losers;
        for (auto& p : players) {
            for (const auto& other_p : players) {
                for (const auto& trail_pos : other_p.trail) {
                    if (p.position == trail_pos) {
                        losers.push_back(p.id);
                    }
                }
            }
        }
     
    // 9) SELECTION BODY for output after Collision
        if (players[0].position == players[1].position) { 
            losers.push_back(1); 
            losers.push_back(2); 
        }
        
        if (!losers.empty()) {
            gameOver = true;
            if (losers.size() > 1 || (losers.front() == 1 && losers.back() == 2)) {
                winner = 0;       // Draw
            }                             
            else if (losers.front() == 1) {
                winner = 2;       // Player 1 crashed, Player 2 wins
            }
            else {
                winner = 1;       // Player 2 crashed, Player 1 wins
            }               
        }
    }

public:
    //Graphics part, 6) Control Default Movement, 8)DETECT COLLISION on time 
    void update() {
        if (gameOver) {
            return;
        }
           
        float dt = clock.restart().asSeconds();
        tickTimer += dt;
        
        if (tickTimer >= TICK_RATE) {
            tickTimer -= TICK_RATE;
            updatePlayerDirections();
            movePlayers();
            checkCollisions();
        }
    }

    //Graphics part 
    void draw(sf::RenderTarget &target) {
    //Window graphics
        float thickness = 3.0f;
        sf::RectangleShape border(sf::Vector2f(GAME_WIDTH - thickness * 2, GAME_HEIGHT - thickness * 2));
        border.setPosition(thickness, thickness);
        border.setFillColor(sf::Color::Transparent);
        border.setOutlineColor(sf::Color(100, 100, 100));
        border.setOutlineThickness(thickness);
        target.draw(border);

        float cellSize = GAME_WIDTH / GRID_SIZE;

    // Trails Graphics
        for (const auto &player : players) {
            for (const auto &trailPos : player.trail) {
                sf::RectangleShape trailSegment(sf::Vector2f(cellSize, cellSize));
                trailSegment.setPosition(trailPos.x * cellSize, trailPos.y * cellSize);
                trailSegment.setFillColor(player.trailColor);
                target.draw(trailSegment);
            }
        }
    //Trails graphics
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

    bool isGameOver() const { return gameOver; }
    int getWinner() const { return winner; }

private:
    //Graphics part(Timer)
    sf::Font &font;
    std::vector<Player> players;
    bool gameOver = false;
    int winner = 0;
    sf::Clock clock;

    const int GRID_SIZE = 40;
    const float TICK_RATE = 0.08f;
    float tickTimer = 0.0f;
};

//Main function

int main()
{
    
    // Graphics part  (display window)
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Neon Cycle Duel", sf::Style::Fullscreen);
    window.setFramerateLimit(60);
    window.setMouseCursorVisible(true);
   
    sf::RenderTexture gameTexture;
    
    if (!gameTexture.create(GAME_WIDTH, GAME_HEIGHT)) {
        return -1;
    }

    sf::Sprite gameSprite(gameTexture.getTexture());

    //Graphics:Window Scaling
    float windowWidth = static_cast<float>(window.getSize().x);
    float windowHeight = static_cast<float>(window.getSize().y);
    float windowRatio = windowWidth / windowHeight;
    float gameRatio = GAME_WIDTH / GAME_HEIGHT;
    float scale = 1.0f;
    
    sf::Vector2f position(0, 0);
    float* ptrPosX = &position.x;
    float* ptrPosY = &position.y;

    if (windowRatio > gameRatio) {
        scale = windowHeight / GAME_HEIGHT;
        *ptrPosX = (windowWidth - (GAME_WIDTH * scale)) / 2.0f;
    } 
    else {
        scale = windowWidth / GAME_WIDTH;
        *ptrPosY = (windowHeight - (GAME_HEIGHT * scale)) / 2.0f;
    }
    gameSprite.setScale(scale, scale);
    gameSprite.setPosition(position);

    // Graphics: Initialise Text Font
    sf::Font font;
    if (!font.loadFromFile("pixel_font.ttf")) {
        std::cerr << "Error: Could not load pixel_font.ttf" << std::endl;
        return -1;
    }
    
    // Initialise for Windows and text 
    GameState gameState = GameState::PLAYING; 
    Game game(font);
    GameOver gameOver(font, GAME_WIDTH, GAME_HEIGHT);
    
    game.start(); 
    
    // 5) LOOP BODY (Game program)
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            //Exit program
            if (event.type == sf::Event::Closed) {
                window.close();
            }             
            // Exit program by pressing 'Esc'   6) SELECTION BODY
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                window.close(); 
            }

            // Enable mouse controlling
            sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
            sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);
            
            float* wPosX = &worldPos.x;
            float* wPosY = &worldPos.y;
            
            *wPosX = (*wPosX - gameSprite.getPosition().x) / gameSprite.getScale().x;
            *wPosY = (*wPosY - gameSprite.getPosition().y) / gameSprite.getScale().y;
            
            if (gameState == GameState::GAME_OVER) {
                gameOver.handleInput(event, worldPos);
            }
        }

         // 9) SELECTION BODY to display Winner text (required result) with graphics
        if (gameState == GameState::PLAYING && game.isGameOver()) {
            gameState = GameState::GAME_OVER;
            gameOver.setWinner(game.getWinner());
        }

        // 6) SELECTION BODY continue or exit program  (return 0)
        if (gameState == GameState::GAME_OVER) {
            if (gameOver.isRestartClicked()) {
                gameState = GameState::PLAYING;
                game.start();
                gameOver.reset();
            } 
            else if (gameOver.isExitClicked()) {
                window.close();
            }
        }
        // Maintaining game logic during playing (continue)
        if (gameState == GameState::PLAYING) {
            game.handleInput(); 
            game.update();
        }


        // Graphics part
        gameTexture.clear(sf::Color::Black);
        
        if (gameState == GameState::PLAYING) {
            game.draw(gameTexture);
        } 
        else if (gameState == GameState::GAME_OVER) {
            game.draw(gameTexture); 
            gameOver.draw(gameTexture);
        }
        gameTexture.display();
        window.clear(sf::Color::Black);
        window.draw(gameSprite);
        window.display();
    }
    
    return 0;
}