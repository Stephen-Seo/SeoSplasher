
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

    bool checkEntity(Entity& entity);
    std::unique_ptr<Node> getNewNode();
    void getCReferencesFromEntity(Entity& entity);
    void update(sf::Time dt, Context context);

    int ID;
};

#endif
