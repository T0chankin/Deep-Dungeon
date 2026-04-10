#include "monster.hpp"
#include "pathfinder.hpp"
#include <memory>
#include <cmath>
#include <algorithm>

Monster::Monster(sf::Vector2f pos, MonsterType type, int floorNumber, sf::Font& font)
    : type(type), state(MonsterState::Idle), font(&font)
{
    float floorMult = 1.f + (floorNumber - 1) * 0.3f;

    if (type == MonsterType::Melee) {
        shape.setSize({18.f, 18.f});
        shape.setFillColor(sf::Color(200, 50, 50));
        hp             = 50.f * floorMult;
        maxHp          = hp;
        speed          = 70.f * floorMult;
        attackDamage   = 10.f * floorMult;
        attackRange    = 25.f;
        attackCooldown = 1.f;
        aggroRange     = 250.f;
    } else {
        shape.setSize({16.f, 16.f});
        shape.setFillColor(sf::Color(200, 100, 200));
        hp             = 30.f * floorMult;
        maxHp          = hp;
        speed          = 40.f * floorMult;
        attackDamage   = 8.f  * floorMult;
        attackRange    = 250.f;
        attackCooldown = 2.f;
        aggroRange     = 300.f;
    }

    shape.setOrigin({shape.getSize().x / 2.f, shape.getSize().y / 2.f});
    shape.setPosition(pos);
    attackTimer = 0.f;

    std::string texPath = (type == MonsterType::Melee)
        ? "assets/monster_melee.png"
        : "assets/monster_ranged.png";

    if (texture.loadFromFile(texPath)) {
        sprite = std::make_unique<sf::Sprite>(texture);
        sprite->setOrigin({texture.getSize().x / 2.f,
                           texture.getSize().y / 2.f});
        hasSprite = true;
    }
}


void Monster::resolveCollision(sf::Vector2f& pos, MapGenerator& map, float half) {
    int tileSize = MapGenerator::TILE_SIZE;

    std::vector<sf::Vector2f> corners = {
        {pos.x - half, pos.y - half},
        {pos.x + half, pos.y - half},
        {pos.x - half, pos.y + half},
        {pos.x + half, pos.y + half}
    };

    for (auto& corner : corners) {
        int tx = (int)(corner.x / tileSize);
        int ty = (int)(corner.y / tileSize);
        if (map.getTile(tx, ty) != Tile::Wall) continue;

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

sf::Vector2f Monster::moveAlongPath(sf::Vector2f myPos, float spd,
                                     float dt, MapGenerator& map, float half)
{
    if (path.empty()) return myPos;

    sf::Vector2f target = {
        path.front().x * (float)MapGenerator::TILE_SIZE + MapGenerator::TILE_SIZE / 2.f,
        path.front().y * (float)MapGenerator::TILE_SIZE + MapGenerator::TILE_SIZE / 2.f
    };

    sf::Vector2f diff = target - myPos;
    float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);

    if (dist < 4.f) {
        path.erase(path.begin());
        return myPos;
    }

    sf::Vector2f dir = diff / dist;
    sf::Vector2f newPos = myPos + dir * spd * dt;
    resolveCollision(newPos, map, half);
    return newPos;
}

void Monster::update(float dt, Player& player, MapGenerator& map) {
    if (isDead()) return;
    attackTimer -= dt;
    pathTimer   -= dt;

    if (type == MonsterType::Melee)
        updateMelee(dt, player, map);
    else
        updateRanged(dt, player, map);
}

void Monster::updateMelee(float dt, Player& player, MapGenerator& map) {
    sf::Vector2f myPos     = shape.getPosition();
    sf::Vector2f playerPos = player.getPos();
    sf::Vector2f diff      = playerPos - myPos;
    float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);

    if (dist > aggroRange) {
        state = MonsterState::Idle;
        path.clear();
        return;
    }

    if (dist <= attackRange) {

        state = MonsterState::Attack;
        path.clear();
        if (attackTimer <= 0.f) {
            player.takeDamage(attackDamage);
            attackTimer = attackCooldown;
        }
        return;
    }

    state = MonsterState::Chase;
    if (pathTimer <= 0.f) {
        sf::Vector2i start = {
            (int)(myPos.x / MapGenerator::TILE_SIZE),
            (int)(myPos.y / MapGenerator::TILE_SIZE)
        };
        sf::Vector2i end = {
            (int)(playerPos.x / MapGenerator::TILE_SIZE),
            (int)(playerPos.y / MapGenerator::TILE_SIZE)
        };
        path = Pathfinder::findPath(start, end, map);
        pathTimer = pathCooldown;
    }

    sf::Vector2f newPos = moveAlongPath(myPos, speed, dt, map, 9.f);
    shape.setPosition(newPos);
}

