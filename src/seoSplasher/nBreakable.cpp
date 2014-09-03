
#include "nBreakable.hpp"

#include <typeindex>

#include "../ec/cPosition.hpp"
#include "cBreakable.hpp"
#include "cPowerup.hpp"
#include "../context.hpp"
#include "../ec/engine.hpp"
#include "utility.hpp"

nBreakable::nBreakable() :
pos(nullptr),
breakable(nullptr),
powerup(nullptr)
{}

bool nBreakable::checkEntity(Entity& entity)
{
    return entity.hasComponent(std::type_index(typeid(cPosition))) &&
           entity.hasComponent(std::type_index(typeid(cBreakable))) &&
           entity.hasComponent(std::type_index(typeid(cPowerup)));
}

std::unique_ptr<Node> nBreakable::getNewNode()
{
    return std::unique_ptr<Node>(new nBreakable);
}

void nBreakable::getCReferencesFromEntity(Entity& entity)
{
    pos = static_cast<cPosition*>(entity.getComponent(std::type_index(typeid(cPosition))));
    breakable = static_cast<cBreakable*>(entity.getComponent(std::type_index(typeid(cBreakable))));
    powerup = static_cast<cPowerup*>(entity.getComponent(std::type_index(typeid(cPowerup))));
    entityRemoved = &entity.removed;

    ID = entity.getID();
}

void nBreakable::update(sf::Time dt, Context context)
{
    if(*entityRemoved)
        return;

    if(breakable->health == 0)
    {
        Utility::createPowerup(pos->x, pos->y, *powerup, context);
        context.ecEngine->removeEntity(ID);
    }
}
