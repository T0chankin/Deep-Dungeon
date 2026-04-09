#include "dungeon.hpp"
#include <cmath>

void Dungeon::drawTestSquare() {
    sf::RectangleShape square;
    square.setSize({50.f, 50.f});
    square.setFillColor(sf::Color::Red);
    square.setOrigin({25.f, 25.f});
    square.setPosition({0.f, 0.f});
    window.draw(square);
}

Dungeon::Dungeon(sf::RenderWindow& window, sf::Font& font)
    : window(window), font(font)
{
    camera = sf::View(sf::FloatRect({0.f, 0.f},
        {(float)window.getSize().x, (float)window.getSize().y}));

    player.setPos({400.f, 300.f});
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

    if (len != 0.f)
        player.move(dir * dt);
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
    updateCamera(dt);
}

void Dungeon::draw() {
    window.setView(camera);
    player.draw(window);
    drawTestSquare();
    window.setView(window.getDefaultView());
}