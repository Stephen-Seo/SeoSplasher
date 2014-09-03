
#ifndef PATH_FINDER_HPP
#define PATH_FINDER_HPP

#include <list>
#include <queue>
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

    std::list<sf::Vector2i> getValidDestinations(const cPosition& pos, Engine& engine);
    std::list<sf::Vector2i> getPathToDestination(const cPosition& pos, const sf::Vector2f& destination, Engine& engine);
    bool isValidDestination(const cPosition& pos, const sf::Vector2f& destination, Engine& engine);
    void invalidateValidGrid(std::list<std::type_index> filter = std::list<std::type_index>());
private:
    bool validGrid[GRID_WIDTH * GRID_HEIGHT];
    bool dirtyFlag;
    static int gridSize;
    std::list<std::type_index> filter;

    void revalidateGrid(Engine& engine);
};

#endif
