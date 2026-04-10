#include "mapGenerator.hpp"
#include <algorithm>
#include <cmath>

MapGenerator::MapGenerator() {
    map.assign(MAP_H, std::vector<Tile>(MAP_W, Tile::Wall));
    rng.seed(std::random_device{}());

    if (floorTexture.loadFromFile("assets/floor.png")) {
        floorTexture.setRepeated(true);
        floorSprite = new sf::Sprite(floorTexture);
        hasFloorTexture = true;
    }
    if (hatchTexture.loadFromFile("assets/hatch.png")) {
    hatchSprite = new sf::Sprite(hatchTexture);
    hatchSprite->setOrigin({hatchTexture.getSize().x / 2.f,
                            hatchTexture.getSize().y / 2.f});
    hasHatchTexture = true;
}
}
MapGenerator::~MapGenerator() {
    delete floorSprite;
}

bool MapGenerator::inBounds(int x, int y) const {
    return x >= 0 && x < MAP_W && y >= 0 && y < MAP_H;
}

Tile MapGenerator::getTile(int x, int y) const {
    if (!inBounds(x, y)) return Tile::Wall;
    return map[y][x];
}

sf::Vector2f MapGenerator::getPlayerSpawn() const                { return playerSpawn; }
std::vector<MonsterSpawn> MapGenerator::getMonsterSpawns() const { return monsterSpawns; }
std::vector<Room> MapGenerator::getRooms() const                 { return rooms; }
FloorType MapGenerator::getFloorType() const                     { return floorType; }

void MapGenerator::generate(int floorNumber) {
    rooms.clear();
    monsterSpawns.clear();
    map.assign(MAP_H, std::vector<Tile>(MAP_W, Tile::Wall));
    floorType = FloorType::Normal;

    // Заполняем всё полом кроме границ
    for (int y = 1; y < MAP_H - 1; y++)
        for (int x = 1; x < MAP_W - 1; x++)
            map[y][x] = Tile::Floor;

    placeObstacleRooms(floorNumber);
    placeSpawn();
    placeHatch();
    placeMonsters(floorNumber);
    placeItems(floorNumber);
}

void MapGenerator::placeObstacleRooms(int floorNumber) {
    std::uniform_int_distribution<int> roomW(5, 13);
    std::uniform_int_distribution<int> roomH(4, 12);
    std::uniform_int_distribution<int> roomX(3, MAP_W - 3);
    std::uniform_int_distribution<int> roomY(3, MAP_H - 2);

    int count = 12 + floorNumber * 2;

    for (int i = 0; i < count; i++) {
        Room room;
        room.width  = roomW(rng);
        room.height = roomH(rng);
        room.x      = roomX(rng);
        room.y      = roomY(rng);

        bool overlaps = false;
        for (auto& r : rooms) {
            if (room.intersects(r)) {
                overlaps = true;
                break;
            }
        }
        if (overlaps) continue;

        // Рисуем только периметр — стены
        for (int y = room.y; y < room.y + room.height; y++) {
            for (int x = room.x; x < room.x + room.width; x++) {
                if (!inBounds(x, y)) continue;
                bool isBorder = (x == room.x ||
                                 x == room.x + room.width  - 1 ||
                                 y == room.y ||
                                 y == room.y + room.height - 1);
                if (isBorder)
                    map[y][x] = Tile::Wall;
            }
        }

        carvePassages(room);
        rooms.push_back(room);
    }
}

void MapGenerator::carvePassages(const Room& room) {
    std::uniform_int_distribution<int> coin(0, 1);

    // Верхняя стена — всегда
    {
        int px = room.x + 1 + coin(rng) * (room.width / 2);
        px = std::clamp(px, room.x + 1, room.x + room.width - 2);
        if (inBounds(px, room.y))
            map[room.y][px] = Tile::Floor;
    }

    // Нижняя стена — всегда
    {
        int px = room.x + 1 + coin(rng) * (room.width / 2);
        px = std::clamp(px, room.x + 1, room.x + room.width - 2);
        int py = room.y + room.height - 1;
        if (inBounds(px, py))
            map[py][px] = Tile::Floor;
    }

    // Левая стена — иногда
    if (coin(rng) == 1) {
        int py = room.y + 1 + coin(rng) * (room.height / 2);
        py = std::clamp(py, room.y + 1, room.y + room.height - 2);
        if (inBounds(room.x, py))
            map[py][room.x] = Tile::Floor;
    }

    // Правая стена — иногда
    if (coin(rng) == 1) {
        int py = room.y + 1 + coin(rng) * (room.height / 2);
        py = std::clamp(py, room.y + 1, room.y + room.height - 2);
        int px = room.x + room.width - 1;
        if (inBounds(px, py))
            map[py][px] = Tile::Floor;
    }
}

sf::Vector2i MapGenerator::randomFloorTile() {
    std::uniform_int_distribution<int> rx(2, MAP_W - 3);
    std::uniform_int_distribution<int> ry(2, MAP_H - 3);

    for (int attempt = 0; attempt < 200; attempt++) {
        int x = rx(rng), y = ry(rng);
        if (map[y][x] == Tile::Floor)
            return {x, y};
    }
    return {2, 2};
}

