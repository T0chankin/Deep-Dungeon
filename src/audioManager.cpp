#include "audioManager.hpp"
#include <iostream>

void AudioManager::loadSound(sf::SoundBuffer& buffer,
                              std::unique_ptr<sf::Sound>& sound,
                              const std::string& path)
{
    if (!buffer.loadFromFile(path)) {
        std::cout << "Звук не найден: " << path << "\n";
        return;
    }
    // Создаём Sound только после успешной загрузки буфера
    sound = std::make_unique<sf::Sound>(buffer);
}

void AudioManager::playSound(std::unique_ptr<sf::Sound>& sound) {
    if (sound) sound->play();
}

AudioManager::AudioManager() {
    loadSound(hoverBuffer,         hoverSound,         "assets/sounds/selectSound.wav");
    loadSound(pickupBuffer,        pickupSound,        "assets/sounds/pickup.mp3");
    loadSound(healBuffer,          healSound,          "assets/sounds/heal.mp3");
    loadSound(swordBuffer,         swordSound,         "assets/sounds/sword.mp3");
    loadSound(bowBuffer,           bowSound,           "assets/sounds/bow.mp3");
    loadSound(fireballBuffer,      fireballSound,      "assets/sounds/fireball.mp3");
    loadSound(monsterAttackBuffer, monsterAttackSound, "assets/sounds/monster_atack.mp3");

    if (!menuMusic.openFromFile("assets/sounds/mainMenuMusic.mp3"))
        std::cout << "mainMenuMusic.mp3 не найден\n";
    else {
        menuMusic.setLooping(true);
        menuMusic.setVolume(musicVolume);
    }

    if (!gameMusic.openFromFile("assets/sounds/gameMusic.mp3"))
        std::cout << "gameMusic.mp3 не найден\n";
    else {
        gameMusic.setLooping(true);
        gameMusic.setVolume(musicVolume);
    }
}

void AudioManager::playHover()         { playSound(hoverSound); }
void AudioManager::playPickup()        { playSound(pickupSound); }
void AudioManager::playHeal()          { playSound(healSound); }
void AudioManager::playSword()         { playSound(swordSound); }
void AudioManager::playBow()           { playSound(bowSound); }
void AudioManager::playFireball()      { playSound(fireballSound); }
void AudioManager::playMonsterAttack() { playSound(monsterAttackSound); }

void AudioManager::playMenuMusic() { gameMusic.stop(); menuMusic.play(); }
void AudioManager::playGameMusic() { menuMusic.stop(); gameMusic.play(); }
void AudioManager::stopMusic()     { menuMusic.stop(); gameMusic.stop(); }

void AudioManager::setMusicVolume(float vol) {
    musicVolume = vol;
    menuMusic.setVolume(vol);
    gameMusic.setVolume(vol);
}

void AudioManager::setSoundVolume(float vol) {
    soundVolume = vol;
    if (hoverSound)         hoverSound->setVolume(vol);
    if (pickupSound)        pickupSound->setVolume(vol);
    if (healSound)          healSound->setVolume(vol);
    if (swordSound)         swordSound->setVolume(vol);
    if (bowSound)           bowSound->setVolume(vol);
    if (fireballSound)      fireballSound->setVolume(vol);
    if (monsterAttackSound) monsterAttackSound->setVolume(vol);
}