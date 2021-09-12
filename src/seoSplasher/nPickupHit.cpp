
#include "nPickupHit.hpp"

#include <typeindex>

#include "../context.hpp"
#include "../ec/engine.hpp"
#include "cPickup.hpp"

nPickupHit::nPickupHit() :
pickup(nullptr)
{}

bool nPickupHit::checkEntity(Entity& entity)
{
    return entity.hasComponent(std::type_index(typeid(cPickup)));
}

std::unique_ptr<Node> nPickupHit::getNewNode()
{
    return std::unique_ptr<Node>(new nPickupHit());
}

void nPickupHit::getCReferencesFromEntity(Entity& entity)
{
    pickup = static_cast<cPickup*>(entity.getComponent(std::type_index(typeid(cPickup))));
    entityRemoved = &entity.removed;
    ID = entity.getID();
}

void nPickupHit::update(sf::Time /*dt*/, Context context)
{
    if(*entityRemoved)
        return;

    if(pickup->hit)
    {
        context.ecEngine->removeEntity(ID);
    }
}
