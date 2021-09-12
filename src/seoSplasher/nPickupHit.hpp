
#ifndef N_PICKUP_HIT_HPP
#define N_PICKUP_HIT_HPP

#include "../ec/node.hpp"

struct cPickup;

struct nPickupHit : Node
{
public:
    nPickupHit();

    cPickup* pickup;

    bool checkEntity(Entity& entity) override;
    std::unique_ptr<Node> getNewNode() override;
    void getCReferencesFromEntity(Entity& entity) override;
    void update(sf::Time dt, Context context) override;
private:
    int ID;
};

#endif