void Monster::updateRanged(float dt, Player& player, MapGenerator& map) {
    sf::Vector2f myPos     = shape.getPosition();
    sf::Vector2f playerPos = player.getPos();
    sf::Vector2f diff      = playerPos - myPos;
    float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);

    updateBullets(dt, player, map);

    if (dist > aggroRange) {
        state = MonsterState::Idle;
        path.clear();
        return;
    }

    if (dist < attackRange && attackTimer <= 0.f) {
        state = MonsterState::Attack;
        sf::Vector2f dir = diff / dist;
        shoot(dir);
        attackTimer = attackCooldown;
    }

    if (dist < 120.f) {
        if (pathTimer <= 0.f) {
            sf::Vector2f awayPos = myPos - diff;
            sf::Vector2i start = {
                (int)(myPos.x / MapGenerator::TILE_SIZE),
                (int)(myPos.y / MapGenerator::TILE_SIZE)
            };
            sf::Vector2i end = {
                (int)(awayPos.x / MapGenerator::TILE_SIZE),
                (int)(awayPos.y / MapGenerator::TILE_SIZE)
            };
            path = Pathfinder::findPath(start, end, map);
            pathTimer = pathCooldown;
        }
        sf::Vector2f newPos = moveAlongPath(myPos, speed, dt, map, 8.f);
        shape.setPosition(newPos);
    }
}

void Monster::shoot(sf::Vector2f direction) {
    Bullet b;
    b.shape.setRadius(5.f);
    b.shape.setFillColor(sf::Color(255, 150, 0));
    b.shape.setOrigin({5.f, 5.f});
    b.shape.setPosition(shape.getPosition());
    b.direction = direction;
    b.speed = 200.f;
    bullets.push_back(b);
}

void Monster::updateBullets(float dt, Player& player, MapGenerator& map) {
    for (auto& b : bullets) {
        if (!b.active) continue;

        b.shape.move(b.direction * b.speed * dt);
        sf::Vector2f bPos = b.shape.getPosition();

        int tx = (int)(bPos.x / MapGenerator::TILE_SIZE);
        int ty = (int)(bPos.y / MapGenerator::TILE_SIZE);
        if (map.getTile(tx, ty) == Tile::Wall) {
            b.active = false;
            continue;
        }
        sf::Vector2f pPos = player.getPos();
        float dx   = bPos.x - pPos.x;
        float dy   = bPos.y - pPos.y;
        if (std::sqrt(dx * dx + dy * dy) < 15.f) {
            player.takeDamage(attackDamage);
            b.active = false;
        }
    }

    bullets.erase(
        std::remove_if(bullets.begin(), bullets.end(),
            [](const Bullet& b) { return !b.active; }),
        bullets.end()
    );
}

void Monster::takeDamage(float amount) { hp -= amount; }
bool Monster::isDead() const           { return hp <= 0.f; }
sf::Vector2f Monster::getPos() const   { return shape.getPosition(); }
sf::FloatRect Monster::getBounds() const { return shape.getGlobalBounds(); }

void Monster::draw(sf::RenderWindow& window) {
        if (isDead()) return;

    if (hasSprite) {
        sprite->setPosition(shape.getPosition());
        window.draw(*sprite);
    } else {
        window.draw(shape);
    }

    sf::Vector2f pos = shape.getPosition();
    float barWidth   = shape.getSize().x + 4.f;
    float barHeight  = 4.f;
    float barY       = pos.y - shape.getSize().y / 2.f - 8.f;
    float barX       = pos.x - barWidth / 2.f;

    sf::RectangleShape bgBar({barWidth, barHeight});
    bgBar.setFillColor(sf::Color(60, 20, 20));
    bgBar.setPosition({barX, barY});
    window.draw(bgBar);

    float ratio = hp / maxHp;
    sf::RectangleShape fillBar({barWidth * ratio, barHeight});
    fillBar.setFillColor(sf::Color(200, 50, 50));
    fillBar.setPosition({barX, barY});
    window.draw(fillBar);

    if (font) {
        sf::Text hpText(*font, std::to_string((int)hp) + "/" +
                               std::to_string((int)maxHp), 8);
        hpText.setFillColor(sf::Color::White);
        auto tb = hpText.getLocalBounds();
        hpText.setOrigin({tb.size.x / 2.f, tb.size.y / 2.f});
        hpText.setPosition({pos.x, barY - 10.f});
        window.draw(hpText);
    }

    for (auto& b : bullets)
        if (b.active) window.draw(b.shape);
}

bool Monster::justDied() const {
    return hp <= 0.f && !deathHandled;
}