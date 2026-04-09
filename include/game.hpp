#pragma once
#include <SFML/Graphics.hpp>
#include "mainMenu.hpp"
#include "options.hpp"

enum class GameState {
    Menu,
    Options,
    Playing
};

class Game {
public:
    Game();
    void run();

private:
    void handleEvents();
    void update(float dt);
    void render();

    sf::RenderWindow window;
    sf::Font font;
    GameState state;
    MainMenu* menu;
    Options* options;
    AudioManager* audio;
};