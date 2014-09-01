
#ifndef C_LIVING_HPP
#define C_LIVING_HPP

#define DEFAULT_BALLOONS 1
#define DEFAULT_RANGE 3
#define DEFAULT_SPEED 15.0f

#include "../ec/component.hpp"

struct cLiving : Component
{
    unsigned char ID;
    bool hit = false;
    unsigned char balloonUp = 0;
    unsigned char rangeUp = 0;
    unsigned char speedUp = 0;
    unsigned char kickUpgrade = 0;
    unsigned char rControlUpgrade = 0;
    unsigned char sBalloonUpgrade = 0;
    unsigned char pierceUpgrade = 0;
    unsigned char spreadUpgrade = 0;
    unsigned char ghostUpgrade = 0;
};

#endif
