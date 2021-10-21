
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
{}

std::map<int, int> PathFinder::getValidDestinations(const cPosition& pos, Engine& engine, unsigned char obstacles)
{
    if(dirtyFlag)
        revalidateGrid(engine);

    return getValidDestinations(pos, engine, obstacles, validGrid);
}

std::map<int, int> PathFinder::getValidDestinations(const cPosition& pos, Engine& /*engine*/, unsigned char obstacles, const ValidGridT &validGrid)
{
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

        if(n % GRID_WIDTH != GRID_WIDTH - 1 && paths.find(n + 1) == paths.end() && (validGrid.at(n + 1) & obstacles) == 0)
        {
            next.push(n + 1);
            paths.insert(std::make_pair(n + 1, n));
        }
        if(n % GRID_WIDTH != 0 && paths.find(n - 1) == paths.end() && (validGrid.at(n - 1) & obstacles) == 0)
        {
            next.push(n - 1);
            paths.insert(std::make_pair(n - 1, n));
        }
        if(n + GRID_WIDTH < GRID_TOTAL && paths.find(n + GRID_WIDTH) == paths.end() && (validGrid.at(n + GRID_WIDTH) & obstacles) == 0)
        {
            next.push(n + GRID_WIDTH);
            paths.insert(std::make_pair(n + GRID_WIDTH, n));
        }
        if(n - GRID_WIDTH >= 0 && paths.find(n - GRID_WIDTH) == paths.end() && (validGrid.at(n - GRID_WIDTH) & obstacles) == 0)
        {
            next.push(n - GRID_WIDTH);
            paths.insert(std::make_pair(n - GRID_WIDTH, n));
        }
    }

    n = x + y * GRID_WIDTH;
    paths.erase(n);
    paths.insert(std::make_pair(n, n));

    return paths;
}

std::map<int, int> PathFinder::getBestPath(const cPosition& pos, const sf::Vector2f& goal, Engine& engine, unsigned char obstacles)
{
    return getBestPath(pos, (int)((goal.x + (float)(-GRID_OFFSET_X + GRID_SQUARE_SIZE / 2)) / GRID_SQUARE_SIZE) + (int)((goal.y + (float)(-GRID_OFFSET_Y + GRID_SQUARE_SIZE / 2)) / GRID_SQUARE_SIZE) * GRID_WIDTH, engine, obstacles);
}

std::map<int, int> PathFinder::getBestPath(const cPosition& pos, int goal, Engine& engine, unsigned char obstacles)
{
    if(dirtyFlag)
        revalidateGrid(engine);

    int x = (pos.x + (float)(-GRID_OFFSET_X + GRID_SQUARE_SIZE / 2)) / GRID_SQUARE_SIZE;
    int y = (pos.y + (float)(-GRID_OFFSET_Y + GRID_SQUARE_SIZE / 2)) / GRID_SQUARE_SIZE;
    int xy = x + y * GRID_WIDTH;

    int endx = (float)(goal % GRID_WIDTH * GRID_SQUARE_SIZE + GRID_OFFSET_X);
    int endy = (float)(goal / GRID_WIDTH * GRID_SQUARE_SIZE + GRID_OFFSET_Y);

    std::map<int, int> paths;
    std::map<int, int> costs;
    std::priority_queue<PElement> next;
    next.push(PElement(xy,0));
    paths.insert(std::make_pair(xy, xy));
    costs.insert(std::make_pair(xy, 0));
    int n;
    int cost;
    while(!next.empty())
    {
        n = next.top().c;
        next.pop();

        if(n == goal)
            break;

        if(n % GRID_WIDTH != 0 && (validGrid.at(n - 1) & obstacles) == 0)
        {
            cost = costs[n] + 1;
            if(costs.find(n - 1) == costs.end() || cost < costs[n - 1])
            {
                costs.insert(std::make_pair(n - 1, cost));
                next.push(PElement(n - 1, cost + PF::heuristic((n - 1) % GRID_WIDTH, (n - 1) / GRID_WIDTH, endx, endy)));
                paths.insert(std::make_pair(n - 1, n));
            }
        }
        if(n % GRID_WIDTH != GRID_WIDTH - 1 && (validGrid.at(n + 1) & obstacles) == 0)
        {
            cost = costs[n] + 1;
            if(costs.find(n + 1) == costs.end() || cost < costs[n + 1])
            {
                costs.insert(std::make_pair(n + 1, cost));
                next.push(PElement(n + 1, cost + PF::heuristic((n + 1) % GRID_WIDTH, (n + 1) / GRID_WIDTH, endx, endy)));
                paths.insert(std::make_pair(n + 1, n));
            }
        }
        if(n - GRID_WIDTH >= 0 && (validGrid.at(n - GRID_WIDTH) & obstacles) == 0)
        {
            cost = costs[n - GRID_WIDTH] + 1;
            if(costs.find(n - GRID_WIDTH) == costs.end() || cost < costs[n - GRID_WIDTH])
            {
                costs.insert(std::make_pair(n - GRID_WIDTH, cost));
                next.push(PElement(n - GRID_WIDTH, cost + PF::heuristic(n % GRID_WIDTH, n / GRID_WIDTH - 1, endx, endy)));
                paths.insert(std::make_pair(n - GRID_WIDTH, n));
            }
        }
        if(n + GRID_WIDTH < GRID_TOTAL && (validGrid.at(n + GRID_WIDTH) & obstacles) == 0)
        {
            cost = costs[n + GRID_WIDTH] + 1;
            if(costs.find(n + GRID_WIDTH) == costs.end() || cost < costs[n + GRID_WIDTH])
            {
                costs.insert(std::make_pair(n + GRID_WIDTH, cost));
                next.push(PElement(n + GRID_WIDTH, cost + PF::heuristic(n % GRID_WIDTH, n / GRID_WIDTH + 1, endx, endy)));
                paths.insert(std::make_pair(n + GRID_WIDTH, n));
            }
        }
    }

    paths.erase(xy);

    return paths;
}

