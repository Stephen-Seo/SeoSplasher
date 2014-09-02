
#include "nSplosion.hpp"

#include <typeindex>

#include "../ec/cPosition.hpp"
#include "cDamage.hpp"
#include "cTimer.hpp"
#include "utility.hpp"
#include "gridInfo.hpp"
#include "cWall.hpp"
#include "../context.hpp"
#include "../ec/engine.hpp"

nSplosion::nSplosion() :
pos(nullptr),
damage(nullptr),
timer(nullptr)
{}

bool nSplosion::checkEntity(Entity& entity)
{
    return entity.hasComponent(std::type_index(typeid(cPosition))) &&
           entity.hasComponent(std::type_index(typeid(cDamage))) &&
           entity.hasComponent(std::type_index(typeid(cTimer)));
}

std::unique_ptr<Node> nSplosion::getNewNode()
{
    return std::unique_ptr<Node>(new nSplosion);
}

void nSplosion::getCReferencesFromEntity(Entity& entity)
{
    pos = static_cast<cPosition*>(entity.getComponent(std::type_index(typeid(cPosition))));
    damage = static_cast<cDamage*>(entity.getComponent(std::type_index(typeid(cDamage))));
    timer = static_cast<cTimer*>(entity.getComponent(std::type_index(typeid(cTimer))));
    entityRemoved = &entity.removed;

    ID = entity.getID();
}

void nSplosion::update(sf::Time dt, Context context)
{
    if(*entityRemoved)
        return;

    if(!damage->spawned && damage->distance > 0)
    {
        damage->spawned = true;

        HitInfo info, infoTwo;
        if(damage->vertical)
        {
            // check up
            info = Utility::collideAgainstComponent(pos->x, pos->y - (float)GRID_SQUARE_SIZE, std::type_index(typeid(cWall)), *context.ecEngine);
            infoTwo = Utility::collideAgainstComponent(pos->x, pos->y - (float)GRID_SQUARE_SIZE, std::type_index(typeid(cDamage)), *context.ecEngine);
            if(info.hit.empty() && infoTwo.hit.empty())
            {
                Utility::createExplosion(pos->x, pos->y - (float)GRID_SQUARE_SIZE, *damage, context, false, true);
            }

            // check down
            info = Utility::collideAgainstComponent(pos->x, pos->y + (float)GRID_SQUARE_SIZE, std::type_index(typeid(cWall)), *context.ecEngine);
            infoTwo = Utility::collideAgainstComponent(pos->x, pos->y + (float)GRID_SQUARE_SIZE, std::type_index(typeid(cDamage)), *context.ecEngine);
            if(info.hit.empty() && infoTwo.hit.empty())
            {
                Utility::createExplosion(pos->x, pos->y + (float)GRID_SQUARE_SIZE, *damage, context, false, true);
            }
        }
        if(damage->horizontal)
        {

        }
    }

    timer->time -= dt.asSeconds();
    if(timer->time <= 0.0f)
    {
        context.ecEngine->removeEntity(ID);
    }
}
