
#ifndef N_MOVE_HPP
#define N_MOVE_HPP

#define MAX_MOVE_OFFSET 8
#define MOVE_SNAP_OFFSET 3.0f

#include <list>
#include <typeindex>

#include "../ec/node.hpp"

struct cPosition;
struct cVelocity;
struct cLiving;
struct cMoveDisabler;

struct nMove : Node
{
    nMove();

    cPosition* pos;
    cVelocity* vel;
    cLiving* living;
    cMoveDisabler* disable;

    bool isBalloon;

    bool checkEntity(Entity& entity);
    std::unique_ptr<Node> getNewNode();
    void getCReferencesFromEntity(Entity& entity);
    void update(sf::Time dt, Context context);
};

#endif
