
#ifndef C_DAMAGE_HPP
#define C_DAMAGE_HPP

#define SPLOSION_LIFETIME 1.0f

#include "../ec/component.hpp"

struct cDamage : Component
{
    unsigned char ID;
    bool piercing = false;
    bool spreading = false;
    bool ghosting = false;
    unsigned char distance;

    bool vertical;
    bool horizontal;

    bool spawned = false;
};

#endif
