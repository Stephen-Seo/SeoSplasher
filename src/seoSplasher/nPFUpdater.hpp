
#ifndef N_PF_UPDATER_HPP
#define N_PF_UPDATER_HPP

#include "../ec/node.hpp"

struct cPathFinder;

struct nPFUpdater : Node
{
    nPFUpdater();

    cPathFinder* cpf;

    bool checkEntity(Entity& entity);
    std::unique_ptr<Node> getNewNode();
    void getCReferencesFromEntity(Entity& entity);
    void update(sf::Time dt, Context context);
};

#endif
