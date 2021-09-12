
#ifndef N_BREAKABLE_HPP
#define N_BREAKABLE_HPP

#include "../ec/node.hpp"

struct cPosition;
struct cBreakable;
struct cPowerup;

struct nBreakable : Node
{
    nBreakable();

    cPosition* pos;
    cBreakable* breakable;
    cPowerup* powerup;

    bool checkEntity(Entity& entity) override;
    std::unique_ptr<Node> getNewNode() override;
    void getCReferencesFromEntity(Entity& entity) override;
    void update(sf::Time dt, Context context) override;

    int ID;
};

#endif
