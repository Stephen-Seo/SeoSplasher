
#ifndef C_BALLOON_HPP
#define C_BALLOON_HPP

#define BALLOON_ALIVE_TIME 3.0f
#define BALLOON_KICK_SPEED 120.0f

#include "../ec/component.hpp"

struct cBalloon : Component
{
    unsigned char ID;
    bool super = false;
    bool hit = false;

    unsigned char* balloonsInPlay = nullptr;
    unsigned char* sBalloonsInPlay = nullptr;

    std::list<int> wIndicators;
};

#endif
