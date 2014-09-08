
#include "uai.hpp"

#include <queue>
#include <iostream>

#include "pathFinder.hpp"
#include "../../ec/cPosition.hpp"
#include "../cLiving.hpp"
#include "../../ec/engine.hpp"
#include "../gridInfo.hpp"
#include "../utility.hpp"
#include "../cWIndicator.hpp"

ActionElement::ActionElement() :
action(AI::PANIC),
priority(0)
{}

ActionElement::ActionElement(AI::Action action, int priority) :
action(action),
priority(priority)
{}

bool ActionElement::operator< (const ActionElement& other) const
{
    return priority < other.priority;
}

UAI::UAI() :
wildness(DEFAULT_AI_WILDNESS),
isRisky(false)
{}

AI::Action UAI::determineAction(const cPosition& pos, const cLiving& living, PathFinder& pf, Engine& engine, std::mt19937& gen)
{
    std::priority_queue<ActionElement> levels;
    for(int i = 0; i < AI::SIZE; ++i)
    {
        levels.push(ActionElement(static_cast<AI::Action>(i),utility(static_cast<AI::Action>(i), pos, living, pf, engine)));
    }

    AI::Action picked[wildness];
    for(unsigned char i = 0; i < wildness; ++i)
    {
        picked[i] = levels.top().action;
        std::clog << "Action " << picked[i] << " of priority " << levels.top().priority << '\n';
        levels.pop();
    }

    if(wildness > 1)
    {
        std::normal_distribution<> dist(0,wildness / 2);
        int i = (int)(dist(gen) + 0.5);
        if(i >= wildness)
            return picked[wildness - 1];
        else if(i >= 0)
            return picked[i];
    }

    return picked[0];
}

void UAI::setWildness(unsigned char wildness)
{
    if(wildness == 0)
        this->wildness = 1;
    else
        this->wildness = wildness;
}

void UAI::setRisky(bool isRisky)
{
    this->isRisky = isRisky;
}

int UAI::utility(AI::Action action, const cPosition& pos, const cLiving& living, PathFinder& pf, Engine& engine)
{
    unsigned char info = nearbyInfo(pos, pf, engine);
    const unsigned char* grid = pf.getValidGrid(engine);
    int xy = (int)((pos.x + (float)(-GRID_OFFSET_X + GRID_SQUARE_SIZE / 2)) / GRID_SQUARE_SIZE) + (int)((pos.y + (float)(-GRID_OFFSET_Y + GRID_SQUARE_SIZE / 2)) / GRID_SQUARE_SIZE) * GRID_WIDTH;
    bool collidingWithWIndicator = false;

    bool adjacentToBreakable = (xy % GRID_WIDTH != 0 && (grid[xy - 1] & 0x4) != 0) || (xy % GRID_WIDTH != GRID_WIDTH - 1 && (grid[xy + 1] & 0x4) != 0) || (xy - GRID_WIDTH >= 0 && (grid[xy - GRID_WIDTH] & 0x4) != 0) || (xy + GRID_WIDTH < GRID_TOTAL && (grid[xy + GRID_WIDTH] & 0x4) != 0);
    bool adjacentToWIndicator = (xy % GRID_WIDTH != 0 && (grid[xy - 1] & 0x20) != 0) || (xy % GRID_WIDTH != GRID_WIDTH - 1 && (grid[xy + 1] & 0x20) != 0) || (xy - GRID_WIDTH >= 0 && (grid[xy - GRID_WIDTH] & 0x20) != 0) || (xy + GRID_WIDTH < GRID_TOTAL && (grid[xy + GRID_WIDTH] & 0x20) != 0);

    switch(action)
    {
    case AI::PLACE_BALLOON:
        return ((info & 0x4) != 0 ? 8 : 0) + ((info & 0x1) != 0 ? 7 : 0) - ((info & 0x8) != 0 ? 5 : 0) - ((info & 0x2) != 0 && !isRisky ? 5 : 0) - ((info & 0x80) != 0 ? 15 : 0) - (adjacentToBreakable ? 0 : 10);
    case AI::GET_POWERUP:
        return ((info & 0x8) != 0 ? 10 : 0) - ((info & 0x2) != 0 ? 5 : 0) - ((info & 0x80) != 0 ? 5 : 0) + ((info & 0x40) != 0 ? 4 : 0);
    case AI::MOVE_TO_ENEMY:
        return ((info & 0x1) != 0 ? -10 : 0) + ((info & 0x8) == 0 ? 5 : 0) - ((info & 0x2) != 0 ? 5 : 0) + (isRisky ? 7 : 0) - ((info & 0x10) == 0 ? 15 : 0);
    case AI::MOVE_TO_BREAKABLE:
        return ((info & 0x4) != 0 ? 5 : 0) - ((info & 0x8) != 0 ? 5 : 0) - ((info & 0x2) != 0 ? 10 : 0) - ((info & 0x80) != 0 ? 5 : 0) + ((info & 0x20) != 0 ? 8 : 0) + (adjacentToBreakable ? 0 : 5);
    case AI::MOVE_TO_SAFETY:
        collidingWithWIndicator = Utility::collidesAgainstComponent(pos.x, pos.y, std::type_index(typeid(cWIndicator)), engine);
        return ((info & 0x2) != 0 || collidingWithWIndicator ? 13 : 0) - (isRisky ? 4 : 0) + ((info & 0x80) != 0 ? 10 : 0) + (adjacentToWIndicator ? 3 : 0);
    case AI::KICK_BALLOON:
        return ((info & 0x2) != 0 ? 7 : 0) + (isRisky ? 7 : 0) - (living.kickUpgrade == 0 ? 7 : -2);
    case AI::PANIC:
        return 1;
    case AI::WAIT:
        collidingWithWIndicator = Utility::collidesAgainstComponent(pos.x, pos.y, std::type_index(typeid(cWIndicator)), engine);
        return (collidingWithWIndicator ? -10 : 2) + (adjacentToWIndicator ? 10 : 0);
    default:
        break;
    }
    return -100;
}

