#include "dungeon.hpp"
#include <cmath>
#include <iostream>
Dungeon::Dungeon(sf::RenderWindow& window, sf::Font& font, AudioManager& audio)
    : window(window), font(font), hud(window, font), audio(audio)
{
    camera = sf::View(sf::FloatRect({0.f, 0.f},
        {(float)window.getSize().x/3, (float)window.getSize().y/3}));

    mapGen.generate(currentFloor);
    player.setPos(mapGen.getPlayerSpawn());
    camera.setCenter(mapGen.getPlayerSpawn());
    monsters.reserve(50);
    spawnMonsters();
    spawnItems();
}


void Dungeon::handleEvent(const sf::Event& event) {
    if (const auto* click = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (click->button == sf::Mouse::Button::Left) {
            sf::Vector2f mousePos = window.mapPixelToCoords(
                sf::Mouse::getPosition(window), camera);
            player.attack(mousePos);
        }
    }

    if (const auto* key = event.getIf<sf::Event::KeyPressed>()) {
        switch (key->code) {
            case sf::Keyboard::Key::Num1: player.switchWeapon(0); break;
            case sf::Keyboard::Key::Num2: player.switchWeapon(1); break;
            case sf::Keyboard::Key::Num3: player.switchWeapon(2); break;
            case sf::Keyboard::Key::Q:    usePotion(ItemType::HealthPotion); break;
            case sf::Keyboard::Key::E:    usePotion(ItemType::ManaPotion);   break;
            default: break;
        }
    }
}

void Dungeon::handleInput(float dt) {
    sf::Vector2f dir = {0.f, 0.f};

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) dir.y -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) dir.y += 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) dir.x -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) dir.x += 1.f;

    float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
    if (len != 0.f) dir /= len;
    if (len != 0.f)player.move(dir, dt);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space))
        player.doDash();

}

void Dungeon::checkHatch() {
    sf::Vector2f pos = player.getPos();
    int tx = (int)(pos.x / MapGenerator::TILE_SIZE);
    int ty = (int)(pos.y / MapGenerator::TILE_SIZE);

    if (mapGen.getTile(tx, ty) == Tile::Hatch)
        nextFloor();
}

void Dungeon::nextFloor() {
    currentFloor++;

    if (currentFloor > 3) {
        finished = true;
        return;
    }
    mapGen.generate(currentFloor);
    player.setPos(mapGen.getPlayerSpawn());
    camera.setCenter(mapGen.getPlayerSpawn());
    spawnMonsters();
    spawnItems();
}

void Dungeon::updateCamera(float dt) {
    sf::Vector2f playerPos = player.getPos();
    sf::Vector2f camCenter = camera.getCenter();

    sf::Vector2f diff = playerPos - camCenter;
    float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);

    if (dist > cameraDeadZone) {
        sf::Vector2f dir = diff / dist;
        float moveAmount = (dist - cameraDeadZone) * cameraSpeed * dt;
        camera.setCenter(camCenter + dir * moveAmount);
    }
}

void Dungeon::update(float dt) {
    handleInput(dt);
    player.update(dt);

    sf::Vector2f pos = player.getPos();
    resolveCollision(pos);
    player.setPos(pos);

    handleAttack();
    handleProjectiles(dt);

    if (attackVisualTimer > 0.f)
        attackVisualTimer -= dt;

    checkItemPickup();
    updateCamera(dt);
    checkHatch();
    hud.update(player);

    for (auto& m : monsters)
        m.update(dt, player, mapGen);

    // Идём с конца чтобы индексы не съезжали
    for (int i = (int)monsters.size() - 1; i >= 0; i--) {
    if (!monsters[i].isDead()) continue;

    std::cout << "Monster " << i << " died, dropping coin\n";
    
    WorldItem coin;
    coin.type = ItemSpawnType::Coin;
    coin.shape.setRadius(6.f);
    coin.shape.setOrigin({6.f, 6.f});
    coin.shape.setPosition(monsters[i].getPos());
    coin.shape.setFillColor(sf::Color(255, 215, 0));
    worldItems.push_back(coin);

    std::cout << "Erasing monster " << i << "\n";
    monsters.erase(monsters.begin() + i);
    std::cout << "Erased\n";
    }
}


