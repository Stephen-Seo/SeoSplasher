
#include "pathFinder.hpp"

#include <typeindex>
#include <set>
#include <map>
#include <cmath>

#include "../../ec/cPosition.hpp"
#include "../../ec/engine.hpp"

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

int PathFinder::gridSize = GRID_WIDTH * GRID_HEIGHT;

PathFinder::PathFinder() :
dirtyFlag(true)
{
}

std::list<sf::Vector2i> PathFinder::getValidDestinations(const cPosition& pos, Engine& engine)
{
    if(dirtyFlag)
        revalidateGrid(engine);

    std::list<sf::Vector2i> validDestinations;

    int x = (pos.x - (float)GRID_OFFSET_X + (float)(GRID_SQUARE_SIZE / 2)) / GRID_SQUARE_SIZE;
    int y = (pos.y - (float)GRID_OFFSET_Y + (float)(GRID_SQUARE_SIZE / 2)) / GRID_SQUARE_SIZE;

    std::set<int> visited;
    std::queue<int> next;
    next.push(x + y * GRID_WIDTH);
    int n;
    while(!next.empty())
    {
        n = next.front();
        next.pop();
        visited.insert(n);

        if(n != x + y * GRID_WIDTH)
            validDestinations.push_back(sf::Vector2i(n % GRID_WIDTH, n / GRID_WIDTH));

        if(n + 1 < PathFinder::gridSize && n % GRID_WIDTH != GRID_WIDTH - 1 && visited.find(n + 1) != visited.end() && validGrid[n + 1])
            next.push(n + 1);
        if(n - 1 >= 0 && n % GRID_WIDTH != 0 && visited.find(n - 1) != visited.end() && validGrid[n - 1])
            next.push(n - 1);
        if(n + GRID_WIDTH < PathFinder::gridSize && visited.find(n + GRID_WIDTH) != visited.end() && validGrid[n + GRID_WIDTH])
            next.push(n + GRID_WIDTH);
        if(n - GRID_WIDTH >= 0 && visited.find(n - GRID_WIDTH) != visited.end() && validGrid[n - GRID_WIDTH])
            next.push(n - GRID_WIDTH);
    }

    return validDestinations;
}

std::list<sf::Vector2i> PathFinder::getPathToDestination(const cPosition& pos, const sf::Vector2f& destination, Engine& engine)
{
    if(dirtyFlag)
        revalidateGrid(engine);

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
            break;

        if(n + 1 < PathFinder::gridSize && n % GRID_WIDTH != GRID_WIDTH - 1 && from.find(n + 1) == from.end() && validGrid[n + 1] && n + 1 != start)
        {
            next.push(PElement(n + 1, PF::heuristic(destX, destY, (n + 1) % GRID_WIDTH, (n + 1) / GRID_WIDTH)));
            from[n + 1] = n;
        }
        if(n - 1 >= 0 && n % GRID_WIDTH != 0 && from.find(n - 1) == from.end() && validGrid[n - 1] && n - 1 != start)
        {
            next.push(PElement(n - 1, PF::heuristic(destX, destY, (n - 1) % GRID_WIDTH, (n - 1) / GRID_WIDTH)));
            from[n - 1] = n;
        }
        if(n + GRID_WIDTH < PathFinder::gridSize && from.find(n + GRID_WIDTH) == from.end() && validGrid[n + GRID_WIDTH] && n + GRID_WIDTH != start)
        {
            next.push(PElement(n + GRID_WIDTH, PF::heuristic(destX, destY, n % GRID_WIDTH, n / GRID_WIDTH + 1)));
            from[n + GRID_WIDTH] = n;
        }
        if(n - GRID_WIDTH >= 0 && from.find(n - GRID_WIDTH) == from.end() && validGrid[n - GRID_WIDTH] && n - GRID_WIDTH != start)
        {
            next.push(PElement(n - GRID_WIDTH, PF::heuristic(destX, destY, n % GRID_WIDTH, n / GRID_WIDTH - 1)));
            from[n - GRID_WIDTH] = n;
        }
    }

    std::list<sf::Vector2i> pathToDestination;
    if(from.find(end) != from.end())
    {
        pathToDestination.push_front(sf::Vector2i(end % GRID_WIDTH, end / GRID_WIDTH));
        int current = end;
        while(current != start)
        {
            pathToDestination.push_front(sf::Vector2i(from[current] % GRID_WIDTH, from[current] / GRID_WIDTH));
            current = from[current];
        }
    }
    return pathToDestination;
}

