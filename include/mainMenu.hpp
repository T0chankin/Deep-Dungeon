#pragma once
#include <SFML/Graphics.hpp>
#include "audioManager.hpp"
#include <vector>
#include <string>

enum class MenuResult { 
    None,
    NewGame,
    LoadGame,
    Options,
    Exit };

class MainMenu {
public:
    MainMenu(sf::RenderWindow& window, sf::Font& font, AudioManager& audio);
    MenuResult handleEvent(const sf::Event& event);
    void draw();

private:
    sf::RenderWindow& window;
    sf::Font& font;
    AudioManager& audio;

    std::vector<sf::Text> items;
    int selected = 0;

    sf::Text title;
    sf::RectangleShape bg;

    void updateColors();
};