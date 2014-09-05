
#ifndef C_BALLOON_HPP
#define C_BALLOON_HPP

#define BALLOON_ALIVE_TIME 3.0f

#include "../ec/component.hpp"

struct cBalloon : Component
{
    unsigned char ID;
    bool piercing = false;
    bool spreading = false;
    bool ghosting = false;
    unsigned char distance;
    bool super = false;
    bool hit = false;

    unsigned char* balloonsInPlay = nullptr;
    unsigned char* sBalloonsInPlay = nullptr;

    std::list<int> wIndicators;
};

#endif
