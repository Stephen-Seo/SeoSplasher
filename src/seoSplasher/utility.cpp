
#include "utility.hpp"

#include <memory>

#include "../ec/entity.hpp"
#include "../ec/engine.hpp"
#include "../ec/cPosition.hpp"
#include "gridInfo.hpp"
#include "cLiving.hpp"
#include "../context.hpp"
#include "cBalloon.hpp"
#include "cSprite.hpp"
#include "cControl.hpp"
#include "cTimer.hpp"
#include "../resourceManager.hpp"

HitInfo Utility::collideAll(const float& x, const float& y, Engine& engine)
{
    HitInfo info;

    for(auto eIter = engine.getEntityIterBegin(); eIter != engine.getEntityIterEnd(); ++eIter)
    {
        if(!eIter->second->hasComponent(std::type_index(typeid(cPosition))))
        {
            continue;
        }
        cPosition* pos = static_cast<cPosition*>(eIter->second->getComponent(std::type_index(typeid(cPosition))));

        if(collide(x, y, pos->x, pos->y))
        {
            info.hit.push_back(eIter->second.get());
        }
    }

    return info;
}

HitInfo Utility::collideAgainstComponent(const float& x, const float& y, const std::type_index& type, Engine& engine)
{
    HitInfo info;

    for(auto eIter = engine.getEntityIterBegin(); eIter != engine.getEntityIterEnd(); ++eIter)
    {
        if(!eIter->second->hasComponent(std::type_index(typeid(cPosition))) ||
           !eIter->second->hasComponent(type))
        {
            continue;
        }
        cPosition* pos = static_cast<cPosition*>(eIter->second->getComponent(std::type_index(typeid(cPosition))));

        if(collide(x, y, pos->x, pos->y))
        {
            info.hit.push_back(eIter->second.get());
        }
    }

    return info;
}

HitInfo Utility::collideAgainstComponentList(const float& x, const float& y, const std::list<std::type_index>& types, Engine& engine)
{
    HitInfo info;

    bool notFound;
    for(auto eIter = engine.getEntityIterBegin(); eIter != engine.getEntityIterEnd(); ++eIter)
    {
        if(!eIter->second->hasComponent(std::type_index(typeid(cPosition))))
        {
            continue;
        }
        notFound = false;
        for(auto tIter = types.begin(); tIter != types.end(); ++tIter)
        {
            if(!eIter->second->hasComponent(*tIter))
            {
                notFound = true;
                break;
            }
        }
        if(notFound)
        {
            continue;
        }

        cPosition* pos = static_cast<cPosition*>(eIter->second->getComponent(std::type_index(typeid(cPosition))));

        if(collide(x, y, pos->x, pos->y))
        {
            info.hit.push_back(eIter->second.get());
        }
    }

    return info;
}

bool Utility::collide(const float& xOne, const float& yOne, const float& xTwo, const float& yTwo)
{
    return (xOne < xTwo + (float)GRID_SQUARE_SIZE &&
            xOne > xTwo &&
            yOne > yTwo &&
            yOne < yTwo + (float)GRID_SQUARE_SIZE) ||
           (xOne + (float)GRID_SQUARE_SIZE > xTwo &&
            xOne + (float)GRID_SQUARE_SIZE < xTwo + (float)GRID_SQUARE_SIZE &&
            yOne > yTwo &&
            yOne < yTwo + (float)GRID_SQUARE_SIZE) ||
           (xOne > xTwo &&
            xOne < xTwo + (float)GRID_SQUARE_SIZE &&
            yOne + (float)GRID_SQUARE_SIZE > yTwo &&
            yOne + (float)GRID_SQUARE_SIZE < yTwo + (float)GRID_SQUARE_SIZE) ||
           (xOne + (float)GRID_SQUARE_SIZE > xTwo &&
            xOne + (float)GRID_SQUARE_SIZE < xTwo + (float)GRID_SQUARE_SIZE &&
            yOne + (float)GRID_SQUARE_SIZE > yTwo &&
            yOne + (float)GRID_SQUARE_SIZE < yTwo + (float)GRID_SQUARE_SIZE);
}

void Utility::createBalloon(const float& x, const float& y, cLiving& living, const Context& context, unsigned char ID)
{
    bool isSuper = false;

    Entity* balloon = new Entity;

    cPosition* pos = new cPosition;
    pos->x = x;
    pos->y = y;
    pos->rot = 0.0f;
    balloon->addComponent(std::type_index(typeid(cPosition)), std::unique_ptr<Component>(pos));

    cBalloon* cballoon = new cBalloon;
    cballoon->ID = ID;
    cballoon->piercing = living.pierceUpgrade > 0;
    cballoon->spreading = living.spreadUpgrade > 0;
    cballoon->ghosting = living.ghostUpgrade > 0;
    if(living.sBalloonInPlay < living.sBalloonUpgrade)
    {
        ++living.sBalloonInPlay;
        cballoon->distance = SUPER_RANGE;
        isSuper = true;
    }
    else
    {
        cballoon->distance = DEFAULT_RANGE + living.rangeUp;
    }
    balloon->addComponent(std::type_index(typeid(cBalloon)), std::unique_ptr<Component>(cballoon));

    cSprite* sprite = new cSprite;
    if(isSuper)
    {
        sprite->sprite.setTexture(context.resourceManager->getTexture(Textures::SUPER_BALLOON_0));
    }
    else
    {
        sprite->sprite.setTexture(context.resourceManager->getTexture(Textures::BALLOON_0));
    }
    balloon->addComponent(std::type_index(typeid(cSprite)), std::unique_ptr<Component>(sprite));

    if(living.rControlUpgrade > 0)
    {
        balloon->addComponent(std::type_index(typeid(cControl)), std::unique_ptr<Component>(new cControl));
    }
    else
    {
        cTimer* timer = new cTimer;
        timer->time = BALLOON_ALIVE_TIME;
        balloon->addComponent(std::type_index(typeid(cTimer)), std::unique_ptr<Component>(timer));
    }

    context.ecEngine->addEntity(std::unique_ptr<Entity>(balloon));
}
