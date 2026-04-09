#pragma once
#include <SFML/Graphics.hpp>
#include "audioManager.hpp"

class Options {
public:
    Options(sf::RenderWindow& window, sf::Font& font, AudioManager& audio);
    void handleEvent(const sf::Event& event, bool& goBack);
    void draw();

private:
    sf::RenderWindow& window;
    sf::Font& font;
    AudioManager& audio;

    sf::RectangleShape bg;
    sf::Text title;
    sf::Text hint;

    sf::Text musicLabel;
    sf::Text soundLabel;

    int selected=0;

    void updateLabels();
};