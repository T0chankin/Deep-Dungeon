#pragma once
#include <SFML/Graphics.hpp>
#include "player.hpp"

class Hud {
public:
    Hud(sf::RenderWindow& window, sf::Font& font);
    void update(Player& player);
    void draw();

private:
    sf::RenderWindow& window;
    sf::Font& font;

    sf::RectangleShape bg;

    sf::RectangleShape hpBarBg;
    sf::RectangleShape hpBar;
    sf::RectangleShape manaBarBg;
    sf::RectangleShape manaBar;

    sf::Text hpText;
    sf::Text manaText;
    sf::Text coinsText;
    sf::Text weaponText;
    sf::Text hintsText;

    float hudHeight = 80.f;
};