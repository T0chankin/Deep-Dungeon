#pragma once
#include <SFML/Audio.hpp>
#include <string>
#include <memory>

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

    sf::SoundBuffer hoverBuffer;
    sf::SoundBuffer pickupBuffer;
    sf::SoundBuffer healBuffer;
    sf::SoundBuffer swordBuffer;
    sf::SoundBuffer bowBuffer;
    sf::SoundBuffer fireballBuffer;
    sf::SoundBuffer monsterAttackBuffer;

    // Указатели — создаём после загрузки буфера
    std::unique_ptr<sf::Sound> hoverSound;
    std::unique_ptr<sf::Sound> pickupSound;
    std::unique_ptr<sf::Sound> healSound;
    std::unique_ptr<sf::Sound> swordSound;
    std::unique_ptr<sf::Sound> bowSound;
    std::unique_ptr<sf::Sound> fireballSound;
    std::unique_ptr<sf::Sound> monsterAttackSound;

    float musicVolume = 50.f;
    float soundVolume = 50.f;

    void loadSound(sf::SoundBuffer& buffer,
                   std::unique_ptr<sf::Sound>& sound,
                   const std::string& path);
    void playSound(std::unique_ptr<sf::Sound>& sound);
};