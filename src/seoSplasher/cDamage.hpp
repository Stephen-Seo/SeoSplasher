
#ifndef C_DAMAGE_HPP
#define C_DAMAGE_HPP

#define SPLOSION_LIFETIME 1.0f

#include "../ec/component.hpp"

struct cDamage : Component
{
    unsigned char ID;

    bool hitCheck = false;
};

#endif
