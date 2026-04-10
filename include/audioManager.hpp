#pragma once
#include <SFML/Audio.hpp>
#include <string>

class AudioManager {
public:
    AudioManager();

    void playHover();
    void playMenuMusic();
    void playGameMusic();
    void stopMusic();

    void playPickup();
    void playHeal();
    void playSword();
    void playBow();
    void playFireball();
    void playMonsterAttack();

    void setMusicVolume(float vol);
    void setSoundVolume(float vol);

    float getMusicVolume() const { return musicVolume; }
    float getSoundVolume() const { return soundVolume; }

private:
    sf::Music menuMusic;
    sf::Music gameMusic;

    // Буферы
    sf::SoundBuffer hoverBuffer;
    sf::SoundBuffer pickupBuffer;
    sf::SoundBuffer healBuffer;
    sf::SoundBuffer swordBuffer;
    sf::SoundBuffer bowBuffer;
    sf::SoundBuffer fireballBuffer;
    sf::SoundBuffer monsterAttackBuffer;

    // Звуки — инициализируем ПОСЛЕ загрузки буферов
    sf::Sound hoverSound;
    sf::Sound pickupSound;
    sf::Sound healSound;
    sf::Sound swordSound;
    sf::Sound bowSound;
    sf::Sound fireballSound;
    sf::Sound monsterAttackSound;

    float musicVolume = 50.f;
    float soundVolume = 50.f;

    bool loadSound(sf::SoundBuffer& buffer, sf::Sound& sound,
                   const std::string& path);
};