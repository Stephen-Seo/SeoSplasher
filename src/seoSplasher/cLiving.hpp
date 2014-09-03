
#ifndef C_LIVING_HPP
#define C_LIVING_HPP

#define DEFAULT_BALLOONS 1
#define DEFAULT_RANGE 2
#define DEFAULT_SPEED 80.0f
#define SPEED_UP_MULT 15.0f
#define SUPER_RANGE 10

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

    unsigned char balloonsInPlay = 0;
    unsigned char sBalloonsInPlay = 0;
};

#endif
