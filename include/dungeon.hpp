#pragma once
#include <SFML/Graphics.hpp>
#include "player.hpp"
#include "mapGenerator.hpp"

class Dungeon {
public:
    Dungeon(sf::RenderWindow& window, sf::Font& font);

    void handleEvent(const sf::Event& event);
    void update(float dt);
    void draw();

    bool isFinished() const;
    int getCoins() const;

private:
    sf::RenderWindow& window;
    sf::Font& font;

    Player player;
    MapGenerator mapGen;

    sf::View camera;
    float cameraDeadZone = 80.f;
    float cameraSpeed=8.f;

    int currentFloor = 1;
    bool finished = false;

    void handleInput(float dt);
    void updateCamera(float dt);
    void checkHatch();
    void nextFloor();
    void resolveCollision(sf::Vector2f& pos);
};