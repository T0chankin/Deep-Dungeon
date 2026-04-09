#include "options.hpp"

Options::Options(sf::RenderWindow& window, sf::Font& font, AudioManager& audio)
    : window(window), font(font), audio(audio),
      title(font, "OPTIONS", 52),
      hint(font, "UP/DOWN - выбор    LEFT/RIGHT - громкость    ESC - назад", 18),
      musicLabel(font, "", 30),
      soundLabel(font, "", 30)
{
    bg.setSize(sf::Vector2f(window.getSize()));
    bg.setFillColor(sf::Color(10, 10, 20));

    title.setFillColor(sf::Color(220, 50, 50));
    title.setStyle(sf::Text::Bold);
    auto tb = title.getLocalBounds();
    title.setOrigin({tb.size.x / 2.f, tb.size.y / 2.f});
    title.setPosition({window.getSize().x / 2.f, 100.f});

    hint.setFillColor(sf::Color(100, 100, 100));
    auto hb = hint.getLocalBounds();
    hint.setOrigin({hb.size.x / 2.f, hb.size.y / 2.f});
    hint.setPosition({window.getSize().x / 2.f, window.getSize().y - 50.f});

    updateLabels();
}

void Options::updateLabels() {
    musicLabel.setString("Music volume:  " + std::to_string((int)audio.getMusicVolume()));
    soundLabel.setString("Sound volume:  " + std::to_string((int)audio.getSoundVolume()));

    musicLabel.setFillColor(selected == 0 ? sf::Color(255, 215, 0) : sf::Color(180, 180, 180));
    soundLabel.setFillColor(selected == 1 ? sf::Color(255, 215, 0) : sf::Color(180, 180, 180));

    musicLabel.setStyle(selected == 0 ? sf::Text::Bold : sf::Text::Regular);
    soundLabel.setStyle(selected == 1 ? sf::Text::Bold : sf::Text::Regular);

    auto mb = musicLabel.getLocalBounds();
    musicLabel.setOrigin({mb.size.x / 2.f, mb.size.y / 2.f});
    musicLabel.setPosition({window.getSize().x / 2.f, 250.f});

    auto sb = soundLabel.getLocalBounds();
    soundLabel.setOrigin({sb.size.x / 2.f, sb.size.y / 2.f});
    soundLabel.setPosition({window.getSize().x / 2.f, 340.f});
}

void Options::handleEvent(const sf::Event& event, bool& goBack) {
    if (const auto* key = event.getIf<sf::Event::KeyPressed>()) {
        if (key->code == sf::Keyboard::Key::Escape)
            goBack = true;

        if (key->code == sf::Keyboard::Key::Up)
            selected = (selected - 1 + 2) % 2;  

        if (key->code == sf::Keyboard::Key::Down)
            selected = (selected + 1) % 2;

        if (key->code == sf::Keyboard::Key::Left) {
            if (selected == 0)
                audio.setMusicVolume(std::max(0.f, audio.getMusicVolume() - 5.f));
            else
                audio.setSoundVolume(std::max(0.f, audio.getSoundVolume() - 5.f));
        }

        if (key->code == sf::Keyboard::Key::Right) {
            if (selected == 0)
                audio.setMusicVolume(std::min(100.f, audio.getMusicVolume() + 5.f));
            else
                audio.setSoundVolume(std::min(100.f, audio.getSoundVolume() + 5.f));
        }

        updateLabels();
    }
}

void Options::draw() {
    window.draw(bg);
    window.draw(title);
    window.draw(hint);
    window.draw(musicLabel);
    window.draw(soundLabel);
}