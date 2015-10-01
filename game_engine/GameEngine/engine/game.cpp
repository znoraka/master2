#include "game.h"

Game *Game::instance = new Game();

Game *Game::getInstance()
{
    return instance;
}

void Game::update(float delta)
{
    if(!this->paused) {
        this->currentScene->update(delta);
    }
}

void Game::pause()
{
    this->paused = true;
}

void Game::resume()
{
    this->paused = false;
}

void Game::setScene(Scene *scene)
{
    this->currentScene = scene;
}

Game::Game()
{
    paused = false;
}
