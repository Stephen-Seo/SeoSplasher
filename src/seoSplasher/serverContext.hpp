
#ifndef SERVER_CONTEXT_HPP
#define SERVER_CONTEXT_HPP

#include <map>
#include <vector>
#include <string>

#include "splashNetworkIdentifiers.hpp"
#include "../ec/cPosition.hpp"
#include "../ec/cVelocity.hpp"

struct ServerContext
{
    bool playersAlive[4];

    cPosition* ppositions[4];
    cVelocity* pvelocities[4];
    float movementTime[4];

    std::map<int, BalloonInfo> balloons;

    std::map<int, sf::Uint8> explosions;

    std::map<int, PowerupInfo> powerups;

    std::vector<sf::Uint8> breakables;

    SS::GameState gameState;

    float startTimer;

    std::string customNames[4];
};

#endif