void PathFinder::invalidateValidGrid()
{
    dirtyFlag = true;
}

const PathFinder::ValidGridT& PathFinder::getValidGrid(Engine& engine)
{
    if(dirtyFlag)
        revalidateGrid(engine);

    return validGrid;
}

bool PathFinder::isDirty() const
{
    return dirtyFlag;
}

void PathFinder::revalidateGrid(Engine& engine)
{
    dirtyFlag = false;

    for(int i = 0; i <= GRID_TOTAL; ++i)
    {
        validGrid.at(i) = 0;
    }

    int x,y,xy;
    std::list<std::pair<int, cBalloon*> > balloons;

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
            validGrid.at(xy) |= 0x1;
            validGrid.at(GRID_TOTAL) |= 0x1;
        }
        else if(iter->second->hasComponent(std::type_index(typeid(cBalloon))))
        {
            validGrid.at(xy) |= 0x22;
            balloons.emplace_back(xy, static_cast<cBalloon*>(iter->second->getComponent(std::type_index(typeid(cBalloon)))));
        }
        else if(iter->second->hasComponent(std::type_index(typeid(cBreakable))))
        {
            validGrid.at(xy) |= 0x4;
            validGrid.at(GRID_TOTAL) |= 0x2;
        }
        else if(iter->second->hasComponent(std::type_index(typeid(cPickup))))
        {
            validGrid.at(xy) |= 0x8;
            validGrid.at(GRID_TOTAL) |= 0x4;
        }
        else if(iter->second->hasComponent(std::type_index(typeid(cWall))))
            validGrid.at(xy) |= 0x10;
    }

    int distance;
    for(auto & balloon : balloons)
    {
        distance = balloon.second->range;
        for(int i = balloon.first - 1; (i + 1) % GRID_WIDTH != 0; --i)
        {
            if(!balloon.second->ghosting && (validGrid.at(i) & 0x10) != 0)
                break;
            validGrid.at(i) |= 0x20;
            if(--distance == 0 || (!balloon.second->piercing && (validGrid.at(i) & 0x4) != 0))
                break;
        }

        distance = balloon.second->range;
        for(int i = balloon.first + 1; (i - 1) % GRID_WIDTH != GRID_WIDTH - 1; ++i)
        {
            if(!balloon.second->ghosting && (validGrid.at(i) & 0x10) != 0)
                break;
            validGrid.at(i) |= 0x20;
            if(--distance == 0 || (!balloon.second->piercing && (validGrid.at(i) & 0x4) != 0))
                break;
        }

        distance = balloon.second->range;
        for(int i = balloon.first - GRID_WIDTH; i >= 0; i -= GRID_WIDTH)
        {
            if(!balloon.second->ghosting && (validGrid.at(i) & 0x10) != 0)
                break;
            validGrid.at(i) |= 0x20;
            if(--distance == 0 || (!balloon.second->piercing && (validGrid.at(i) & 0x4) != 0))
                break;
        }

        distance = balloon.second->range;
        for(int i = balloon.first + GRID_WIDTH; i < GRID_TOTAL; i += GRID_WIDTH)
        {
            if(!balloon.second->ghosting && (validGrid.at(i) & 0x10) != 0)
                break;
            validGrid.at(i) |= 0x20;
            if(--distance == 0 || (!balloon.second->piercing && (validGrid.at(i) & 0x4) != 0))
                break;
        }
    }
}