unsigned char UAI::nearbyInfo(const cPosition& pos, PathFinder& pf, Engine& engine)
{
    const unsigned char* grid = pf.getValidGrid(engine);
    int xy = (int)((pos.x + (float)(-GRID_OFFSET_X + GRID_SQUARE_SIZE / 2)) / GRID_SQUARE_SIZE) + (int)((pos.y + (float)(-GRID_OFFSET_Y + GRID_SQUARE_SIZE / 2)) / GRID_SQUARE_SIZE) * GRID_WIDTH;

    unsigned char info = 0;

    info |= ((grid[xy] & 0x2) << 6);
    info |= ((grid[xy] & 0x20) >> 4);

    // check up
    for(int c = xy - GRID_WIDTH; c >= 0; c -= GRID_WIDTH)
    {
        if(grid[c] == 0)
            continue;
        else if((grid[c] & 0x10) != 0)
            break;
        info |= (grid[c] & 0x1);
        info |= (grid[c] & 0x4);
        info |= (grid[c] & 0x8);
    }

    // check down
    for(int c = xy + GRID_WIDTH; c < GRID_TOTAL; c += GRID_WIDTH)
    {
        if(grid[c] == 0)
            continue;
        else if((grid[c] & 0x10) != 0)
            break;
        info |= (grid[c] & 0x1);
        info |= (grid[c] & 0x4);
        info |= (grid[c] & 0x8);
    }

    // check left
    for(int c = xy - 1; c % GRID_WIDTH != 0; --c)
    {
        if(grid[c] == 0)
            continue;
        else if((grid[c] & 0x10) != 0)
            break;
        info |= (grid[c] & 0x1);
        info |= (grid[c] & 0x4);
        info |= (grid[c] & 0x8);
    }

    // check right
    for(int c = xy + 1; c % GRID_WIDTH != GRID_WIDTH - 1; ++c)
    {
        if(grid[c] == 0)
            continue;
        else if((grid[c] & 0x10) != 0)
            break;
        info |= (grid[c] & 0x1);
        info |= (grid[c] & 0x4);
        info |= (grid[c] & 0x8);
    }

    info |= ((grid[GRID_TOTAL] & 0x1) << 4);
    info |= ((grid[GRID_TOTAL] & 0x2) << 4);
    info |= ((grid[GRID_TOTAL] & 0x4) << 4);

    return info;
}
