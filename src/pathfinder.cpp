#include "pathfinder.hpp"
#include <cmath>
#include <algorithm>

float Pathfinder::heuristic(sf::Vector2i a, sf::Vector2i b) {
    // Манхэттенское расстояние
    return std::abs(a.x - b.x) + std::abs(a.y - b.y);
}

bool Pathfinder::isWalkable(sf::Vector2i pos, MapGenerator& map) {
    Tile t = map.getTile(pos.x, pos.y);
    return t != Tile::Wall;
}

std::vector<sf::Vector2i> Pathfinder::findPath(
    sf::Vector2i start,
    sf::Vector2i end,
    MapGenerator& map)
{
    // Если цель — стена, сразу возвращаем пустой путь
    if (!isWalkable(end, map)) return {};

    // Приоритетная очередь — min-heap по f
    std::priority_queue<PathNode,
                        std::vector<PathNode>,
                        std::greater<PathNode>> open;

    // Для восстановления пути
    std::unordered_map<int, sf::Vector2i> cameFrom;
    std::unordered_map<int, float> gScore;

    // Ключ для хранения позиции в map
    auto key = [&](sf::Vector2i p) {
        return p.y * MapGenerator::MAP_W + p.x;
    };

    gScore[key(start)] = 0.f;
    open.push({start, heuristic(start, end), 0.f});

    // 4 направления движения
    std::vector<sf::Vector2i> dirs = {{1,0},{-1,0},{0,1},{0,-1}};

    int iterations = 0;
    const int MAX_ITER = 1000;  // ограничение чтобы не тормозило

    while (!open.empty() && iterations < MAX_ITER) {
        iterations++;
        PathNode current = open.top();
        open.pop();

        if (current.pos == end) {
            std::vector<sf::Vector2i> path;
            sf::Vector2i cur = end;
            while (cur != start) {
                path.push_back(cur);
                cur = cameFrom[key(cur)];
            }
            std::reverse(path.begin(), path.end());
            return path;
        }

        for (auto& dir : dirs) {
            sf::Vector2i next = current.pos + dir;
            if (!isWalkable(next, map)) continue;

            float newG = gScore[key(current.pos)] + 1.f;

            if (gScore.find(key(next)) == gScore.end() ||
                newG < gScore[key(next)])
            {
                gScore[key(next)] = newG;
                float f = newG + heuristic(next, end);
                open.push({next, f, newG});
                cameFrom[key(next)] = current.pos;
            }
        }
    }

    return {};
}