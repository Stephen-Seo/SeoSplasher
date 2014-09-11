
#ifndef PATH_FINDER_HPP
#define PATH_FINDER_HPP

#include <list>
#include <queue>
#include <map>
#include <typeindex>

#include <SFML/System.hpp>

#include "../gridInfo.hpp"

struct cPosition;
class Engine;

struct PElement
{
    PElement();
    PElement(int c, int priority);

    int c;
    int priority;

    bool operator< (const PElement& other) const;
};

namespace PF
{
    int heuristic(int xOne, int yOne, int xTwo, int yTwo);
}

class PathFinder
{
public:
    PathFinder();

    std::map<int, int> getValidDestinations(const cPosition& pos, Engine& engine, unsigned char obstacles);
    std::map<int, int> getValidDestinations(const cPosition& pos, Engine& engine, unsigned char obstacles, const unsigned char* validGrid);
    std::map<int, int> getBestPath(const cPosition& pos, const sf::Vector2f& goal, Engine& engine, unsigned char obstacles);
    std::map<int, int> getBestPath(const cPosition& pos, int goal, Engine& engine, unsigned char obstacles);
    void invalidateValidGrid();
    const unsigned char* getValidGrid(Engine& engine);
    bool isDirty();
private:
    /**
     * array of bitfields.
     * 0000 0000 - nothing
     * 0000 0001 - a player
     * 0000 0010 - balloon
     * 0000 0100 - breakable
     * 0000 1000 - powerup
     * 0001 0000 - wall
     * 0010 0000 - wIndicator
     * At validGrid[GRID_TOTAL]:
     * 0000 0000 - nothing
     * 0000 0001 - player exists
     * 0000 0010 - breakable exists
     * 0000 0100 - powerup exists
     */
    unsigned char validGrid[GRID_TOTAL + 1];
    bool dirtyFlag;

    void revalidateGrid(Engine& engine);
};

#endif
