#pragma once
#include <SFML/Graphics.hpp>
#include "player.hpp"
#include "mapGenerator.hpp"
#include "pathfinder.hpp"

enum class MonsterState { Idle, Chase, Attack };

class Monster {
public:
    Monster(sf::Vector2f pos, MonsterType type, int floorNumber, sf::Font& font);
    ~Monster();
    void update(float dt, Player& player, MapGenerator& map);
    void draw(sf::RenderWindow& window);

    void takeDamage(float amount);
    bool isDead() const;

    sf::Vector2f getPos() const;
    sf::FloatRect getBounds() const;
    bool justDied() const;

private:
    sf::RectangleShape shape;
    MonsterType type;
    MonsterState state;

    float hp;
    float maxHp;
    float speed;
    float attackDamage;
    float attackRange;
    float attackTimer;
    float attackCooldown;
    float aggroRange;

    sf::Font* font = nullptr;

    sf::Texture texture;
    sf::Sprite* sprite = nullptr;
    bool hasSprite = false;

    std::vector<sf::Vector2i> path;
    float pathTimer    = 0.f;
    float pathCooldown = 0.5f;

    struct Bullet {
        sf::CircleShape shape;
        sf::Vector2f direction;
        float speed;
        bool active = true;
    };
    std::vector<Bullet> bullets;

    void updateMelee(float dt, Player& player, MapGenerator& map);
    void updateRanged(float dt, Player& player, MapGenerator& map);
    void updateBullets(float dt, Player& player, MapGenerator& map);
    void shoot(sf::Vector2f direction);
    void resolveCollision(sf::Vector2f& pos, MapGenerator& map, float half);

    sf::Vector2f moveAlongPath(sf::Vector2f myPos, float spd, float dt, MapGenerator& map, float half);
    bool deathHandled = false;
};