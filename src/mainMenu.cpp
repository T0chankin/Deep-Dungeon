#include "mainMenu.hpp"

MainMenu::MainMenu(sf::RenderWindow& window, sf::Font& font, AudioManager& audio)
    : window(window), font(font), audio(audio), title(font, "DEEP DUNGEON", 64)
{
    bg.setSize(sf::Vector2f(window.getSize()));
    bg.setFillColor(sf::Color(10, 10, 20));

    title.setFillColor(sf::Color(220, 50, 50));
    title.setStyle(sf::Text::Bold);
    auto tb = title.getLocalBounds();
    title.setOrigin({tb.size.x / 2.f, tb.size.y / 2.f});
    title.setPosition({window.getSize().x / 2.f, 160.f});

    std::vector<std::string> labels = {"New Game", "Load Game", "Options", "Exit"};
    float startY = 320.f;
    for (size_t i = 0; i < labels.size(); ++i) {
        sf::Text item(font, labels[i], 36);
        item.setFillColor(sf::Color(180, 180, 180));
        auto lb = item.getLocalBounds();
        item.setOrigin({lb.size.x / 2.f, lb.size.y / 2.f});
        item.setPosition({window.getSize().x / 2.f, startY + i * 70.f});
        items.push_back(item);
    }
    updateColors();
}

void MainMenu::updateColors() {
    for (int i = 0; i < (int)items.size(); ++i) {
        if (i == selected) {
            items[i].setFillColor(sf::Color(255, 215, 0));
            items[i].setStyle(sf::Text::Bold);
        } else {
            items[i].setFillColor(sf::Color(180, 180, 180));
            items[i].setStyle(sf::Text::Regular);
        }
    }
}

MenuResult MainMenu::handleEvent(const sf::Event& event) {
    if (const auto* key = event.getIf<sf::Event::KeyPressed>()) {
        if (key->code == sf::Keyboard::Key::Up) {
            selected = (selected - 1 + items.size()) % items.size();
            audio.playHover();   // <-- звук
            updateColors();
        } else if (key->code == sf::Keyboard::Key::Down) {
            selected = (selected + 1) % items.size();
            audio.playHover();   // <-- звук
            updateColors();
        } else if (key->code == sf::Keyboard::Key::Enter) {
            switch (selected) {
                case 0: return MenuResult::NewGame;
                case 1: return MenuResult::LoadGame;
                case 2: return MenuResult::Options;
                case 3: return MenuResult::Exit;
            }
        }
    }
    return MenuResult::None;
}

void MainMenu::draw() {
    window.draw(bg);
    window.draw(title);
    for (auto& item : items)
        window.draw(item);
}