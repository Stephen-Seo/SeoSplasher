
#include "nBalloon.hpp"

#include <typeindex>

#include "../ec/cPosition.hpp"
#include "cBalloon.hpp"
#include "cControl.hpp"
#include "cTimer.hpp"
#include "../context.hpp"
#include "../ec/engine.hpp"
#include "utility.hpp"

nBalloon::nBalloon() :
pos(nullptr),
balloon(nullptr),
control(nullptr),
timer(nullptr)
{}

bool nBalloon::checkEntity(Entity& entity)
{
    return entity.hasComponent(std::type_index(typeid(cPosition))) &&
           entity.hasComponent(std::type_index(typeid(cBalloon))) &&
          (entity.hasComponent(std::type_index(typeid(cControl))) ||
           entity.hasComponent(std::type_index(typeid(cTimer))));
}

std::unique_ptr<Node> nBalloon::getNewNode()
{
    return std::unique_ptr<Node>(new nBalloon);
}

void nBalloon::getCReferencesFromEntity(Entity& entity)
{
    pos = static_cast<cPosition*>(entity.getComponent(std::type_index(typeid(cPosition))));
    balloon = static_cast<cBalloon*>(entity.getComponent(std::type_index(typeid(cBalloon))));
    if(entity.hasComponent(std::type_index(typeid(cControl))))
    {
        control = static_cast<cControl*>(entity.getComponent(std::type_index(typeid(cControl))));
    }
    else
    {
        timer = static_cast<cTimer*>(entity.getComponent(std::type_index(typeid(cTimer))));
    }
    entityRemoved = &entity.removed;

    ID = entity.getID();
}

void nBalloon::update(sf::Time dt, Context context)
{
    if(*entityRemoved)
        return;

    if(control)
    {
        if(*control->fired)
        {
            Utility::createExplosion(pos->x, pos->y, *balloon, context, true, true);
            --(*balloon->balloonsInPlay);
            if(balloon->super)
                --(*balloon->sBalloonsInPlay);
            context.ecEngine->removeEntity(ID);
        }
    }
    else
    {
        timer->time -= dt.asSeconds();
        if(timer->time <= 0.0f)
        {
            Utility::createExplosion(pos->x, pos->y, *balloon, context, true, true);
            --(*balloon->balloonsInPlay);
            if(balloon->super)
                --(*balloon->sBalloonsInPlay);
            context.ecEngine->removeEntity(ID);
        }
    }
}
