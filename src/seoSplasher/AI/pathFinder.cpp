
#include "pathFinder.hpp"

#include <typeindex>
#include <set>
#include <cmath>

#include "../../ec/cPosition.hpp"
#include "../../ec/engine.hpp"
#include "../cLiving.hpp"
#include "../cBalloon.hpp"
#include "../cBreakable.hpp"
#include "../cPickup.hpp"
#include "../cWall.hpp"
#include "../cWIndicator.hpp"

PElement::PElement() :
c(0),
priority(0)
{}

PElement::PElement(int c, int priority) :
c(c),
priority(priority)
{}

bool PElement::operator< (const PElement& other) const
{
    return priority < other.priority;
}

int PF::heuristic(int xOne, int yOne, int xTwo, int yTwo)
{
    return std::abs(xOne - xTwo) + std::abs(yOne - yTwo);
}

PathFinder::PathFinder() :
dirtyFlag(true)
{
}

std::map<int, int> PathFinder::getValidDestinations(const cPosition& pos, Engine& engine, std::list<std::type_index> obstacles)
{
    if(dirtyFlag)
        revalidateGrid(engine);

    unsigned char mask = 0;
    if(obstacles.empty())
    {
        mask = 0xFF;
    }
    else
    {
        for(auto iter = obstacles.begin(); iter != obstacles.end(); ++iter)
        {
            if(*iter == std::type_index(typeid(cLiving)))
                mask |= 0x1;
            if(*iter == std::type_index(typeid(cBalloon)))
                mask |= 0x2;
            if(*iter == std::type_index(typeid(cBreakable)))
                mask |= 0x4;
            if(*iter == std::type_index(typeid(cWall)))
                mask |= 0x10;
            if(*iter == std::type_index(typeid(cWIndicator)))
                mask |= 0x20;
        }
    }

    int x = (pos.x - (float)GRID_OFFSET_X + (float)(GRID_SQUARE_SIZE / 2)) / GRID_SQUARE_SIZE;
    int y = (pos.y - (float)GRID_OFFSET_Y + (float)(GRID_SQUARE_SIZE / 2)) / GRID_SQUARE_SIZE;

    std::map<int, int> paths;
    std::queue<int> next;
    int n = x + y * GRID_WIDTH;
    next.push(n);
    paths.insert(std::make_pair(n, n));
    while(!next.empty())
    {
        n = next.front();
        next.pop();

        if(n % GRID_WIDTH != GRID_WIDTH - 1 && paths.find(n + 1) == paths.end() && (validGrid[n + 1] & mask) == 0)
        {
            next.push(n + 1);
            paths.insert(std::make_pair(n + 1, n));
        }
        if(n % GRID_WIDTH != 0 && paths.find(n - 1) == paths.end() && (validGrid[n - 1] & mask) == 0)
        {
            next.push(n - 1);
            paths.insert(std::make_pair(n - 1, n));
        }
        if(n + GRID_WIDTH < GRID_TOTAL && paths.find(n + GRID_WIDTH) == paths.end() && (validGrid[n + GRID_WIDTH] & mask) == 0)
        {
            next.push(n + GRID_WIDTH);
            paths.insert(std::make_pair(n + GRID_WIDTH, n));
        }
        if(n - GRID_WIDTH >= 0 && paths.find(n - GRID_WIDTH) == paths.end() && (validGrid[n - GRID_WIDTH] & mask) == 0)
        {
            next.push(n - GRID_WIDTH);
            paths.insert(std::make_pair(n - GRID_WIDTH, n));
        }
    }

    paths.erase(x + y * GRID_WIDTH);

    return paths;
}

