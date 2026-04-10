#include "game.hpp"
#include <stdexcept>
const int WINDOW_WIDTH=1920;
const int WINDOW_HIGHT=1080;
Game::Game()
    : window(sf::VideoMode({WINDOW_HIGHT,WINDOW_WIDTH}), "Deep Dungeon",
             sf::Style::Default, sf::State::Windowed,
             sf::ContextSettings{0, 0, 8}),
      state(GameState::Menu)
{
    window.setFramerateLimit(60);

    if (!font.openFromFile("NintendoGameboy-Regular.otf"))
        throw std::runtime_error("Шрифт не найден!");

    audio = new AudioManager();
    menu = new MainMenu(window, font, *audio);
    options = new Options(window, font, *audio);
    dungeon = new Dungeon(window, font);
    scoreScreen = new ScoreScreen(window, font);
    audio->playMenuMusic();
}

void Game::run() {
    sf::Clock clock;
    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        handleEvents();
        update(dt);
        render();
    }
}

void Game::handleEvents() {
    while (auto event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>())
            window.close();

        if (state == GameState::Menu) {
            MenuResult result = menu->handleEvent(*event);
            if (result == MenuResult::NewGame) {
                delete dungeon;
                dungeon = new Dungeon(window, font);
                state = GameState::Playing;
                audio->playGameMusic();
            }
            if (result == MenuResult::Options)
                state = GameState::Options;
            if (result == MenuResult::Exit)
                window.close();
        }

        if (state == GameState::Options) {
            bool goBack = false;
            options->handleEvent(*event, goBack);
            if (goBack)
                state = GameState::Menu;
        }

        if (state == GameState::Playing) {
            dungeon->handleEvent(*event);

            if (const auto* key = event->getIf<sf::Event::KeyPressed>())
                if (key->code == sf::Keyboard::Key::Escape) {
                    state = GameState::Menu;
                    audio->playMenuMusic();
                }
        }
        if (state == GameState::Score) {
            bool goMenu = false;
            scoreScreen->handleEvent(*event, goMenu);
            if (goMenu) {
                state = GameState::Menu;
                audio->playMenuMusic();
            }
        }
    }
}

void Game::update(float dt) {
    if (state == GameState::Playing) {
        dungeon->update(dt);

        if (dungeon->isFinished()) {
            scoreScreen->show(ScoreType::Win,
                              dungeon->getCoins(),
                              dungeon->getCurrentFloor());
            state = GameState::Score;
            audio->playMenuMusic();
        }

        if (dungeon->isPlayerDead()) {
            scoreScreen->show(ScoreType::Death,
                              dungeon->getCoins(),
                              dungeon->getCurrentFloor());
            state = GameState::Score;
            audio->playMenuMusic();
        }
    }
}

void Game::render() {
    window.clear(sf::Color::Black);

    if (state == GameState::Menu)
        menu->draw();
    else if (state == GameState::Options)
        options->draw();
    else if (state == GameState::Playing)
        dungeon->draw();
    else if (state == GameState::Score)
        scoreScreen->draw();

    window.display();
}