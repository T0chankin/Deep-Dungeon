#pragma once
#include <SFML/Graphics.hpp>
#include "mainMenu.hpp"
#include "options.hpp"
#include "audioManager.hpp"
#include "dungeon.hpp"
#include "scoreScreen.hpp"

enum class GameState { Menu, Options, Playing, Score };

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

    AudioManager*  audio;
    MainMenu*      menu;
    Options*       options;
    Dungeon*       dungeon;
    ScoreScreen*   scoreScreen;
};