bool PathFinder::isValidDestination(const cPosition& pos, const sf::Vector2f& destination, Engine& engine, std::list<std::type_index> obstacles)
{
    if(dirtyFlag)
        revalidateGrid(engine);

    unsigned char mask = 0;
    if(obstacles.empty())
    {
        mask = 0xFF;
    }
    else
    {
        for(auto iter = obstacles.begin(); iter != obstacles.end(); ++iter)
        {
            if(*iter == std::type_index(typeid(cLiving)))
                mask |= 0x1;
            if(*iter == std::type_index(typeid(cBalloon)))
                mask |= 0x2;
            if(*iter == std::type_index(typeid(cBreakable)))
                mask |= 0x4;
            if(*iter == std::type_index(typeid(cWall)))
                mask |= 0x8;
        }
    }

    int x = (pos.x - (float)GRID_OFFSET_X + (float)(GRID_SQUARE_SIZE / 2)) / GRID_SQUARE_SIZE;
    int y = (pos.y - (float)GRID_OFFSET_Y + (float)(GRID_SQUARE_SIZE / 2)) / GRID_SQUARE_SIZE;
    int start = x + y * GRID_WIDTH;
    int destX = (destination.x - (float)GRID_OFFSET_X + (float)(GRID_SQUARE_SIZE / 2)) / GRID_SQUARE_SIZE;
    int destY = (destination.y - (float)GRID_OFFSET_Y + (float)(GRID_SQUARE_SIZE / 2)) / GRID_SQUARE_SIZE;
    int end = destX + destY * GRID_WIDTH;

    std::map<int, int> from;
    std::priority_queue<PElement> next;
    next.push(PElement(start, 0));
    int n;
    while(!next.empty())
    {
        n = next.top().c;
        next.pop();

        if(n == end)
            return true;

        if(n + 1 < GRID_TOTAL && n % GRID_WIDTH != GRID_WIDTH - 1 && from.find(n + 1) == from.end() && (validGrid[n + 1] & mask) == 0 && n + 1 != start)
        {
            next.push(PElement(n + 1, PF::heuristic(destX, destY, (n + 1) % GRID_WIDTH, (n + 1) / GRID_WIDTH)));
            from[n + 1] = n;
        }
        if(n - 1 >= 0 && n % GRID_WIDTH != 0 && from.find(n - 1) == from.end() && (validGrid[n - 1] & mask) == 0 && n - 1 != start)
        {
            next.push(PElement(n - 1, PF::heuristic(destX, destY, (n - 1) % GRID_WIDTH, (n - 1) / GRID_WIDTH)));
            from[n - 1] = n;
        }
        if(n + GRID_WIDTH < GRID_TOTAL && from.find(n + GRID_WIDTH) == from.end() && (validGrid[n + GRID_WIDTH] & mask) == 0 && n + GRID_WIDTH != start)
        {
            next.push(PElement(n + GRID_WIDTH, PF::heuristic(destX, destY, n % GRID_WIDTH, n / GRID_WIDTH + 1)));
            from[n + GRID_WIDTH] = n;
        }
        if(n - GRID_WIDTH >= 0 && from.find(n - GRID_WIDTH) == from.end() && (validGrid[n - GRID_WIDTH] & mask) == 0 && n - GRID_WIDTH != start)
        {
            next.push(PElement(n - GRID_WIDTH, PF::heuristic(destX, destY, n % GRID_WIDTH, n / GRID_WIDTH - 1)));
            from[n - GRID_WIDTH] = n;
        }
    }

    return false;
}

void PathFinder::invalidateValidGrid()
{
    dirtyFlag = true;
}

const unsigned char* PathFinder::getValidGrid(Engine& engine)
{
    if(dirtyFlag)
        revalidateGrid(engine);

    return validGrid;
}

bool PathFinder::isDirty()
{
    return dirtyFlag;
}

void PathFinder::revalidateGrid(Engine& engine)
{
    dirtyFlag = false;

    for(int i = 0; i <= GRID_TOTAL; ++i)
    {
        validGrid[i] = 0;
    }

    int x,y,xy;
    for(auto iter = engine.getEntityIterBegin(); iter != engine.getEntityIterEnd(); ++iter)
    {
        if(iter->second->removed || !iter->second->hasComponent(std::type_index(typeid(cPosition))))
            continue;

        cPosition* pos = static_cast<cPosition*>(iter->second->getComponent(std::type_index(typeid(cPosition))));
        if(pos->x < (float)GRID_OFFSET_X || pos->y < (float)GRID_OFFSET_Y || pos->x >= (float)(GRID_OFFSET_X + GRID_WIDTH * GRID_SQUARE_SIZE) || pos->y >= (float)(GRID_OFFSET_Y + GRID_HEIGHT * GRID_SQUARE_SIZE))
        {
            continue;
        }
        x = (pos->x - (float)GRID_OFFSET_X + (float)(GRID_SQUARE_SIZE / 2))/GRID_SQUARE_SIZE;
        y = (pos->y - (float)GRID_OFFSET_Y + (float)(GRID_SQUARE_SIZE / 2))/GRID_SQUARE_SIZE;
        xy = x + y * GRID_WIDTH;
        if(iter->second->hasComponent(std::type_index(typeid(cLiving))))
        {
            validGrid[xy] |= 0x1;
            validGrid[GRID_TOTAL] |= 0x1;
        }
        else if(iter->second->hasComponent(std::type_index(typeid(cBalloon))))
            validGrid[xy] |= 0x2;
        else if(iter->second->hasComponent(std::type_index(typeid(cBreakable))))
        {
            validGrid[xy] |= 0x4;
            validGrid[GRID_TOTAL] |= 0x2;
        }
        else if(iter->second->hasComponent(std::type_index(typeid(cPickup))))
        {
            validGrid[xy] |= 0x8;
            validGrid[GRID_TOTAL] |= 0x4;
        }
        else if(iter->second->hasComponent(std::type_index(typeid(cWall))))
            validGrid[xy] |= 0x10;
        else if(iter->second->hasComponent(std::type_index(typeid(cWIndicator))))
            validGrid[xy] |= 0x20;
    }
}
