
#include "nSplosion.hpp"

#include <typeindex>

#include "../ec/cPosition.hpp"
#include "cDamage.hpp"
#include "cTimer.hpp"
#include "utility.hpp"
#include "gridInfo.hpp"
#include "../context.hpp"
#include "../ec/engine.hpp"
#include "cBreakable.hpp"
#include "cBalloon.hpp"
#include "cLiving.hpp"
#include "cPickup.hpp"

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

    if(!damage->hitCheck)
    {
        // hit check
        damage->hitCheck = true;

        HitInfo infoBreak = Utility::collideAgainstComponent(pos->x, pos->y, std::type_index(typeid(cBreakable)), *context.ecEngine);

        HitInfo infoBalloon = Utility::collideAgainstComponent(pos->x, pos->y, std::type_index(typeid(cBalloon)), *context.ecEngine);

        HitInfo infoLiving = Utility::collideAgainstComponent(pos->x, pos->y, std::type_index(typeid(cLiving)), *context.ecEngine);

        HitInfo infoPowerup = Utility::collideAgainstComponent(pos->x, pos->y, std::type_index(typeid(cPickup)), *context.ecEngine);

        for(auto iter = infoBreak.hit.begin(); iter != infoBreak.hit.end(); ++iter)
        {
            cBreakable* breakable = static_cast<cBreakable*>((*iter)->getComponent(std::type_index(typeid(cBreakable))));
            if(breakable->health > 0)
                --(breakable->health);
        }

        for(auto iter = infoBalloon.hit.begin(); iter != infoBalloon.hit.end(); ++iter)
        {
            cBalloon* balloon = static_cast<cBalloon*>((*iter)->getComponent(std::type_index(typeid(cBalloon))));
            balloon->hit = true;
        }

        for(auto iter = infoLiving.hit.begin(); iter != infoLiving.hit.end(); ++iter)
        {
            cLiving* living = static_cast<cLiving*>((*iter)->getComponent(std::type_index(typeid(cLiving))));
            living->hit = true;
        }

        for(auto iter = infoPowerup.hit.begin(); iter != infoPowerup.hit.end(); ++iter)
        {
            cPickup* pickup = static_cast<cPickup*>((*iter)->getComponent(std::type_index(typeid(cPickup))));
            pickup->hit = true;
        }
    }

    timer->time -= dt.asSeconds();
    if(timer->time <= 0.0f)
    {
        context.ecEngine->removeEntity(ID);
    }
}
