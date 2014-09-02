
#ifndef N_PCONTROL_HPP
#define N_PCONTROL_HPP

#include "../ec/node.hpp"

struct cPlayerControl;
struct cPosition;
struct cVelocity;
struct cLiving;

struct nPControl : Node
{
    nPControl();

    cPlayerControl* control;
    cPosition* pos;
    cVelocity* vel;
    cLiving* living;

    bool checkEntity(Entity& entity);
    std::unique_ptr<Node> getNewNode();
    void getCReferencesFromEntity(Entity& entity);
    void update(sf::Time dt, Context context);
};

#endif
