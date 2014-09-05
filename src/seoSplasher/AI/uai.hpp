
#ifndef UAI_HPP
#define UAI_HPP

#define DEFAULT_AI_WILDNESS 2

#include <random>

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
        SIZE,
        NONE
    };
}

struct ActionElement
{
    ActionElement();
    ActionElement(AI::Action action, int priority);

    AI::Action action;
    int priority;

    bool operator< (const ActionElement& other) const;
};

class UAI
{
public:
    UAI();

    AI::Action determineAction(const cPosition& pos, const cLiving& living, PathFinder& pf, Engine& engine, std::mt19937& gen);
    void setWildness(unsigned char wildness);
    void setRisky(bool isRisky);
private:
    unsigned char wildness;
    bool isRisky;

    int utility(AI::Action action, const cPosition& pos, const cLiving& living, PathFinder& pf, Engine& engine);
    /**
     * returns bitfield
     * 0000 0000 - nothing near
     * 0000 0001 - a player in sight
     * 0000 0010 - a balloon in sight
     * 0000 0100 - a breakable in sight
     * 0000 1000 - a powerup in sight
     * 0001 0000 - player exists
     * 0010 0000 - breakable exists
     * 0100 0000 - powerup exists
     * 1000 0000 - on balloon
     */
    unsigned char nearbyInfo(const cPosition& pos, PathFinder& pf, Engine& engine);
};

#endif
