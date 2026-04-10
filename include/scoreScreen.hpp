#pragma once
#include <SFML/Graphics.hpp>

class ScoreScreen {
public:
    ScoreScreen(sf::RenderWindow& window, sf::Font& font);

    void setCoins(int coins);
    void handleEvent(const sf::Event& event, bool& goMenu);
    void draw();

private:
    sf::RenderWindow& window;
    sf::Font& font;

    sf::RectangleShape bg;
    sf::Text titleText;
    sf::Text coinsText;
    sf::Text hintText;

    int coins = 0;
};