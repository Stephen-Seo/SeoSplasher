
#ifndef C_BALLOON_HPP
#define C_BALLOON_HPP

#define BALLOON_ALIVE_TIME 3.0F
#define BALLOON_KICK_SPEED 200.0F

#include "../ec/component.hpp"

struct cBalloon : Component
{
    unsigned char ID;
    bool super = false;
    bool hit = false;

    unsigned char* balloonsInPlay = nullptr;
    unsigned char* sBalloonsInPlay = nullptr;

    unsigned char range;
    bool piercing = false;
    bool ghosting = false;
};

#endif
