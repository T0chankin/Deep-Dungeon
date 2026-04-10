#include "dungeon.hpp"
#include <cmath>

Dungeon::Dungeon(sf::RenderWindow& window, sf::Font& font)
    : window(window), font(font)
{
    camera = sf::View(sf::FloatRect({0.f, 0.f},
        {(float)window.getSize().x/3, (float)window.getSize().y/3}));

    mapGen.generate(currentFloor);
    player.setPos(mapGen.getPlayerSpawn());
    camera.setCenter(mapGen.getPlayerSpawn());
}

void Dungeon::handleEvent(const sf::Event& event) {
    if (const auto* click = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (click->button == sf::Mouse::Button::Left) {
            sf::Vector2f mousePos = window.mapPixelToCoords(
                sf::Mouse::getPosition(window), camera);
            player.attack(mousePos);
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

    updateCamera(dt);
    checkHatch();
}

void Dungeon::draw() {
    window.setView(camera);
    mapGen.draw(window);
    player.draw(window);
    window.setView(window.getDefaultView());
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

bool Dungeon::isFinished() const { return finished; }
int Dungeon::getCoins() const    { return player.getCoins(); }