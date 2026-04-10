#pragma once
#include <SFML/Graphics.hpp>

enum class ScoreType {
    Win,
    Death
};

class ScoreScreen {
public:
    ScoreScreen(sf::RenderWindow& window, sf::Font& font);

    void show(ScoreType type, int coins, int floor);
    void handleEvent(const sf::Event& event, bool& goMenu);
    void draw();

private:
    sf::RenderWindow& window;
    sf::Font& font;

    sf::RectangleShape bg;
    sf::Text titleText;
    sf::Text coinsText;
    sf::Text floorText;
    sf::Text hintText;
};