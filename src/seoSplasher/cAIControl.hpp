
#ifndef C_AI_CONTROL_HPP
#define C_AI_CONTROL_HPP

#include "../ec/component.hpp"

#include "AI/uai.hpp"

class PathFinder;

struct cAIControl : Component
{
    unsigned char ID;

    PathFinder* pf;
    UAI ai;

    float aiTickTime = 0.3F;
    float timer = 0.3F;

    AI::Action currentAction = AI::NONE;

    bool fired;
};

#endif
