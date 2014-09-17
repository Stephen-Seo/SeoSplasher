
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
    unsigned char* mode;
    /** mode
     * 0 = local singleplayer
     * 1 = networked client multiplayer
     * 2 = server multiplayer
     * 3 = dedicated server multiplayer
     * 4 = dedicated server multiplayer (no draw)
    **/
};

#endif
