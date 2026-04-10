#pragma once
#include <SFML/Graphics.hpp>
#include "player.hpp"
#include "mapGenerator.hpp"
#include "monster.hpp"
#include "hud.hpp"
#include "audioManager.hpp"
#include <deque>


class Dungeon {
public:
    Dungeon(sf::RenderWindow& window, sf::Font& font, AudioManager& audio);

    void handleEvent(const sf::Event& event);
    void update(float dt);
    void draw();

    bool isFinished() const;
    int getCoins() const;

    int getCurrentFloor() const;

    bool isPlayerDead() const;
private:
    sf::RenderWindow& window;
    sf::Font& font;

    Player player;
    MapGenerator mapGen;

    std::deque<Monster> monsters;
    void spawnMonsters();

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

    void handleAttack();
    float attackVisualTimer = 0.f;
    sf::ConvexShape attackShape;
    void drawAttackVisual();

    void handleProjectiles(float dt);
    
    Hud hud;
    void usePotion(ItemType type);

    struct WorldItem {
        sf::CircleShape shape;
        ItemSpawnType type;
        bool collected = false;
    };

    std::vector<WorldItem> worldItems;

    void spawnItems();
    void checkItemPickup();
    AudioManager& audio;
};