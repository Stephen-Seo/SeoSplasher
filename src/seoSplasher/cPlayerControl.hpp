
#ifndef C_PLAYER_CONTROL_HPP
#define C_PLAYER_CONTROL_HPP

#include "../ec/component.hpp"

#include "direction.hpp"

struct cPlayerControl : Component
{
    cPlayerControl(Direction::Direction* dir, bool* place, bool* placeAction, bool* kick, bool* kickAction, unsigned char ID, bool* cFired);

    Direction::Direction* dir;

    unsigned char ID;

    bool* placeBalloon;
    bool* placeAction;

    bool* kick;
    bool* kickAction;

    bool* cFired;
};

#endif
