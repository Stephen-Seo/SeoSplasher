
#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include <random>

#include <SFML/Graphics.hpp>

#include "resourceIdentifiers.hpp"

class ResourceManager;
class MusicPlayer;
class SoundPlayer;
class Engine;


struct Context
{
    Context(sf::RenderWindow& window,
            ResourceManager& resourceManager,
            MusicPlayer& mPlayer,
            SoundPlayer& sPlayer,
            Engine& ecEngine,
            std::mt19937& rGen,
            bool& isQuitting,
            unsigned char& mode);

    sf::RenderWindow* window;
    ResourceManager* resourceManager;
    MusicPlayer* mPlayer;
    SoundPlayer* sPlayer;
    Engine* ecEngine;
    std::mt19937* rGen;
    bool* isQuitting;
    /**
     * 0000 = computers
     * 0001 = singleplayer
     * 0010 = 2 player multiplayer
     * 0011 = 3 player multiplayer
     * 0100 = 4 player multiplayer
     * 1000 = is server
    **/
    unsigned char* mode;
};

#endif
