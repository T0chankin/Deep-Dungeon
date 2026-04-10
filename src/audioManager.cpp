#include "audioManager.hpp"
#include <iostream>

// Загружаем буфер и сразу привязываем к sound
bool AudioManager::loadSound(sf::SoundBuffer& buffer, sf::Sound& sound,
                              const std::string& path)
{
    if (!buffer.loadFromFile(path)) {
        std::cout << "Звук не найден: " << path << "\n";
        return false;
    }
    sound.setBuffer(buffer);
    return true;
}

AudioManager::AudioManager()
    : hoverSound(hoverBuffer),
      pickupSound(pickupBuffer),
      healSound(healBuffer),
      swordSound(swordBuffer),
      bowSound(bowBuffer),
      fireballSound(fireballBuffer),
      monsterAttackSound(monsterAttackBuffer)
{
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

void AudioManager::playHover()         { hoverSound.play(); }
void AudioManager::playPickup()        { pickupSound.play(); }
void AudioManager::playHeal()          { healSound.play(); }
void AudioManager::playSword()         { swordSound.play(); }
void AudioManager::playBow()           { bowSound.play(); }
void AudioManager::playFireball()      { fireballSound.play(); }
void AudioManager::playMonsterAttack() { monsterAttackSound.play(); }

void AudioManager::playMenuMusic() {
    gameMusic.stop();
    menuMusic.play();
}

void AudioManager::playGameMusic() {
    menuMusic.stop();
    gameMusic.play();
}

void AudioManager::stopMusic() {
    menuMusic.stop();
    gameMusic.stop();
}

void AudioManager::setMusicVolume(float vol) {
    musicVolume = vol;
    menuMusic.setVolume(vol);
    gameMusic.setVolume(vol);
}

void AudioManager::setSoundVolume(float vol) {
    soundVolume = vol;
    hoverSound.setVolume(vol);
    pickupSound.setVolume(vol);
    healSound.setVolume(vol);
    swordSound.setVolume(vol);
    bowSound.setVolume(vol);
    fireballSound.setVolume(vol);
    monsterAttackSound.setVolume(vol);
}