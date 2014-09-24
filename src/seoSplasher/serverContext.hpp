
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
    bool placedBalloon[4];
    int playerEID[4];

/*
    On server, balloons map EIDs (Entity IDs) to info pertinent to that entity
    On client, balloons map SEIDs (Server Entity IDs) to info pertinent to that entity
*/
    std::map<int, BalloonInfo> balloons;

    std::map<int, ExplosionInfo> explosions;
    std::map<sf::Uint8, int> explosionXYToEID;

    std::map<int, PowerupInfo> powerups;
    std::map<sf::Uint8, int> powerupXYToEID;

    std::vector<sf::Uint8> breakables;
    std::map<sf::Uint8, int> breakableXYToEID;

    SS::GameState gameState;

    float startTimer;

    std::string customNames[4];
    std::string ownName;

    std::string serverIP;
};

#endif
