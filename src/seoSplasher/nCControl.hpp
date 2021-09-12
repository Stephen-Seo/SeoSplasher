
#ifndef N_CCONTROL_HPP
#define N_CCONTROL_HPP

#include "../ec/node.hpp"

struct cPosition;
struct cVelocity;
struct cLiving;
struct cPathFinderRef;

struct nCControl : Node
{
    nCControl();

    cPosition* pos;
    cVelocity* vel;
    cLiving* living;
    cPathFinderRef* pfRef;

    bool remoteFired;

    bool checkEntity(Entity& entity) override;
    std::unique_ptr<Node> getNewNode() override;
    void getCReferencesFromEntity(Entity& entity) override;
    void update(sf::Time dt, Context context) override;
};

#endif
