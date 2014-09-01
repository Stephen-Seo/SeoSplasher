
#ifndef C_DAMAGE_HPP
#define C_DAMAGE_HPP

#include "../ec/component.hpp"

struct cDamage : Component
{
    bool piercing = false;
    bool spreading = false;
    bool ghosting = false;
};

#endif
