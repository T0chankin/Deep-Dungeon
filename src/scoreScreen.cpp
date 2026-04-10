#include "scoreScreen.hpp"

ScoreScreen::ScoreScreen(sf::RenderWindow& window, sf::Font& font)
    : window(window), font(font),
      titleText(font, "YOU WIN!", 64),
      coinsText(font, "Coins: 0", 40),
      hintText(font, "Press ENTER to return to menu", 24)
{
    bg.setSize(sf::Vector2f(window.getSize()));
    bg.setFillColor(sf::Color(10, 10, 20));

    titleText.setFillColor(sf::Color(255, 215, 0));
    titleText.setStyle(sf::Text::Bold);
    auto tb = titleText.getLocalBounds();
    titleText.setOrigin({tb.size.x / 2.f, tb.size.y / 2.f});
    titleText.setPosition({window.getSize().x / 2.f, 150.f});

    coinsText.setFillColor(sf::Color::White);
    auto cb = coinsText.getLocalBounds();
    coinsText.setOrigin({cb.size.x / 2.f, cb.size.y / 2.f});
    coinsText.setPosition({window.getSize().x / 2.f, 300.f});

    hintText.setFillColor(sf::Color(120, 120, 120));
    auto hb = hintText.getLocalBounds();
    hintText.setOrigin({hb.size.x / 2.f, hb.size.y / 2.f});
    hintText.setPosition({window.getSize().x / 2.f, 450.f});
}

void ScoreScreen::setCoins(int c) {
    coins = c;
    coinsText.setString("Coins collected: " + std::to_string(coins));
    auto cb = coinsText.getLocalBounds();
    coinsText.setOrigin({cb.size.x / 2.f, cb.size.y / 2.f});
    coinsText.setPosition({window.getSize().x / 2.f, 300.f});
}

void ScoreScreen::handleEvent(const sf::Event& event, bool& goMenu) {
    if (const auto* key = event.getIf<sf::Event::KeyPressed>())
        if (key->code == sf::Keyboard::Key::Enter)
            goMenu = true;
}

void ScoreScreen::draw() {
    window.draw(bg);
    window.draw(titleText);
    window.draw(coinsText);
    window.draw(hintText);
}