
#include "uai.hpp"

#include <queue>
#include <utility>

#ifndef NDEBUG
    #include <iostream>
#endif

#include "pathFinder.hpp"
#include "../../ec/cPosition.hpp"
#include "../cLiving.hpp"
#include "../../ec/engine.hpp"
#include "../gridInfo.hpp"
#include "../utility.hpp"
#include "../cWIndicator.hpp"

PAMapping::PAMapping() :
priority(0.0F),
paths(),
destination(-1)
{}

PAMapping::PAMapping(float priority, std::map<int, int> paths, int destination) :
priority(priority),
paths(std::move(paths)),
destination(destination)
{}

ActionElement::ActionElement() :
action(AI::PANIC),
pm()
{}

ActionElement::ActionElement(AI::Action action, PAMapping pm) :
action(action),
pm(std::move(pm))
{}

bool ActionElement::operator< (const ActionElement& other) const
{
    return pm.priority < other.pm.priority;
}

UAI::UAI() :
wildness(DEFAULT_AI_WILDNESS),
isRisky(false)
{}

PAMapping UAI::determineAction(const cPosition& pos, const cLiving& living, PathFinder& pf, Engine& engine, std::mt19937& gen)
{
    std::priority_queue<ActionElement> levels;
    for(int i = 0; i < AI::SIZE; ++i)
    {
        levels.push(ActionElement(static_cast<AI::Action>(i),utility(static_cast<AI::Action>(i), pos, living, pf, engine)));
    }

    ActionElement picked[wildness];
    for(unsigned char i = 0; i < wildness; ++i)
    {
        picked[i] = levels.top();

        picked[i].pm.action = picked[i].action;

        levels.pop();
    }

    if(wildness > 1)
    {
        std::normal_distribution<> dist(0,wildness / 2);
        int i = (int)(dist(gen) + 0.5);
        if(i >= wildness)
            return picked[wildness - 1].pm;
        else if(i >= 0)
            return picked[i].pm;
    }

    return picked[0].pm;
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

PAMapping UAI::utility(AI::Action action, const cPosition& pos, const cLiving& living, PathFinder& pf, Engine& engine)
{
    PAMapping pm;

    const PathFinder::ValidGridT &grid = pf.getValidGrid(engine);
    int xy = (int)((pos.x + (float)(-GRID_OFFSET_X + GRID_SQUARE_SIZE / 2)) / GRID_SQUARE_SIZE) + (int)((pos.y + (float)(-GRID_OFFSET_Y + GRID_SQUARE_SIZE / 2)) / GRID_SQUARE_SIZE) * GRID_WIDTH;
    //bool collidingWithWIndicator = false;

    PathFinder::ValidGridT postGrid;
    unsigned short info = nearbyInfo(pos, grid, engine);
    //unsigned short postInfo;
    for(int i = 0; i <= GRID_TOTAL; ++i)
        postGrid.at(i) = grid.at(i);

    int temp, range;
    float ret = 0.0F, mult = 0.0F;

    switch(action)
    {
    case AI::PLACE_BALLOON:
    {
        std::map<int, int> validDestinations = pf.getValidDestinations(pos, engine, 0x36, grid);
        if(validDestinations.size() < 2)
            break;

        postGrid.at(xy) |= 0x22;
        if(living.sBalloonsInPlay < living.sBalloonUpgrade)
            range = SUPER_RANGE;
        else
            range = living.rangeUp + DEFAULT_RANGE;

        // predict explosion left
        temp = range;
        for(int i = xy - 1; (i + 1) % GRID_WIDTH != 0 && temp > 0; --i)
        {
            if(living.ghostUpgrade == 0 && (postGrid.at(i) & 0x10) != 0)
                break;
            postGrid.at(i) |= 0x20;
            if(living.pierceUpgrade == 0 && (postGrid.at(i) & 0x4) != 0)
                break;
            --temp;
        }

        // predict explosion right
        temp = range;
        for(int i = xy + 1; (i - 1) % GRID_WIDTH != GRID_WIDTH - 1 && temp > 0; ++i)
        {
            if(living.ghostUpgrade == 0 && (postGrid.at(i) & 0x10) != 0)
                break;
            postGrid.at(i) |= 0x20;
            if(living.pierceUpgrade == 0 && (postGrid.at(i) & 0x4) != 0)
                break;
            --temp;
        }

        // predict explosion up
        temp = range;
        for(int i = xy - GRID_WIDTH; i >= 0 && temp > 0; i -= GRID_WIDTH)
        {
            if(living.ghostUpgrade == 0 && (postGrid.at(i) & 0x10) != 0)
                break;
            postGrid.at(i) |= 0x20;
            if(living.pierceUpgrade == 0 && (postGrid.at(i) & 0x4) != 0)
                break;
            --temp;
        }

        // predict explosion down
        temp = range;
        for(int i = xy + GRID_WIDTH; i < GRID_TOTAL && temp > 0; i += GRID_WIDTH)
        {
            if(living.ghostUpgrade == 0 && (postGrid.at(i) & 0x10) != 0)
                break;
            postGrid.at(i) |= 0x20;
            if(living.pierceUpgrade == 0 && (postGrid.at(i) & 0x4) != 0)
                break;
            --temp;
        }

        validDestinations = pf.getValidDestinations(pos, engine, 0x14, postGrid);
        for(auto & validDestination : validDestinations)
        {
            if((postGrid.at(validDestination.first) & 0x20) == 0)
            {
                mult = 1.0F;
                break;
            }
        }

        ret = 0.4F;
        if((info & 0x1) != 0)
            ret += 0.3F;
        else if((info & 0x2) != 0)
            ret -= 0.2F;
        if((info & 0x100) != 0)
            ret += 0.3F;
        if((info & 0x8) != 0)
            ret -= 0.3F;
        break;
    }
    case AI::GET_POWERUP:
        if((info & 0x40) != 0)
        {
            pm.paths = pf.getValidDestinations(pos, engine, 0x34, grid);
            int pdist = -1, tdist = 0, tx, ty;
            for(auto iter = pm.paths.begin(); iter != pm.paths.end(); ++iter)
            {
                if((grid.at(iter->first) & 0x8) != 0)
                {
                    mult = 1.0F;
                    tx = iter->first % GRID_WIDTH;
                    ty = iter->first / GRID_WIDTH;
                    if(pdist == -1 || (tdist = PF::heuristic(tx, ty, xy % GRID_WIDTH, xy / GRID_WIDTH)) < pdist)
                    {
                        pm.destination = iter->first;
                        pdist = tdist;
                    }
                }
            }
            if(mult == 0.0F)
                break;

            ret = 1.0F / (float) pdist;
        }
        break;
    case AI::MOVE_TO_ENEMY:
        if((info & 0x10) != 0)
        {
            pm.paths = pf.getValidDestinations(pos, engine, 0x34, grid);
            int tx, ty, pdist = -1, tdist = 0;
            for(auto iter = pm.paths.begin(); iter != pm.paths.end(); ++iter)
            {
                if(xy != iter->first && (grid.at(iter->first) & 0x1) != 0)
                {
                    mult = 1.0F;
                    tx = iter->first % GRID_WIDTH;
                    ty = iter->first / GRID_WIDTH;
                    if(pdist == -1 || (tdist = PF::heuristic(tx, ty, xy % GRID_WIDTH, xy / GRID_WIDTH)) < pdist)
                    {
                        pm.destination = iter->first;
                        pdist = tdist;
                    }
                }
            }
            if(mult == 0.0F)
                break;
            ret = (float)pdist / 28.0F;
            if((info & 0x20) != 0)
                ret -= 0.2F;
        }
        break;
    case AI::MOVE_TO_BREAKABLE:
        if((info & 0x20) != 0)
        {
            pm.paths = pf.getValidDestinations(pos,engine, 0x34, grid);
            int tx, ty, pdist = -1, tdist = 0;
            for(auto iter = pm.paths.begin(); iter != pm.paths.end(); ++iter)
            {
                if(iter->first % GRID_WIDTH != 0 && (grid.at(iter->first - 1) & 0x4) != 0)
                {
                    mult = 1.0F;
                    tx = (iter->first - 1) % GRID_WIDTH;
                    ty = (iter->first - 1) / GRID_WIDTH;
                    if(pdist == -1 || (tdist = PF::heuristic(tx, ty, xy % GRID_WIDTH, xy / GRID_WIDTH)) < pdist)
                    {
                        pm.destination = iter->first;
                        pdist = tdist;
                    }
                }
                if(iter->first % GRID_WIDTH != GRID_WIDTH - 1 && (grid.at(iter->first + 1) & 0x4) != 0)
                {
                    mult = 1.0F;
                    tx = (iter->first + 1) % GRID_WIDTH;
                    ty = (iter->first + 1) / GRID_WIDTH;
                    if(pdist == -1 || (tdist = PF::heuristic(tx, ty, xy % GRID_WIDTH, xy / GRID_WIDTH)) < pdist)
                    {
                        pm.destination = iter->first;
                        pdist = tdist;
                    }
                }
                if(iter->first - GRID_WIDTH >= 0 && (grid.at(iter->first - GRID_WIDTH) & 0x4) != 0)
                {
                    mult = 1.0F;
                    tx = iter->first % GRID_WIDTH;
                    ty = iter->first / GRID_WIDTH - 1;
                    if(pdist == -1 || (tdist = PF::heuristic(tx, ty, xy % GRID_WIDTH, xy / GRID_WIDTH)) < pdist)
                    {
                        pm.destination = iter->first;
                        pdist = tdist;
                    }
                }
                if(iter->first + GRID_WIDTH < GRID_TOTAL && (grid.at(iter->first + GRID_WIDTH) & 0x4) != 0)
                {
                    mult = 1.0F;
                    tx = iter->first % GRID_WIDTH;
                    ty = iter->first / GRID_WIDTH + 1;
                    if(pdist == -1 || (tdist = PF::heuristic(tx, ty, xy % GRID_WIDTH, xy / GRID_WIDTH)) < pdist)
                    {
                        pm.destination = iter->first;
                        pdist = tdist;
                    }
                }
            }
            if(mult == 0.0F)
            {
                break;
            }
            ret = (float)pdist / 20.0F;
            if(ret == 0.0F)
                ret = 0.2F;
        }
        break;
    case AI::MOVE_TO_SAFETY:
        if((info & 0x2) != 0)
        {
            mult = 1.0F;
            ret = 0.7F;
            if((info & 0x80) != 0)
                ret += 0.3F;

            postGrid.at(xy) &= 0xFD;
            pm.paths = pf.getValidDestinations(pos, engine, 0x16, postGrid);
            int tx, ty, pdist = -1, tdist = 0;
            for(auto iter = pm.paths.begin(); iter != pm.paths.end(); ++iter)
            {
                if((grid.at(iter->first) & 0x20) == 0)
                {
                    tx = iter->first % GRID_WIDTH;
                    ty = iter->first / GRID_WIDTH;
                    if(pdist == -1 || (tdist = PF::heuristic(tx, ty, xy % GRID_WIDTH, xy / GRID_WIDTH)) < pdist)
                    {
                        pm.destination = iter->first;
                        pdist = tdist;
                    }
                }
            }
        }
        break;
    case AI::KICK_BALLOON:
        break;
    case AI::PANIC:
        if((info & 0x400) != 0 && (info & 0x2) != 0)
        {
            mult = 1.0F;
            ret = 0.9F;
        }
        break;
    case AI::WAIT:
        if((info & 0x400) != 0 && (info & 0x2) == 0)
        {
            mult = 1.0F;
            ret = 0.5F;
        }
        break;
    default:
        break;
    }

    pm.priority = ret * mult;
    if(pm.priority > 1.0F)
        pm.priority = 1.0F;
    else if(pm.priority < 0.0F)
        pm.priority = 0.0F;

    return pm;
}

unsigned short UAI::nearbyInfo(const cPosition& pos, const PathFinder::ValidGridT &grid, Engine& /*engine*/)
{
    int xy = (int)((pos.x + (float)(-GRID_OFFSET_X + GRID_SQUARE_SIZE / 2)) / GRID_SQUARE_SIZE) + (int)((pos.y + (float)(-GRID_OFFSET_Y + GRID_SQUARE_SIZE / 2)) / GRID_SQUARE_SIZE) * GRID_WIDTH;

    unsigned short info = 0;

    info |= ((grid.at(xy) & 0x2) << 6);
    info |= ((grid.at(xy) & 0x20) >> 4);

    unsigned char surrounding = 0;
    // check directly adjacent
    if(xy % GRID_WIDTH != 0)
    {
        info |= ((grid.at(xy - 1) & 0x4) << 6);
        info |= ((grid.at(xy - 1) & 0x20) << 4);
        if((grid.at(xy - 1) & 0x36) != 0)
            surrounding += 1;
    }
    if(xy % GRID_WIDTH != GRID_WIDTH - 1)
    {
        info |= ((grid.at(xy + 1) & 0x4) << 6);
        info |= ((grid.at(xy + 1) & 0x20) << 4);
        if((grid.at(xy + 1) & 0x36) != 0)
            surrounding += 1;
    }
    if(xy - GRID_WIDTH >= 0)
    {
        info |= ((grid.at(xy - GRID_WIDTH) & 0x4) << 6);
        info |= ((grid.at(xy - GRID_WIDTH) & 0x20) << 4);
        if((grid.at(xy - GRID_WIDTH) & 0x36) != 0)
            surrounding += 1;
    }
    if(xy + GRID_WIDTH < GRID_TOTAL)
    {
        info |= ((grid.at(xy + GRID_WIDTH) & 0x4) << 6);
        info |= ((grid.at(xy + GRID_WIDTH) & 0x20) << 4);
        if((grid.at(xy + GRID_WIDTH) & 0x36) != 0)
            surrounding += 1;
    }

    if(surrounding == 4)
        info |= 0x400;

    // check up
    for(int c = xy - GRID_WIDTH; c >= 0; c -= GRID_WIDTH)
    {
        if(grid.at(c) == 0)
            continue;
        else if((grid.at(c) & 0x10) != 0)
            break;
        info |= (grid.at(c) & 0x1);
        info |= (grid.at(c) & 0x4);
        info |= (grid.at(c) & 0x8);
    }

    // check down
    for(int c = xy + GRID_WIDTH; c < GRID_TOTAL; c += GRID_WIDTH)
    {
        if(grid.at(c) == 0)
            continue;
        else if((grid.at(c) & 0x10) != 0)
            break;
        info |= (grid.at(c) & 0x1);
        info |= (grid.at(c) & 0x4);
        info |= (grid.at(c) & 0x8);
    }

    // check left
    if(xy % GRID_WIDTH > 0) {
        for(int c = xy - 1; c % GRID_WIDTH != 0; --c)
        {
            if(grid.at(c) == 0)
                continue;
            else if((grid.at(c) & 0x10) != 0)
                break;
            info |= (grid.at(c) & 0x1);
            info |= (grid.at(c) & 0x4);
            info |= (grid.at(c) & 0x8);
        }
    }

    // check right
    if(xy % GRID_WIDTH < GRID_WIDTH - 1) {
        for(int c = xy + 1; c % GRID_WIDTH != GRID_WIDTH - 1; ++c)
        {
            if(grid.at(c) == 0)
                continue;
            else if((grid.at(c) & 0x10) != 0)
                break;
            info |= (grid.at(c) & 0x1);
            info |= (grid.at(c) & 0x4);
            info |= (grid.at(c) & 0x8);
        }
    }

    info |= ((grid.at(GRID_TOTAL) & 0x1) << 4);
    info |= ((grid.at(GRID_TOTAL) & 0x2) << 4);
    info |= ((grid.at(GRID_TOTAL) & 0x4) << 4);

    return info;
}
