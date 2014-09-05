
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

    std::list<sf::Vector2i> getValidDestinations(const cPosition& pos, Engine& engine, std::list<std::type_index> obstacles = std::list<std::type_index>());
    std::list<sf::Vector2i> getPathToDestination(const cPosition& pos, const sf::Vector2f& destination, Engine& engine, std::list<std::type_index> obstacles = std::list<std::type_index>());
    bool isValidDestination(const cPosition& pos, const sf::Vector2f& destination, Engine& engine, std::list<std::type_index> obstacles = std::list<std::type_index>());
    void invalidateValidGrid();
    const unsigned char* getValidGrid(Engine& engine);
private:
    /**
     * array of bitfields.
     * 0000 0000 - nothing
     * 0000 0001 - a player
     * 0000 0010 - balloon
     * 0000 0100 - breakable
     * 0000 1000 - powerup
     * 0001 0000 - wall
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
