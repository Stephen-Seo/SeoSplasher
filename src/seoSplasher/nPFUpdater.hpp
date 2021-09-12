
#ifndef N_PF_UPDATER_HPP
#define N_PF_UPDATER_HPP

#include "../ec/node.hpp"

struct cPathFinder;

struct nPFUpdater : Node
{
    nPFUpdater();

    cPathFinder* cpf;

    bool checkEntity(Entity& entity) override;
    std::unique_ptr<Node> getNewNode() override;
    void getCReferencesFromEntity(Entity& entity) override;
    void update(sf::Time dt, Context context) override;
};

#endif