void Dungeon::draw() {
    window.setView(camera);
    mapGen.draw(window);

    for (auto& item : worldItems)
        if (!item.collected) window.draw(item.shape);

    drawAttackVisual();
    player.drawProjectiles(window);
    for (auto& m : monsters) m.draw(window);
    player.draw(window);

    window.setView(window.getDefaultView());
    hud.draw();
}


void Dungeon::resolveCollision(sf::Vector2f& pos) {
    int tileSize = MapGenerator::TILE_SIZE;
    float half   = 10.f;

    std::vector<sf::Vector2f> corners = {
        {pos.x - half, pos.y - half},
        {pos.x + half, pos.y - half},
        {pos.x - half, pos.y + half},
        {pos.x + half, pos.y + half}
    };

    for (auto& corner : corners) {
        int tx = (int)(corner.x / tileSize);
        int ty = (int)(corner.y / tileSize);

        if (mapGen.getTile(tx, ty) != Tile::Wall) continue;

        float tileLeft   = tx * tileSize;
        float tileRight  = tileLeft  + tileSize;
        float tileTop    = ty * tileSize;
        float tileBottom = tileTop   + tileSize;

        float overlapLeft   = (pos.x + half) - tileLeft;
        float overlapRight  = tileRight  - (pos.x - half);
        float overlapTop    = (pos.y + half) - tileTop;
        float overlapBottom = tileBottom - (pos.y - half);

        float minOverlap = std::min({overlapLeft, overlapRight,
                                     overlapTop,  overlapBottom});

        if      (minOverlap == overlapLeft)   pos.x -= overlapLeft;
        else if (minOverlap == overlapRight)  pos.x += overlapRight;
        else if (minOverlap == overlapTop)    pos.y -= overlapTop;
        else                                  pos.y += overlapBottom;
    }
}
void Dungeon::drawAttackVisual() {
    if (attackVisualTimer <= 0.f) return;

    Weapon w          = player.getCurrentWeapon();
    sf::Vector2f pos  = player.getPos();
    sf::Vector2f dir  = player.getAttackDirection();

    float attackAngle = std::atan2(dir.y, dir.x) * (180.f / 3.14159f);
    float halfAngle   = w.angle / 2.f;

    int points = 12;
    sf::ConvexShape sector(points + 2);
    sector.setPoint(0, {0.f, 0.f});

    for (int i = 0; i <= points; i++) {
        float angle = (attackAngle - halfAngle + (w.angle / points) * i)
                      * (3.14159f / 180.f);
        sector.setPoint(i + 1, {
            std::cos(angle) * w.range,
            std::sin(angle) * w.range
        });
    }

    sector.setPosition(pos);

    if (w.type == WeaponType::Sword)
        sector.setFillColor(sf::Color(255, 255, 100, 80));
    else if (w.type == WeaponType::Fireball)
        sector.setFillColor(sf::Color(255, 100, 0, 80));
    else
        sector.setFillColor(sf::Color(100, 200, 255, 80));

    window.draw(sector);
}

void Dungeon::spawnMonsters() {
    monsters.clear();
    monsters.reserve(50);  // резервируем снова после clear
    for (auto& spawn : mapGen.getMonsterSpawns()) {
        sf::Vector2f pos = {
            spawn.tilePos.x * (float)MapGenerator::TILE_SIZE + MapGenerator::TILE_SIZE / 2.f,
            spawn.tilePos.y * (float)MapGenerator::TILE_SIZE + MapGenerator::TILE_SIZE / 2.f
        };
        monsters.emplace_back(pos, spawn.type, currentFloor, font);
    }
}
void Dungeon::handleAttack() {
    if (!player.getIsAttacking()) return;

    Weapon w = player.getCurrentWeapon();
    if (w.type == WeaponType::Sword)
        audio.playSword();
    else if (w.type == WeaponType::Bow)
        audio.playBow();
    else if (w.type == WeaponType::Fireball)
        audio.playFireball();
    
    sf::Vector2f playerPos = player.getPos();
    sf::Vector2f attackDir = player.getAttackDirection();

    for (auto& m : monsters) {
        if (m.isDead()) continue;

        sf::Vector2f diff = m.getPos() - playerPos;
        float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);

        if (dist > w.range) continue;

        float monsterAngle = std::atan2(diff.y, diff.x);
        float attackAngle  = std::atan2(attackDir.y, attackDir.x);
        float halfAngle    = (w.angle / 2.f) * (3.14159f / 180.f);

        float angleDiff = monsterAngle - attackAngle;
        
        while (angleDiff >  3.14159f) angleDiff -= 2.f * 3.14159f;
        while (angleDiff < -3.14159f) angleDiff += 2.f * 3.14159f;

        if (std::abs(angleDiff) <= halfAngle)
            m.takeDamage(w.damage);
    }

    attackVisualTimer = 0.1f;
    player.stopAttacking();
}

