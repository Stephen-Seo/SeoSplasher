
#ifndef N_PICKUP_HIT_HPP
#define N_PICKUP_HIT_HPP

#include "../ec/node.hpp"

struct cPickup;

struct nPickupHit : Node
{
public:
    nPickupHit();

    cPickup* pickup;

    bool checkEntity(Entity& entity);
    std::unique_ptr<Node> getNewNode();
    void getCReferencesFromEntity(Entity& entity);
    void update(sf::Time dt, Context context);
private:
    int ID;
};

#endif
