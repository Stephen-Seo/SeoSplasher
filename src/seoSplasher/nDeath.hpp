
#ifndef N_DEATH_HPP
#define N_DEATH_HPP

#include "../ec/node.hpp"

struct cLiving;

struct nDeath : Node
{
public:
    nDeath();

    cLiving* living;

    bool checkEntity(Entity& entity) override;
    std::unique_ptr<Node> getNewNode() override;
    void getCReferencesFromEntity(Entity& entity) override;
    void update(sf::Time dt, Context context) override;
private:
    int ID;
};

#endif
