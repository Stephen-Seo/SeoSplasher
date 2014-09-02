
#ifndef C_POWERUP_HPP
#define C_POWERUP_HPP

#include "../ec/component.hpp"

struct cPowerup : Component
{
    enum Powerup {
        NONE,
        BALLOON_UP,
        RANGE_UP,
        SPEED_UP,
        KICK_UPGRADE,
        RCONTROL_UPGRADE,
        SBALLOON_UPGRADE,
        PIERCE_UPGRADE,
        SPREAD_UPGRADE,
        GHOST_UPGRADE
    };

    Powerup powerup;
};

#endif
