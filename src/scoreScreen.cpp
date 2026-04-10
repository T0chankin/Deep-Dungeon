#include "scoreScreen.hpp"

ScoreScreen::ScoreScreen(sf::RenderWindow& window, sf::Font& font)
    : window(window), font(font),
      titleText(font, "", 64),
      coinsText(font, "", 36),
      floorText(font, "", 28),
      hintText(font, "Press ENTER to return to menu", 22)
{
    bg.setSize(sf::Vector2f(window.getSize()));
    bg.setFillColor(sf::Color(10, 10, 20));

    hintText.setFillColor(sf::Color(120, 120, 120));
    auto hb = hintText.getLocalBounds();
    hintText.setOrigin({hb.size.x / 2.f, hb.size.y / 2.f});
    hintText.setPosition({window.getSize().x / 2.f, window.getSize().y - 80.f});
}

void ScoreScreen::show(ScoreType type, int coins, int floor) {
    float cx = window.getSize().x / 2.f;

    if (type == ScoreType::Win) {
        titleText.setString("YOU WIN!");
        titleText.setFillColor(sf::Color(255, 215, 0));
        bg.setFillColor(sf::Color(10, 20, 10));
    } else {
        titleText.setString("YOU DIED");
        titleText.setFillColor(sf::Color(200, 30, 30));
        bg.setFillColor(sf::Color(20, 5, 5));
    }

    titleText.setStyle(sf::Text::Bold);
    auto tb = titleText.getLocalBounds();
    titleText.setOrigin({tb.size.x / 2.f, tb.size.y / 2.f});
    titleText.setPosition({cx, 150.f});

    floorText.setString("Floor reached: " + std::to_string(floor));
    floorText.setFillColor(sf::Color(180, 180, 180));
    auto fb = floorText.getLocalBounds();
    floorText.setOrigin({fb.size.x / 2.f, fb.size.y / 2.f});
    floorText.setPosition({cx, 280.f});

    coinsText.setString("Coins collected: " + std::to_string(coins));
    coinsText.setFillColor(sf::Color(255, 215, 0));
    auto cb = coinsText.getLocalBounds();
    coinsText.setOrigin({cb.size.x / 2.f, cb.size.y / 2.f});
    coinsText.setPosition({cx, 340.f});
}

void ScoreScreen::handleEvent(const sf::Event& event, bool& goMenu) {
    if (const auto* key = event.getIf<sf::Event::KeyPressed>())
        if (key->code == sf::Keyboard::Key::Enter)
            goMenu = true;
}

void ScoreScreen::draw() {
    window.draw(bg);
    window.draw(titleText);
    window.draw(floorText);
    window.draw(coinsText);
    window.draw(hintText);
}