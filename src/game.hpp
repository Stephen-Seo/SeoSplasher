
#ifndef GAME_HPP
#define GAME_HPP

#include <random>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "ec/engine.hpp"
#include "resourceHolder.hpp"
#include "resourceIdentifiers.hpp"
#include "resourceManager.hpp"
#include "musicPlayer.hpp"
#include "soundPlayer.hpp"
#include "state.hpp"
#include "stateStack.hpp"
#include "seoSplasher/serverContext.hpp"
#include "seoSplasher/soundContext.hpp"

class Game
{
public:
    Game();
    void run();
private:
    void processEvents();
    void update(sf::Time deltaTime);
    void draw();

    void registerResources();
    void registerStates();

    sf::RenderWindow window;
    sf::Time frameTime;

    ResourceManager resourceManager;

    MusicPlayer mPlayer;
    SoundPlayer sPlayer;

    StateStack stateStack;

    bool isQuitting;

    Engine ecEngine;

    std::mt19937 rGen;

    unsigned char mode;

    ServerContext scontext;
    SoundContext sfxContext;
};

#endif
