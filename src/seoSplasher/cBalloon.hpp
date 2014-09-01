
#ifndef C_BALLOON_HPP
#define C_BALLOON_HPP

#include "../ec/component.hpp"

struct cBalloon : Component
{
    bool piercing = false;
    bool spreading = false;
    bool ghosting = false;
};

#endif
