
#ifndef C_BALLOON_HPP
#define C_BALLOON_HPP

#define BALLOON_ALIVE_TIME 0.9f

#include "../ec/component.hpp"

struct cBalloon : Component
{
    unsigned char ID;
    bool piercing = false;
    bool spreading = false;
    bool ghosting = false;
    unsigned char distance;
};

#endif
