#include "hud.hpp"

Hud::Hud(sf::RenderWindow& window, sf::Font& font)
    : window(window), font(font),
      hpText(font, "HP", 16),
      manaText(font, "MP", 16),
      coinsText(font, "Coins: 0", 16),
      weaponText(font, "Weapon", 16),
      hintsText(font, "Q-heal  E-mana  1/2/3-weapon", 12)
{
    float W = window.getSize().x;
    float H = window.getSize().y;

    bg.setSize({W, hudHeight});
    bg.setFillColor(sf::Color(20, 20, 30, 220));
    bg.setPosition({0.f, H - hudHeight});

    hpBarBg.setSize({200.f, 18.f});
    hpBarBg.setFillColor(sf::Color(60, 20, 20));
    hpBarBg.setPosition({10.f, H - hudHeight + 10.f});

    hpBar.setSize({200.f, 18.f});
    hpBar.setFillColor(sf::Color(200, 50, 50));
    hpBar.setPosition({10.f, H - hudHeight + 10.f});

    manaBarBg.setSize({200.f, 18.f});
    manaBarBg.setFillColor(sf::Color(20, 20, 60));
    manaBarBg.setPosition({10.f, H - hudHeight + 35.f});

    manaBar.setSize({200.f, 18.f});
    manaBar.setFillColor(sf::Color(50, 80, 200));
    manaBar.setPosition({10.f, H - hudHeight + 35.f});

    hpText.setFillColor(sf::Color::White);
    hpText.setPosition({15.f, H - hudHeight + 10.f});

    manaText.setFillColor(sf::Color::White);
    manaText.setPosition({15.f, H - hudHeight + 35.f});

    coinsText.setFillColor(sf::Color(255, 215, 0));
    coinsText.setPosition({10.f, H - hudHeight + 58.f});

    weaponText.setFillColor(sf::Color(200, 200, 200));
    weaponText.setPosition({W / 2.f - 60.f, H - hudHeight + 15.f});

    hintsText.setFillColor(sf::Color(120, 120, 120));
    hintsText.setPosition({W / 2.f - 60.f, H - hudHeight + 50.f});
}

void Hud::update(Player& player) {
    float W = window.getSize().x;
    float H = window.getSize().y;

    float hpRatio = player.getHp() / player.getMaxHp();
    hpBar.setSize({200.f * hpRatio, 18.f});

    float manaRatio = player.getMana() / player.getMaxMana();
    manaBar.setSize({200.f * manaRatio, 18.f});

    hpText.setString("HP  " + std::to_string((int)player.getHp())
                     + " / " + std::to_string((int)player.getMaxHp()));
    manaText.setString("MP  " + std::to_string((int)player.getMana())
                       + " / " + std::to_string((int)player.getMaxMana()));
    coinsText.setString("Coins: " + std::to_string(player.getCoins()));

    Weapon w = player.getCurrentWeapon();
    weaponText.setString("Weapon [1/2/3]: " + w.name);

    int hpPots   = 0;
    int manaPots = 0;
    for (auto& item : player.getInventory()) {
        if (item.type == ItemType::HealthPotion) hpPots++;
        if (item.type == ItemType::ManaPotion)   manaPots++;
    }
    hintsText.setString("Q-heal(" + std::to_string(hpPots) + ")"
                        + "  E-mana(" + std::to_string(manaPots) + ")"
                        + "  SPACE-dash");
}

void Hud::draw() {
    window.draw(bg);
    window.draw(hpBarBg);
    window.draw(hpBar);
    window.draw(manaBarBg);
    window.draw(manaBar);
    window.draw(hpText);
    window.draw(manaText);
    window.draw(coinsText);
    window.draw(weaponText);
    window.draw(hintsText);
}