void MapGenerator::placeSpawn() {
    sf::Vector2i pos = randomFloorTile();
    map[pos.y][pos.x] = Tile::Spawn;
    playerSpawn = {
        pos.x * (float)TILE_SIZE + TILE_SIZE / 2.f,
        pos.y * (float)TILE_SIZE + TILE_SIZE / 2.f
    };
}

void MapGenerator::placeHatch() {
    sf::Vector2f spawnPx = playerSpawn;
    sf::Vector2i best    = {MAP_W - 3, MAP_H - 3};
    float bestDist       = 0.f;

    std::uniform_int_distribution<int> rx(2, MAP_W - 3);
    std::uniform_int_distribution<int> ry(2, MAP_H - 3);

    for (int attempt = 0; attempt < 200; attempt++) {
        int x = rx(rng), y = ry(rng);
        if (map[y][x] != Tile::Floor) continue;

        float dx   = x * (float)TILE_SIZE - spawnPx.x;
        float dy   = y * (float)TILE_SIZE - spawnPx.y;
        float dist = std::sqrt(dx * dx + dy * dy);

        if (dist > bestDist) {
            bestDist = dist;
            best     = {x, y};
        }
    }

    map[best.y][best.x] = Tile::Hatch;
}

void MapGenerator::placeMonsters(int floorNumber) {
    std::uniform_int_distribution<int> coin(0, 1);
    int count = 4 + floorNumber * 2;

    for (int i = 0; i < count; i++) {
        sf::Vector2i pos = randomFloorTile();
        MonsterSpawn spawn;
        spawn.tilePos = pos;
        spawn.type    = (coin(rng) == 0) ? MonsterType::Melee
                                         : MonsterType::Ranged;
        monsterSpawns.push_back(spawn);
    }
}

sf::Color MapGenerator::getTileColor(Tile tile) const {
    switch (tile) {
        case Tile::Wall:  return sf::Color(40,  40,  60);
        case Tile::Floor: return sf::Color(100, 90,  80);
        case Tile::Hatch: return sf::Color(0,   200, 200);
        case Tile::Spawn: return sf::Color(0,   200, 0);
        default:          return sf::Color::Black;
    }
}

void MapGenerator::draw(sf::RenderWindow& window) {
    sf::RectangleShape tile({(float)TILE_SIZE - 1.f, (float)TILE_SIZE - 1.f});

    for (int y = 0; y < MAP_H; y++) {
        for (int x = 0; x < MAP_W; x++) {
            Tile t = map[y][x];
            sf::Vector2f pos = {x * (float)TILE_SIZE, y * (float)TILE_SIZE};

            if (t == Tile::Wall) {
                tile.setPosition(pos);
                tile.setFillColor(sf::Color(40, 40, 60));
                window.draw(tile);
            } else {
                // Пол, люк, спавн — рисуем текстуру или цвет
                if (hasFloorTexture) {
                    floorSprite->setPosition(pos);
                    floorSprite->setTextureRect(sf::IntRect(
                        {0, 0},
                        {TILE_SIZE, TILE_SIZE}
                    ));
                    window.draw(*floorSprite);
                } else {
                    tile.setPosition(pos);
                    tile.setFillColor(getTileColor(t));
                    window.draw(tile);
                }
                if (t == Tile::Hatch) {
                    if (hasHatchTexture) {
                        hatchSprite->setPosition({
                            x * (float)TILE_SIZE + TILE_SIZE / 2.f,
                            y * (float)TILE_SIZE + TILE_SIZE / 2.f
                        });
                        window.draw(*hatchSprite);
                    } else {
                        tile.setFillColor(sf::Color(0, 200, 200));
                        tile.setPosition(pos);
                        window.draw(tile);
                    }
                }

                // Поверх текстуры рисуем люк и спавн
                if (t == Tile::Hatch || t == Tile::Spawn) {
                    tile.setSize({(float)TILE_SIZE - 1.f, (float)TILE_SIZE - 1.f});
                    tile.setPosition(pos);
                    tile.setFillColor(getTileColor(t));
                    window.draw(tile);
                }
            }
        }
    }
}

std::vector<ItemSpawn> MapGenerator::getItemSpawns() const { return itemSpawns; }

void MapGenerator::placeItems(int floorNumber) {
    itemSpawns.clear();
    std::uniform_int_distribution<int> coin(0, 2);  // 0=монета 1=хилка 2=мана

    int count = 8 + floorNumber * 3; //количество предметов на этаже

    for (int i = 0; i < count; i++) {
        sf::Vector2i pos = randomFloorTile();
        ItemSpawn spawn;
        spawn.tilePos = pos;

        int roll = coin(rng);
        if (roll == 0)      spawn.type = ItemSpawnType::Coin;
        else if (roll == 1) spawn.type = ItemSpawnType::HealthPotion;
        else                spawn.type = ItemSpawnType::ManaPotion;

        itemSpawns.push_back(spawn);
    }
}