void Dungeon::handleProjectiles(float dt) {
    player.updateProjectiles(dt, mapGen);

    auto& projectiles = player.getProjectiles();
    for (int i = 0; i < (int)projectiles.size(); i++) {
        if (!projectiles[i].active) continue;

        for (int j = 0; j < (int)monsters.size(); j++) {
            if (monsters[j].isDead()) continue;

            sf::Vector2f diff = projectiles[i].shape.getPosition() - monsters[j].getPos();
            float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);

            if (dist < 20.f) {
                monsters[j].takeDamage(projectiles[i].damage);
                projectiles[i].active = false;
                break;
            }
        }
    }
}

void Dungeon::usePotion(ItemType type) {
    auto inventory = player.getInventory();
    for (int i = 0; i < (int)inventory.size(); i++) {
        if (inventory[i].type == type) {
            player.useItem(i);
            audio.playHeal();
            break;
        }
    }
}

void Dungeon::spawnItems() {
    worldItems.clear();

    for (auto& spawn : mapGen.getItemSpawns()) {
        WorldItem item;
        sf::Vector2f pos = {
            spawn.tilePos.x * (float)MapGenerator::TILE_SIZE + MapGenerator::TILE_SIZE / 2.f,
            spawn.tilePos.y * (float)MapGenerator::TILE_SIZE + MapGenerator::TILE_SIZE / 2.f
        };

        item.type = spawn.type;
        item.shape.setRadius(6.f);
        item.shape.setOrigin({6.f, 6.f});
        item.shape.setPosition(pos);

        switch (spawn.type) {
            case ItemSpawnType::Coin:
                item.shape.setFillColor(sf::Color(255, 215, 0));   // золотой
                break;
            case ItemSpawnType::HealthPotion:
                item.shape.setFillColor(sf::Color(200, 50, 50));   // красный
                break;
            case ItemSpawnType::ManaPotion:
                item.shape.setFillColor(sf::Color(50, 80, 200));   // синий
                break;
        }

        worldItems.push_back(item);
    }
}

void Dungeon::checkItemPickup() {
    sf::Vector2f playerPos = player.getPos();

    for (auto& item : worldItems) {
        if (item.collected) continue;

        sf::Vector2f diff = item.shape.getPosition() - playerPos;
        float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);

        if (dist > 20.f) continue;

        item.collected = true;
        switch (item.type) {
            case ItemSpawnType::Coin:
                player.addCoins(1);
                audio.playPickup();
                break;
            case ItemSpawnType::HealthPotion: {
                Item pot;
                pot.name  = "Health Potion";
                pot.type  = ItemType::HealthPotion;
                pot.value = 30.f;
                player.pickUpItem(pot);
                audio.playPickup();
                break;
            }
            case ItemSpawnType::ManaPotion: {
            Item pot;
            pot.name  = "Mana Potion";
            pot.type  = ItemType::ManaPotion;
            pot.value = 40.f;
            player.pickUpItem(pot);
            audio.playPickup();   // <-- звук подбора
            break;
            }
        }
    }

    worldItems.erase(
        std::remove_if(worldItems.begin(), worldItems.end(),
            [](const WorldItem& i) { return i.collected; }),
        worldItems.end()
    );
}

int Dungeon::getCurrentFloor() const {return currentFloor;}
bool Dungeon::isFinished() const {return finished;}
int Dungeon::getCoins() const {return player.getCoins();}

bool Dungeon::isPlayerDead() const { return player.isDead(); }