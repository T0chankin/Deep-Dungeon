#pragma once
#include <SFML/Graphics.hpp>
#include "player.hpp"

class Dungeon {
public:
    Dungeon(sf::RenderWindow& window, sf::Font& font);

    void handleEvent(const sf::Event& event);
    void update(float dt);
    void draw();

private:
    sf::RenderWindow& window;
    sf::Font& font;

    Player player;

    sf::View camera;
    float cameraDeadZone = 100.f;
    float cameraSpeed    = 5.f;

    void updateCamera(float dt);
    void handleInput(float dt);

    void drawTestSquare();
};