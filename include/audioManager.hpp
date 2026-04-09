#pragma once
#include <SFML/Audio.hpp>

class AudioManager {
public:
    AudioManager();

    void playHover();
    void playMenuMusic();
    void playGameMusic();
    void stopMusic();

    void setMusicVolume(float vol);   
    void setSoundVolume(float vol);  

    float getMusicVolume() const { return musicVolume; }
    float getSoundVolume() const { return soundVolume; }

private:
    sf::Music menuMusic;
    sf::Music gameMusic;

    sf::SoundBuffer hoverBuffer;
    sf::Sound hoverSound{hoverBuffer};

    float musicVolume = 50.f;
    float soundVolume = 50.f;
};