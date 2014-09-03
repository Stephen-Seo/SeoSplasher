
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
#include "cDamage.hpp"
#include "cAnimated.hpp"
#include "cPickup.hpp"
#include "cPowerup.hpp"

HitInfo Utility::collideAll(const float& x, const float& y, Engine& engine)
{
    HitInfo info;

    for(auto eIter = engine.getEntityIterBegin(); eIter != engine.getEntityIterEnd(); ++eIter)
    {
        if(eIter->second->removed || !eIter->second->hasComponent(std::type_index(typeid(cPosition))))
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
        if(eIter->second->removed ||
           !eIter->second->hasComponent(std::type_index(typeid(cPosition))) ||
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
        if(eIter->second->removed || !eIter->second->hasComponent(std::type_index(typeid(cPosition))))
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

bool Utility::collidesAgainstComponent(const float& x, const float& y, const std::type_index& type, Engine& engine)
{
    for(auto eIter = engine.getEntityIterBegin(); eIter != engine.getEntityIterEnd(); ++eIter)
    {
        if(eIter->second->removed ||
           !eIter->second->hasComponent(std::type_index(typeid(cPosition))) ||
           !eIter->second->hasComponent(type))
        {
            continue;
        }
        cPosition* pos = static_cast<cPosition*>(eIter->second->getComponent(std::type_index(typeid(cPosition))));

        if(collide(x, y, pos->x, pos->y))
        {
            return true;
        }
    }

    return false;
}

bool Utility::collide(const float& xOne, const float& yOne, const float& xTwo, const float& yTwo)
{
    float halfSquare = ((float)GRID_SQUARE_SIZE) / 2.0f;
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
            yOne + (float)GRID_SQUARE_SIZE < yTwo + (float)GRID_SQUARE_SIZE)
            ||
           (xOne + halfSquare > xTwo &&
            xOne + halfSquare < xTwo + (float)GRID_SQUARE_SIZE &&
            yOne > yTwo &&
            yOne < yTwo + (float)GRID_SQUARE_SIZE) ||
           (xOne > xTwo &&
            xOne < xTwo + (float)GRID_SQUARE_SIZE &&
            yOne + halfSquare > yTwo &&
            yOne + halfSquare < yTwo + (float)GRID_SQUARE_SIZE) ||
           (xOne + halfSquare > xTwo &&
            xOne + halfSquare < xTwo + (float)GRID_SQUARE_SIZE &&
            yOne + (float)GRID_SQUARE_SIZE > yTwo &&
            yOne + (float)GRID_SQUARE_SIZE < yTwo + (float)GRID_SQUARE_SIZE) ||
           (xOne + (float)GRID_SQUARE_SIZE > xTwo &&
            xOne + (float)GRID_SQUARE_SIZE < xTwo + (float)GRID_SQUARE_SIZE &&
            yOne + halfSquare > yTwo &&
            yOne + halfSquare < yTwo + (float)GRID_SQUARE_SIZE)
            ||
           (xOne + halfSquare > xTwo &&
            xOne + halfSquare < xTwo + (float)GRID_SQUARE_SIZE &&
            yOne + halfSquare > yTwo &&
            yOne + halfSquare < yTwo + (float)GRID_SQUARE_SIZE);
}

void Utility::createBalloon(const float& x, const float& y, cLiving& living, const Context& context, unsigned char ID, bool* fired)
{
    if(living.balloonsInPlay >= DEFAULT_BALLOONS + living.balloonUp)
        return;

    ++living.balloonsInPlay;

    bool isSuper = false;

    Entity* balloon = new Entity;

    cPosition* pos = new cPosition;
    sf::Vector2f v = alignToGrid(x,y);
    pos->x = v.x;
    pos->y = v.y;
    pos->rot = 0.0f;
    balloon->addComponent(std::type_index(typeid(cPosition)), std::unique_ptr<Component>(pos));

    cBalloon* cballoon = new cBalloon;
    cballoon->ID = ID;
    cballoon->piercing = living.pierceUpgrade > 0;
    cballoon->spreading = living.spreadUpgrade > 0;
    cballoon->ghosting = living.ghostUpgrade > 0;
    if(living.sBalloonsInPlay < living.sBalloonUpgrade)
    {
        ++living.sBalloonsInPlay;
        cballoon->distance = SUPER_RANGE;
        isSuper = true;
        cballoon->super = true;
    }
    else
    {
        cballoon->distance = DEFAULT_RANGE + living.rangeUp;
        cballoon->super = false;
    }
    cballoon->balloonsInPlay = &living.balloonsInPlay;
    cballoon->sBalloonsInPlay = &living.sBalloonsInPlay;
    balloon->addComponent(std::type_index(typeid(cBalloon)), std::unique_ptr<Component>(cballoon));

    cSprite* sprite = new cSprite;
    if(isSuper)
    {
        sprite->sprite.setTexture(context.resourceManager->getTexture(Textures::SUPER_BALLOON_0));

        cAnimated* animated = new cAnimated;
        animated->textures.push_back(&context.resourceManager->getTexture(Textures::SUPER_BALLOON_0));
        animated->textures.push_back(&context.resourceManager->getTexture(Textures::SUPER_BALLOON_1));
        animated->textures.push_back(&context.resourceManager->getTexture(Textures::SUPER_BALLOON_2));
        balloon->addComponent(std::type_index(typeid(cAnimated)), std::unique_ptr<Component>(animated));
    }
    else
    {
        sprite->sprite.setTexture(context.resourceManager->getTexture(Textures::BALLOON_0));

        cAnimated* animated = new cAnimated;
        animated->textures.push_back(&context.resourceManager->getTexture(Textures::BALLOON_0));
        animated->textures.push_back(&context.resourceManager->getTexture(Textures::BALLOON_1));
        animated->textures.push_back(&context.resourceManager->getTexture(Textures::BALLOON_2));
        balloon->addComponent(std::type_index(typeid(cAnimated)), std::unique_ptr<Component>(animated));
    }
    balloon->addComponent(std::type_index(typeid(cSprite)), std::unique_ptr<Component>(sprite));

    if(living.rControlUpgrade > 0)
    {
        cControl* ccontrol = new cControl;
        ccontrol->fired = fired;
        balloon->addComponent(std::type_index(typeid(cControl)), std::unique_ptr<Component>(ccontrol));
    }
    else
    {
        cTimer* timer = new cTimer;
        timer->time = BALLOON_ALIVE_TIME;
        balloon->addComponent(std::type_index(typeid(cTimer)), std::unique_ptr<Component>(timer));
    }

    context.ecEngine->addEntity(std::unique_ptr<Entity>(balloon));
}

void Utility::createExplosion(const float& x, const float& y, cBalloon& balloon, const Context& context, bool horizontal, bool vertical)
{
    Entity* splosion = new Entity;

    cPosition* pos = new cPosition;
    pos->x = x;
    pos->y = y;
    pos->rot = 0.0f;
    splosion->addComponent(std::type_index(typeid(cPosition)), std::unique_ptr<Component>(pos));

    cDamage* damage = new cDamage;
    damage->ID = balloon.ID;
    damage->piercing = balloon.piercing;
    damage->spreading = balloon.spreading;
    damage->ghosting = balloon.ghosting;
    damage->distance = balloon.distance;
    damage->vertical = vertical;
    damage->horizontal = horizontal;
    splosion->addComponent(std::type_index(typeid(cDamage)), std::unique_ptr<Component>(damage));

    cSprite* sprite = new cSprite;
    if(vertical && horizontal)
    {
        sprite->sprite.setTexture(context.resourceManager->getTexture(Textures::SPLOSION_PLUS));
    }
    else if(vertical)
    {
        sprite->sprite.setTexture(context.resourceManager->getTexture(Textures::SPLOSION_VERT));
    }
    else
    {
        sprite->sprite.setTexture(context.resourceManager->getTexture(Textures::SPLOSION_HORIZ));
    }
    splosion->addComponent(std::type_index(typeid(cSprite)), std::unique_ptr<Component>(sprite));

    cTimer* timer = new cTimer;
    timer->time = SPLOSION_LIFETIME;
    splosion->addComponent(std::type_index(typeid(cTimer)), std::unique_ptr<Component>(timer));

    context.ecEngine->addEntity(std::unique_ptr<Entity>(splosion));
}

void Utility::createExplosion(const float& x, const float& y, cDamage& damage, const Context& context, bool horizontal, bool vertical)
{
    Entity* splosion = new Entity;

    cPosition* pos = new cPosition;
    pos->x = x;
    pos->y = y;
    pos->rot = 0.0f;
    splosion->addComponent(std::type_index(typeid(cPosition)), std::unique_ptr<Component>(pos));

    cDamage* cdamage = new cDamage;
    cdamage->ID = damage.ID;
    cdamage->piercing = damage.piercing;
    cdamage->spreading = damage.spreading;
    cdamage->ghosting = damage.ghosting;
    cdamage->distance = damage.distance - 1;
    cdamage->vertical = vertical;
    cdamage->horizontal = horizontal;
    splosion->addComponent(std::type_index(typeid(cDamage)), std::unique_ptr<Component>(cdamage));

    cSprite* sprite = new cSprite;
    if(horizontal && vertical)
    {
        sprite->sprite.setTexture(context.resourceManager->getTexture(Textures::SPLOSION_PLUS));
    }
    else if(horizontal)
    {
        sprite->sprite.setTexture(context.resourceManager->getTexture(Textures::SPLOSION_HORIZ));
    }
    else
    {
        sprite->sprite.setTexture(context.resourceManager->getTexture(Textures::SPLOSION_VERT));
    }
    splosion->addComponent(std::type_index(typeid(cSprite)), std::unique_ptr<Component>(sprite));

    cTimer* timer = new cTimer;
    timer->time = SPLOSION_LIFETIME;
    splosion->addComponent(std::type_index(typeid(cTimer)), std::unique_ptr<Component>(timer));

    context.ecEngine->addEntity(std::unique_ptr<Entity>(splosion));
}

sf::Vector2f Utility::alignToGrid(const float& x, const float& y)
{
    sf::Vector2f v;

    v.x = (float)((int)((x - (float)GRID_OFFSET_X) / 32.0f + 0.5f)) * 32.0f + (float)GRID_OFFSET_X;
    v.y = (float)((int)((y - (float)GRID_OFFSET_Y) / 32.0f + 0.5f)) * 32.0f + (float)GRID_OFFSET_Y;

    return v;
}

void Utility::createPowerup(const float& x, const float& y, cPowerup& powerup, const Context& context)
{
    if(powerup.powerup == cPowerup::NONE)
        return;

    Entity* epowerup = new Entity;

    cPosition* pos = new cPosition;
    pos->x = x;
    pos->y = y;
    pos->rot = 0.0f;
    epowerup->addComponent(std::type_index(typeid(cPosition)), std::unique_ptr<Component>(pos));

    cPowerup* cpowerup = new cPowerup;
    cpowerup->powerup = powerup.powerup;
    epowerup->addComponent(std::type_index(typeid(cPowerup)), std::unique_ptr<Component>(cpowerup));

    cSprite* sprite = new cSprite;
    cAnimated* animated = new cAnimated;

    animated->frameTime = 0.3f;
    switch(powerup.powerup)
    {
    case cPowerup::BALLOON_UP:
        sprite->sprite.setTexture(context.resourceManager->getTexture(Textures::BALLOON_UP_0));
        animated->textures.push_back(&context.resourceManager->getTexture(Textures::BALLOON_UP_0));
        animated->textures.push_back(&context.resourceManager->getTexture(Textures::BALLOON_UP_1));
        animated->textures.push_back(&context.resourceManager->getTexture(Textures::BALLOON_UP_2));
        break;
    case cPowerup::RANGE_UP:
        sprite->sprite.setTexture(context.resourceManager->getTexture(Textures::RANGE_UP_0));
        animated->textures.push_back(&context.resourceManager->getTexture(Textures::RANGE_UP_0));
        animated->textures.push_back(&context.resourceManager->getTexture(Textures::RANGE_UP_1));
        animated->textures.push_back(&context.resourceManager->getTexture(Textures::RANGE_UP_2));
        break;
    case cPowerup::SPEED_UP:
        sprite->sprite.setTexture(context.resourceManager->getTexture(Textures::SPEED_UP_0));
        animated->textures.push_back(&context.resourceManager->getTexture(Textures::SPEED_UP_0));
        animated->textures.push_back(&context.resourceManager->getTexture(Textures::SPEED_UP_1));
        animated->textures.push_back(&context.resourceManager->getTexture(Textures::SPEED_UP_2));
        break;
    case cPowerup::KICK_UPGRADE:
        sprite->sprite.setTexture(context.resourceManager->getTexture(Textures::KICK_UPGRADE_0));
        animated->textures.push_back(&context.resourceManager->getTexture(Textures::KICK_UPGRADE_0));
        animated->textures.push_back(&context.resourceManager->getTexture(Textures::KICK_UPGRADE_1));
        animated->textures.push_back(&context.resourceManager->getTexture(Textures::KICK_UPGRADE_2));
        break;
    case cPowerup::RCONTROL_UPGRADE:
        sprite->sprite.setTexture(context.resourceManager->getTexture(Textures::RCONTROL_UPGRADE_0));
        animated->textures.push_back(&context.resourceManager->getTexture(Textures::RCONTROL_UPGRADE_0));
        animated->textures.push_back(&context.resourceManager->getTexture(Textures::RCONTROL_UPGRADE_1));
        animated->textures.push_back(&context.resourceManager->getTexture(Textures::RCONTROL_UPGRADE_2));
        break;
    case cPowerup::SBALLOON_UPGRADE:
        sprite->sprite.setTexture(context.resourceManager->getTexture(Textures::SBALLOON_UPGRADE_0));
        animated->textures.push_back(&context.resourceManager->getTexture(Textures::SBALLOON_UPGRADE_0));
        animated->textures.push_back(&context.resourceManager->getTexture(Textures::SBALLOON_UPGRADE_1));
        animated->textures.push_back(&context.resourceManager->getTexture(Textures::SBALLOON_UPGRADE_2));
        break;
    case cPowerup::PIERCE_UPGRADE:
        sprite->sprite.setTexture(context.resourceManager->getTexture(Textures::PIERCE_UPGRADE_0));
        animated->textures.push_back(&context.resourceManager->getTexture(Textures::PIERCE_UPGRADE_0));
        animated->textures.push_back(&context.resourceManager->getTexture(Textures::PIERCE_UPGRADE_1));
        animated->textures.push_back(&context.resourceManager->getTexture(Textures::PIERCE_UPGRADE_2));
        break;
    case cPowerup::SPREAD_UPGRADE:
        sprite->sprite.setTexture(context.resourceManager->getTexture(Textures::SPREAD_UPGRADE_0));
        animated->textures.push_back(&context.resourceManager->getTexture(Textures::SPREAD_UPGRADE_0));
        animated->textures.push_back(&context.resourceManager->getTexture(Textures::SPREAD_UPGRADE_1));
        animated->textures.push_back(&context.resourceManager->getTexture(Textures::SPREAD_UPGRADE_2));
        break;
    case cPowerup::GHOST_UPGRADE:
        sprite->sprite.setTexture(context.resourceManager->getTexture(Textures::GHOST_UPGRADE_0));
        animated->textures.push_back(&context.resourceManager->getTexture(Textures::GHOST_UPGRADE_0));
        animated->textures.push_back(&context.resourceManager->getTexture(Textures::GHOST_UPGRADE_1));
        animated->textures.push_back(&context.resourceManager->getTexture(Textures::GHOST_UPGRADE_2));
        break;
    default:
        break;
    }
    epowerup->addComponent(std::type_index(typeid(cSprite)), std::unique_ptr<Component>(sprite));
    epowerup->addComponent(std::type_index(typeid(cAnimated)), std::unique_ptr<Component>(animated));

    epowerup->addComponent(std::type_index(typeid(cPickup)), std::unique_ptr<Component>(new cPickup));

    context.ecEngine->addEntity(std::unique_ptr<Entity>(epowerup));
}
