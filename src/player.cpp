#include "player.hpp"

Player::Player() {
    shape.setSize({32.f, 32.f});
    shape.setFillColor(sf::Color(50, 100, 200));
    shape.setOrigin({16.f, 16.f});

    shape.setPosition({400.f, 300.f});

    speed        = 150.f;
    dashSpeed    = 400.f;
    dashDuration = 0.15f;
    dashTimer    = 0.f;
    dashCooldown = 1.f;
    dashCoolTimer = 0.f;
    canDash      = true;
    invincible   = false;

    maxHp       = 100.f;
    hp          = maxHp;
    maxMana     = 100.f;
    mana        = maxMana;
    manaRegen   = 5.f;
    maxStamina  = 100.f;
    stamina     = maxStamina;
    staminaRegen = 10.f;

    attackTimer = 0.f;

    Weapon sword;
    sword.name        = "Sword";
    sword.type        = WeaponType::Sword;
    sword.damage      = 20;
    sword.attackSpeed = 0.5f; 
    sword.manaCost    = 0.f;
    sword.range       = 60.f; 
    sword.angle       = 120.f; 

    weapons.push_back(sword);
    currentWeaponIndex = 0;

    coins = 0;
}

Player::~Player() {}

void Player::update(float dt) {
    if (attackTimer > 0.f)
        attackTimer -= dt;

    mana = std::min(maxMana, mana + manaRegen * dt);

    stamina = std::min(maxStamina, stamina + staminaRegen * dt);

    if (dashTimer > 0.f) {
        dashTimer -= dt;
        invincible = true;
        if (dashTimer <= 0.f)
            invincible = false;
    }

    // Перезарядка рывка
    if (!canDash) {
        dashCoolTimer -= dt;
        if (dashCoolTimer <= 0.f)
            canDash = true;
    }
}

void Player::move(sf::Vector2f direction) {
    shape.move(direction * speed);
}

void Player::doDash() {
    if (!canDash) return;
    dashTimer    = dashDuration;
    canDash      = false;
    dashCoolTimer = dashCooldown;
    invincible   = true;
}

void Player::attack(sf::Vector2f mousePos) {
    if (attackTimer > 0.f) return;

    Weapon& w = weapons[currentWeaponIndex];

    if (w.type == WeaponType::Fireball) {
        if (mana < w.manaCost) return;
        mana -= w.manaCost;
    }

    attackTimer = w.attackSpeed;

    sf::Vector2f dir = mousePos - getPos();
    float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
    if (len != 0.f) dir /= len;  

    attackDirection = dir;
    isAttacking = true;
}

void Player::takeDamage(float amount) {
    if (invincible) return;
    hp -= amount;
    if (hp < 0.f) hp = 0.f;
}

bool Player::isDead() const {
    return hp <= 0.f;
}

void Player::pickUpItem(Item item) {
    inventory.push_back(item);
}

void Player::useItem(int index) {
    if (index < 0 || index >= (int)inventory.size()) return;

    Item& item = inventory[index];
    if (item.type == ItemType::HealthPotion)
        hp = std::min(maxHp, hp + item.value);
    else if (item.type == ItemType::ManaPotion)
        mana = std::min(maxMana, mana + item.value);

    inventory.erase(inventory.begin() + index);
}

void Player::addCoins(int amount) {
    coins += amount;
}

void Player::pickUpWeapon(Weapon w) {
    weapons.push_back(w);
}

void Player::switchWeapon(int index) {
    if (index >= 0 && index < (int)weapons.size())
        currentWeaponIndex = index;
}

void Player::draw(sf::RenderWindow& window) {
    if (invincible) {
        sf::Color c = shape.getFillColor();
        c.a = (c.a == 255) ? 100 : 255;
        shape.setFillColor(c);
    }
    window.draw(shape);
}

sf::Vector2f Player::getPos() const   { return shape.getPosition(); }
float Player::getHp() const           { return hp; }
float Player::getMaxHp() const        { return maxHp; }
float Player::getMana() const         { return mana; }
float Player::getMaxMana() const      { return maxMana; }
float Player::getStamina() const      { return stamina; }
float Player::getMaxStamina() const   { return maxStamina; }
bool Player::isInvincible() const     { return invincible; }
int Player::getCoins() const          { return coins; }
Weapon Player::getCurrentWeapon() const { return weapons[currentWeaponIndex]; }
std::vector<Item> Player::getInventory() const { return inventory; }

void Player::setPos(sf::Vector2f newPos) { shape.setPosition(newPos); }
void Player::setSpeed(float newSpeed)    { speed = newSpeed; }