#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <queue>
#include <unordered_map>
#include "mapGenerator.hpp"

struct PathNode {
    sf::Vector2i pos;
    float f, g; 
    bool operator>(const PathNode& other) const { return f > other.f; }
};

class Pathfinder {
public:

    static std::vector<sf::Vector2i> findPath(
        sf::Vector2i start,
        sf::Vector2i end,
        MapGenerator& map
    );

private:
    static float heuristic(sf::Vector2i a, sf::Vector2i b);
    static bool isWalkable(sf::Vector2i pos, MapGenerator& map);
};