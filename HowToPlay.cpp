#include "HowToPlay.hpp"

void centerOriginH2P(sf::Text &text) {
    sf::FloatRect bounds = text.getLocalBounds();
    text.setOrigin(bounds.left + bounds.width / 2.0f, bounds.top + bounds.height / 2.0f);
}

HowToPlay::HowToPlay(sf::Font &font, float width, float height) : font(font)
{
    float centerX = width / 2.0f;

    controlsTitle.setFont(font);
    controlsTitle.setString("Controls");
    controlsTitle.setCharacterSize(32);
    controlsTitle.setFillColor(sf::Color::White);
    centerOriginH2P(controlsTitle);
    controlsTitle.setPosition(centerX, 80);

    float p1_centerX = centerX - 220;
    player1Label.setFont(font);
    player1Label.setString("Player 1");
    player1Label.setCharacterSize(24);
    player1Label.setFillColor(sf::Color::Red);
    centerOriginH2P(player1Label);
    player1Label.setPosition(p1_centerX, 150);

    createKey("W", sf::Vector2f(p1_centerX, 200));
    createKey("A", sf::Vector2f(p1_centerX - 50, 250));
    createKey("S", sf::Vector2f(p1_centerX, 250));
    createKey("D", sf::Vector2f(p1_centerX + 50, 250));

    float p2_centerX = centerX + 220;
    player2Label.setFont(font);
    player2Label.setString("Player 2");
    player2Label.setCharacterSize(24);
    player2Label.setFillColor(sf::Color::Blue);
    centerOriginH2P(player2Label);
    player2Label.setPosition(p2_centerX, 150);

    createKey(sf::String(L"↑"), sf::Vector2f(p2_centerX, 200));
    createKey(sf::String(L"←"), sf::Vector2f(p2_centerX - 50, 250));
    createKey(sf::String(L"↓"), sf::Vector2f(p2_centerX, 250));
    createKey(sf::String(L"→"), sf::Vector2f(p2_centerX + 50, 250));

    separator.setSize(sf::Vector2f(width - 150, 2));
    separator.setFillColor(sf::Color(100, 100, 100));
    separator.setOrigin(separator.getSize().x / 2.0f, separator.getSize().y / 2.0f);
    separator.setPosition(centerX, 350);

    powerUpsLabel.setFont(font);
    powerUpsLabel.setString("Power-Ups");
    powerUpsLabel.setCharacterSize(32);
    powerUpsLabel.setFillColor(sf::Color::White);
    centerOriginH2P(powerUpsLabel);
    powerUpsLabel.setPosition(centerX, 420);

    createPowerUp("Speed Trap:", "Force your opponent into temporary speed boost.", sf::Color::Yellow, 500);
    createPowerUp("Trail Breaker:", "Safely pass through and destroy any trail.", sf::Color(138, 43, 226), 570);
    createPowerUp("Control Swap:", "Reverses your opponent movement.", sf::Color::Green, 640);

    mainMenuButton.setFont(font);
    mainMenuButton.setString("Main Menu");
    mainMenuButton.setCharacterSize(24);
    mainMenuButton.setFillColor(sf::Color(200, 200, 200));
    centerOriginH2P(mainMenuButton);
    mainMenuButton.setPosition(centerX, height - 80);
}

void HowToPlay::createKey(const sf::String& text, sf::Vector2f position) {
    sf::RectangleShape key(sf::Vector2f(50, 50));
    key.setOrigin(key.getSize() / 2.f);
    key.setFillColor(sf::Color(20, 20, 20));
    key.setOutlineColor(sf::Color(80, 80, 80));
    key.setOutlineThickness(2);
    key.setPosition(position);
    keyShapes.push_back(key);

    sf::Text keyText;
    keyText.setFont(font);
    keyText.setString(text);
    keyText.setCharacterSize(20);
    keyText.setFillColor(sf::Color::White);
    centerOriginH2P(keyText);
    keyText.setPosition(position);
    keyTexts.push_back(keyText);
}

void HowToPlay::createPowerUp(const std::string& name, const std::string& description, sf::Color color, float yPos) {
    float startX = 60;
    float textY = yPos - 12;

    sf::CircleShape icon(15);
    icon.setOrigin(icon.getRadius(), icon.getRadius());
    icon.setFillColor(color);
    icon.setPosition(startX, yPos);
    powerUpIcons.push_back(icon);
    
    sf::Text nameText(name, font, 20);
    nameText.setFillColor(color);
    nameText.setPosition(startX + 30, textY);
    powerUpTexts.push_back(nameText);

    sf::Text descText(description, font, 20);
    descText.setFillColor(sf::Color::White);
    descText.setPosition(nameText.getPosition().x + nameText.getGlobalBounds().width + 10, textY);
    powerUpTexts.push_back(descText);
}

void HowToPlay::draw(sf::RenderTarget &target)
{
    target.draw(controlsTitle);
    target.draw(player1Label);
    target.draw(player2Label);
    for(const auto& shape : keyShapes) target.draw(shape);
    for(const auto& text : keyTexts) target.draw(text);
    target.draw(separator);
    target.draw(powerUpsLabel);
    for(const auto& icon : powerUpIcons) target.draw(icon);
    for(const auto& text : powerUpTexts) target.draw(text);
    target.draw(mainMenuButton);
}

void HowToPlay::handleInput(const sf::Event& event, sf::Vector2f mousePos) {
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
        closeScreen = true;
    }
    
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        if (mainMenuButton.getGlobalBounds().contains(mousePos)) {
            closeScreen = true;
        }
    }
}

bool HowToPlay::shouldClose() const { return closeScreen; }

void HowToPlay::reset() {
    closeScreen = false;
}