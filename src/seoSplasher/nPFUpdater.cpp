
#include "nPFUpdater.hpp"

#include <typeindex>

#include "cPathFinder.hpp"
#include "../context.hpp"

nPFUpdater::nPFUpdater() :
cpf(nullptr)
{}

bool nPFUpdater::checkEntity(Entity& entity)
{
    return entity.hasComponent(std::type_index(typeid(cPathFinder)));
}

std::unique_ptr<Node> nPFUpdater::getNewNode()
{
    return std::unique_ptr<Node>(new nPFUpdater);
}

void nPFUpdater::getCReferencesFromEntity(Entity& entity)
{
    cpf = static_cast<cPathFinder*>(entity.getComponent(std::type_index(typeid(cPathFinder))));
    entityRemoved = &entity.removed;
}

void nPFUpdater::update(sf::Time dt, Context context)
{
    cpf->timer -= dt.asSeconds();
    if(cpf->timer <= 0.0f)
    {
        cpf->timer = PF_UPDATE_TIME;
        cpf->pf.invalidateValidGrid();
    }
}