bool PathFinder::isValidDestination(const cPosition& pos, const sf::Vector2f& destination, Engine& engine)
{
    if(dirtyFlag)
        revalidateGrid(engine);

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

        if(n + 1 < PathFinder::gridSize && n % GRID_WIDTH != GRID_WIDTH - 1 && from.find(n + 1) == from.end() && validGrid[n + 1] && n + 1 != start)
        {
            next.push(PElement(n + 1, PF::heuristic(destX, destY, (n + 1) % GRID_WIDTH, (n + 1) / GRID_WIDTH)));
            from[n + 1] = n;
        }
        if(n - 1 >= 0 && n % GRID_WIDTH != 0 && from.find(n - 1) == from.end() && validGrid[n - 1] && n - 1 != start)
        {
            next.push(PElement(n - 1, PF::heuristic(destX, destY, (n - 1) % GRID_WIDTH, (n - 1) / GRID_WIDTH)));
            from[n - 1] = n;
        }
        if(n + GRID_WIDTH < PathFinder::gridSize && from.find(n + GRID_WIDTH) == from.end() && validGrid[n + GRID_WIDTH] && n + GRID_WIDTH != start)
        {
            next.push(PElement(n + GRID_WIDTH, PF::heuristic(destX, destY, n % GRID_WIDTH, n / GRID_WIDTH + 1)));
            from[n + GRID_WIDTH] = n;
        }
        if(n - GRID_WIDTH >= 0 && from.find(n - GRID_WIDTH) == from.end() && validGrid[n - GRID_WIDTH] && n - GRID_WIDTH != start)
        {
            next.push(PElement(n - GRID_WIDTH, PF::heuristic(destX, destY, n % GRID_WIDTH, n / GRID_WIDTH - 1)));
            from[n - GRID_WIDTH] = n;
        }
    }

    return false;
}

void PathFinder::invalidateValidGrid(std::list<std::type_index> filter)
{
    this->filter = filter;
    dirtyFlag = true;
}

void PathFinder::revalidateGrid(Engine& engine)
{
    dirtyFlag = false;

    for(int i = 0; i < GRID_WIDTH * GRID_HEIGHT; ++i)
    {
        validGrid[i] = true;
    }

    int x,y;
    for(auto iter = engine.getEntityIterBegin(); iter != engine.getEntityIterEnd(); ++iter)
    {
        if(iter->second->removed)
            continue;

        if(!filter.empty())
        {
            bool skip = true;
            for(auto fiter = filter.begin(); fiter != filter.end(); ++fiter)
            {
                if(iter->second->hasComponent(*fiter))
                {
                    skip = false;
                    break;
                }
            }
            if(skip)
                continue;
        }

        cPosition* pos = static_cast<cPosition*>(iter->second->getComponent(std::type_index(typeid(cPosition))));
        if(pos->x < (float)GRID_OFFSET_X || pos->y < (float)GRID_OFFSET_Y || pos->x >= (float)(GRID_OFFSET_X + GRID_WIDTH * GRID_SQUARE_SIZE) || pos->y >= (float)(GRID_OFFSET_Y + GRID_HEIGHT * GRID_SQUARE_SIZE))
        {
            continue;
        }
        x = (pos->x - (float)GRID_OFFSET_X + (float)(GRID_SQUARE_SIZE / 2))/GRID_SQUARE_SIZE;
        y = (pos->y - (float)GRID_OFFSET_Y + (float)(GRID_SQUARE_SIZE / 2))/GRID_SQUARE_SIZE;
        validGrid[x + y * GRID_WIDTH] = false;
    }
}
