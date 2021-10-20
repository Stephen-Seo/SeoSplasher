
#ifndef UAI_HPP
#define UAI_HPP

#define DEFAULT_AI_WILDNESS 2

#include <map>
#include <random>

#include "pathFinder.hpp"

struct cPosition;
struct cLiving;
class Engine;
class PathFinder;

namespace AI
{
    enum Action {
        PLACE_BALLOON,
        GET_POWERUP,
        MOVE_TO_ENEMY,
        MOVE_TO_BREAKABLE,
        MOVE_TO_SAFETY,
        KICK_BALLOON,
        PANIC,
        WAIT,
        SIZE,
        NONE
    };
}

struct PAMapping
{
    PAMapping();
    PAMapping(float priority, std::map<int, int> paths, int destination);

    union {
        float priority;
        AI::Action action;
    };
    std::map<int, int> paths;
    int destination;
};

struct ActionElement
{
    ActionElement();
    ActionElement(AI::Action action, PAMapping pm);

    AI::Action action;
    PAMapping pm;

    bool operator< (const ActionElement& other) const;
};

class UAI
{
public:
    UAI();

    PAMapping determineAction(const cPosition& pos, const cLiving& living, PathFinder& pf, Engine& engine, std::mt19937& gen);
    void setWildness(unsigned char wildness);
    void setRisky(bool isRisky);
private:
    unsigned char wildness;
    bool isRisky;

    PAMapping utility(AI::Action action, const cPosition& pos, const cLiving& living, PathFinder& pf, Engine& engine);
    /**
     * returns bitfield
     * 0000 0000 0000 - nothing near
     * 0000 0000 0001 - a player in sight
     * 0000 0000 0010 - in WIndicator
     * 0000 0000 0100 - a breakable in sight
     * 0000 0000 1000 - a powerup in sight
     * 0000 0001 0000 - player exists
     * 0000 0010 0000 - breakable exists
     * 0000 0100 0000 - powerup exists
     * 0000 1000 0000 - on balloon
     * 0001 0000 0000 - adjacent to breakable
     * 0010 0000 0000 - adjacent to WIndicator
     * 0100 0000 0000 - surrounded
     */
    unsigned short nearbyInfo(const cPosition& pos, const PathFinder::ValidGridT &grid, Engine& engine);
};

#endif
