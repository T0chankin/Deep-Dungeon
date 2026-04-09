#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

enum class ItemType {
    HealthPotion,
    ManaPotion
};

struct Item {
    std::string name;
    ItemType type;
    float value;
};

enum class WeaponType {
    Fireball,
    Bow,
    Sword,
    Spear
};

struct Weapon {
    std::string name;
    WeaponType type;
    int damage;
    float attackSpeed;
    float manaCost;
    float range;
    float angle;
};

class Player {
public:
    Player();
    ~Player();

    void update(float dt);
    void draw(sf::RenderWindow& window);

    void move(sf::Vector2f direction);
    void doDash();

    void attack(sf::Vector2f mousePos);
    void takeDamage(float amount);
    bool isDead() const;

    void pickUpItem(Item item);
    void useItem(int index);
    std::vector<Item> getInventory() const;

    void addCoins(int amount);
    int getCoins() const;

    void pickUpWeapon(Weapon w);
    void switchWeapon(int index);
    Weapon getCurrentWeapon() const;
    sf::Vector2f getAttackDirection() const;
    bool getIsAttacking() const;
    void stopAttacking();

    sf::Vector2f getPos() const;
    float getHp() const;
    float getMaxHp() const;
    float getMana() const;
    float getMaxMana() const;
    float getStamina() const;
    float getMaxStamina() const;
    bool isInvincible() const;

    void setPos(sf::Vector2f newPos);
    void setSpeed(float newSpeed);

private:
    sf::RectangleShape shape;

    float speed;
    float dashSpeed;
    float dashDuration;
    float dashTimer;
    float dashCooldown;
    float dashCoolTimer;
    bool canDash;
    bool invincible;

    float hp;
    float maxHp;
    float mana;
    float maxMana;
    float manaRegen;
    float stamina;
    float maxStamina;
    float staminaRegen;

    float attackTimer;
    sf::Vector2f attackDirection;
    bool isAttacking = false;

    std::vector<Weapon> weapons;
    int currentWeaponIndex;

    std::vector<Item> inventory;
    int coins;
};