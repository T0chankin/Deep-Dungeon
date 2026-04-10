#include "player.hpp"
#include "mapGenerator.hpp"
Player::Player() {
    shape.setSize({20.f, 20.f});
    shape.setOrigin({10.f, 10.f});
    shape.setFillColor(sf::Color(50, 100, 200));

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

    Weapon bow;
    bow.name        = "Bow";
    bow.type        = WeaponType::Bow;
    bow.damage      = 15;
    bow.attackSpeed = 0.8f;
    bow.manaCost    = 0.f;
    bow.range       = 200.f;
    bow.angle       = 20.f;
    weapons.push_back(bow);

    Weapon fireball;
    fireball.name        = "Fireball";
    fireball.type        = WeaponType::Fireball;
    fireball.damage      = 35;
    fireball.attackSpeed = 1.f;
    fireball.manaCost    = 25.f;
    fireball.range       = 250.f;
    fireball.angle       = 10.f;
    weapons.push_back(fireball);

    Item hp_pot;
    hp_pot.name  = "Health Potion";
    hp_pot.type  = ItemType::HealthPotion;
    hp_pot.value = 30.f;  // восстанавливает 30 HP
    inventory.push_back(hp_pot);
    inventory.push_back(hp_pot);  // два зелья здоровья

    Item mana_pot;
    mana_pot.name  = "Mana Potion";
    mana_pot.type  = ItemType::ManaPotion;
    mana_pot.value = 40.f;  // восстанавливает 40 маны
    inventory.push_back(mana_pot);

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

    if (!canDash) {
        dashCoolTimer -= dt;
        if (dashCoolTimer <= 0.f)
            canDash = true;
    }
}


void Player::doDash() {
    if (!canDash) return;

    sf::Vector2f dir = {0.f, 0.f};
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) dir.y -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) dir.y += 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) dir.x -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) dir.x += 1.f;

    float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
    if (len == 0.f) return;
    dir /= len;

    dashDirection = dir;
    dashTimer     = 0.12f;
    dashSpeed     = 280.f;
    canDash       = false;
    dashCoolTimer = dashCooldown;
    invincible    = true;
}

void Player::move(sf::Vector2f direction, float dt) {
    if (dashTimer > 0.f) {
        shape.move(dashDirection * dashSpeed * dt);
    } else {
        shape.move(direction * speed * dt);
    }
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

    if (w.type == WeaponType::Bow || w.type == WeaponType::Fireball) {
        Projectile p;
        p.direction = dir;
        p.damage    = w.damage;
        p.speed     = 300.f;

        p.shape.setOrigin({5.f, 5.f});
        p.shape.setPosition(getPos());

        if (w.type == WeaponType::Bow) {
            p.shape.setRadius(4.f);
            p.shape.setFillColor(sf::Color(200, 170, 100));
        } else {
            p.shape.setRadius(7.f);
            p.shape.setFillColor(sf::Color(255, 80, 0));
            p.speed = 250.f;
        }

        projectiles.push_back(p);
        isAttacking = false;  
    }
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
    if (item.type == ItemType::HealthPotion) {
        hp = std::min(maxHp, hp + item.value);
        hp = std::max(0.f, hp);
    }else if (item.type == ItemType::ManaPotion) {
        mana = std::min(maxMana, mana + item.value); 
        mana = std::max(0.f, mana);
    }

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
void Player::updateProjectiles(float dt, MapGenerator& map) {
    for (auto& p : projectiles) {
        if (!p.active) continue;

        p.shape.move(p.direction * p.speed * dt);
        sf::Vector2f pos = p.shape.getPosition();

        int tx = (int)(pos.x / MapGenerator::TILE_SIZE);
        int ty = (int)(pos.y / MapGenerator::TILE_SIZE);
        if (map.getTile(tx, ty) == Tile::Wall) {
            p.active = false;
            continue;
        }

        sf::Vector2f startPos = getPos();
        float dx = pos.x - startPos.x;
        float dy = pos.y - startPos.y;
        if (std::sqrt(dx * dx + dy * dy) > 600.f)
            p.active = false;
    }

    projectiles.erase(
        std::remove_if(projectiles.begin(), projectiles.end(),
            [](const Projectile& p) { return !p.active; }),
        projectiles.end()
    );
}

void Player::drawProjectiles(sf::RenderWindow& window) {
    for (auto& p : projectiles)
        if (p.active) window.draw(p.shape);
}

std::vector<Projectile>& Player::getProjectiles() {
    return projectiles;
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

sf::Vector2f Player::getAttackDirection() const { return attackDirection; }
bool Player::getIsAttacking() const{ return isAttacking; }
void Player::stopAttacking(){ isAttacking = false; }