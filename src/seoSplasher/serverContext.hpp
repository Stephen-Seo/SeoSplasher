
#ifndef SERVER_CONTEXT_HPP
#define SERVER_CONTEXT_HPP

#include <map>

#include "splashNetworkIdentifiers.hpp"
#include "../ec/cPosition.hpp"
#include "../ec/cVelocity.hpp"

struct ServerContext
{
    bool playersAlive[4];

    cPosition* ppositions[4];
    cVelocity* pvelocities[4];
    float moveTime[4];

    std::map<int, BalloonInfo> balloons;

    std::map<int, int> explosions;

    std::map<int, PowerupInfo> powerups;
};

#endif
