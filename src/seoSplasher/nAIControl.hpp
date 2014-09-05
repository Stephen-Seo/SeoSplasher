
#ifndef N_AI_CONTROL_HPP
#define N_AI_CONTROL_HPP

#include "../ec/node.hpp"

struct cAIControl;
struct cLiving;
struct cPosition;
struct cVelocity;
struct cPathFinderRef;

struct nAIControl : Node
{
    nAIControl();

    cAIControl* control;
    cLiving* living;
    cPosition* pos;
    cVelocity* vel;
    cPathFinderRef* pfref;

    bool checkEntity(Entity& entity);
    std::unique_ptr<Node> getNewNode();
    void getCReferencesFromEntity(Entity& entity);
    void update(sf::Time dt, Context context);
};

#endif
