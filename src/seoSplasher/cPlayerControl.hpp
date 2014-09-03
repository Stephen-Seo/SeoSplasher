
#ifndef C_PLAYER_CONTROL_HPP
#define C_PLAYER_CONTROL_HPP

#include "../ec/component.hpp"

struct cPlayerControl : Component
{
    enum Direction {
        NONE,
        RIGHT,
        LEFT,
        UP,
        DOWN
    };

    cPlayerControl(Direction* dir, bool* place, bool* placeAction, bool* kick, bool* kickAction, unsigned char ID, bool* cFired);

    Direction* dir;

    unsigned char ID;

    bool* placeBalloon;
    bool* placeAction;

    bool* kick;
    bool* kickAction;

    bool* cFired;
};

#endif
