#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <random>

enum class Tile {
    Wall,
    Floor,
    Hatch,
    Spawn
};

enum class MonsterType {
    Melee,
    Ranged
};

struct MonsterSpawn {
    sf::Vector2i tilePos;
    MonsterType type;
};

enum class ItemSpawnType {
    Coin,
    HealthPotion,
    ManaPotion
};

struct ItemSpawn {
    sf::Vector2i tilePos;
    ItemSpawnType type;
};

struct Room {
    int x, y;
    int width, height;

    sf::Vector2i center() const {
        return {x + width / 2, y + height / 2};
    }

    bool intersects(const Room& other) const {
        int padding = 2;
        return x - padding < other.x + other.width  &&
               x + width  + padding > other.x        &&
               y - padding < other.y + other.height  &&
               y + height + padding > other.y;
    }
};

enum class FloorType { Normal, Boss };

class MapGenerator {
public:
    static const int MAP_W     = 60;
    static const int MAP_H     = 45;
    static const int TILE_SIZE = 32;

    MapGenerator();
    ~MapGenerator();
    void generate(int floorNumber);

    Tile getTile(int x, int y) const;
    sf::Vector2f getPlayerSpawn() const;
    std::vector<MonsterSpawn> getMonsterSpawns() const;
    std::vector<Room> getRooms() const;
    FloorType getFloorType() const;

    void draw(sf::RenderWindow& window);

    std::vector<ItemSpawn> getItemSpawns() const;

private:
    std::vector<std::vector<Tile>> map;
    std::vector<Room> rooms;
    std::vector<MonsterSpawn> monsterSpawns;

    sf::Vector2f playerSpawn;
    FloorType floorType;

    std::mt19937 rng;

    void placeObstacleRooms(int floorNumber);
    void placeSpawn();
    void placeHatch();
    void placeMonsters(int floorNumber);
    
    void carvePassages(const Room& room);
    sf::Vector2i randomFloorTile();

    sf::Color getTileColor(Tile tile) const;
    bool inBounds(int x, int y) const;

    std::vector<ItemSpawn> itemSpawns;
    void placeItems(int floorNumber);

    sf::Texture floorTexture;
    sf::Sprite* floorSprite = nullptr;
    bool hasFloorTexture = false;

    sf::Texture hatchTexture;
    sf::Sprite* hatchSprite = nullptr;
    bool hasHatchTexture = false;
};

