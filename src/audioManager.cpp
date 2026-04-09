#include "audioManager.hpp"
#include <stdexcept>

AudioManager::AudioManager() {
    if (!hoverBuffer.loadFromFile("assets/sounds/selectSound.wav"))
        throw std::runtime_error("Не найден selectSound.wav");

    hoverSound.setBuffer(hoverBuffer);
    hoverSound.setVolume(soundVolume);

    if (!menuMusic.openFromFile("assets/sounds/mainMenuMusic.mp3"))
        throw std::runtime_error("Не найден mainMenuMusic.mp3");

    menuMusic.setLooping(true);
    menuMusic.setVolume(musicVolume);

    if (!gameMusic.openFromFile("assets/sounds/gameMusic.mp3"))
        throw std::runtime_error("Не найден gameMusic.mp3");
    gameMusic.setLooping(true);
    gameMusic.setVolume(musicVolume);
}

void AudioManager::playHover() {
    hoverSound.play();
}

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
}