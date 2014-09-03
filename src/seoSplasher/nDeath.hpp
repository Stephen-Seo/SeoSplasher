
#ifndef N_DEATH_HPP
#define N_DEATH_HPP

#include "../ec/node.hpp"

struct cLiving;

struct nDeath : Node
{
public:
    nDeath();

    cLiving* living;

    bool checkEntity(Entity& entity);
    std::unique_ptr<Node> getNewNode();
    void getCReferencesFromEntity(Entity& entity);
    void update(sf::Time dt, Context context);
private:
    int ID;
};

#endif
