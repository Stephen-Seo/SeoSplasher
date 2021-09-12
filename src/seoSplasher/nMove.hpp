
#ifndef N_MOVE_HPP
#define N_MOVE_HPP

#define MAX_MOVE_OFFSET 8
#define MOVE_SNAP_OFFSET 3.0F

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
    bool isAI;

    bool checkEntity(Entity& entity) override;
    std::unique_ptr<Node> getNewNode() override;
    void getCReferencesFromEntity(Entity& entity) override;
    void update(sf::Time dt, Context context) override;
};

#endif
