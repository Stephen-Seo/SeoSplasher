
#ifndef C_PATH_FINDER_HPP
#define C_PATH_FINDER_HPP

#define PF_UPDATE_TIME 0.5f

#include "../ec/component.hpp"
#include "AI/pathFinder.hpp"

struct cPathFinder : Component
{
    PathFinder pf;

    float timer = PF_UPDATE_TIME;
};

#endif
