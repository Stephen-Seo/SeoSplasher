
#include "utility.hpp"

#include <memory>
#include <cmath>

#include "../ec/entity.hpp"
#include "../ec/engine.hpp"
#include "../ec/cPosition.hpp"
#include "../ec/cVelocity.hpp"
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
#include "cWIndicator.hpp"
#include "cPathFinderRef.hpp"
#include "cPathFinder.hpp"
#include "nMove.hpp"
#include "splashNetworkIdentifiers.hpp"

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

bool Utility::collidesAll(const float& x, const float& y, Engine& engine)
{
    for(auto eIter = engine.getEntityIterBegin(); eIter != engine.getEntityIterEnd(); ++eIter)
    {
        if(eIter->second->removed || !eIter->second->hasComponent(std::type_index(typeid(cPosition))))
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

bool Utility::createBalloon(const float& x, const float& y, cLiving& living, const Context& context, unsigned char ID, bool* fired, cPathFinderRef& pfRef)
{
    if(living.balloonsInPlay >= DEFAULT_BALLOONS + living.balloonUp)
        return false;

    ++living.balloonsInPlay;

    bool isSuper = false;

    Entity* balloon = new Entity;

    cPosition* pos = new cPosition;
    sf::Vector2f v = alignToGrid(x,y);
    pos->x = v.x;
    pos->y = v.y;
    pos->rot = 0.0f;
    balloon->addComponent(std::type_index(typeid(cPosition)), std::unique_ptr<Component>(pos));

    cVelocity* vel = new cVelocity;
    vel->x = 0.0;
    vel->y = 0.0;
    vel->rot = 0.0f;
    balloon->addComponent(std::type_index(typeid(cVelocity)), std::unique_ptr<Component>(vel));

    unsigned char distance;
    cBalloon* cballoon = new cBalloon;
    cballoon->ID = ID;
    if(living.sBalloonsInPlay < living.sBalloonUpgrade)
    {
        ++living.sBalloonsInPlay;
        distance = SUPER_RANGE;
        isSuper = true;
        cballoon->super = true;
    }
    else
    {
        distance = DEFAULT_RANGE + living.rangeUp;
        cballoon->super = false;
    }
    cballoon->balloonsInPlay = &living.balloonsInPlay;
    cballoon->sBalloonsInPlay = &living.sBalloonsInPlay;
    cballoon->range = distance;
    cballoon->piercing = living.pierceUpgrade > 0;
    cballoon->ghosting = living.ghostUpgrade > 0;
    balloon->addComponent(std::type_index(typeid(cBalloon)), std::unique_ptr<Component>(cballoon));

    balloon->addComponent(std::type_index(typeid(cPathFinderRef)), std::unique_ptr<Component>(new cPathFinderRef(pfRef.cpf)));

    cSprite* sprite = new cSprite;
    if(living.rControlUpgrade == 0)
    {
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
    }
    else
    {
        if(isSuper)
        {
            sprite->sprite.setTexture(context.resourceManager->getTexture(Textures::C_SUPER_BALLOON_0));

            cAnimated* animated = new cAnimated;
            animated->textures.push_back(&context.resourceManager->getTexture(Textures::C_SUPER_BALLOON_0));
            animated->textures.push_back(&context.resourceManager->getTexture(Textures::C_SUPER_BALLOON_1));
            animated->textures.push_back(&context.resourceManager->getTexture(Textures::C_SUPER_BALLOON_2));
            balloon->addComponent(std::type_index(typeid(cAnimated)), std::unique_ptr<Component>(animated));
        }
        else
        {
            sprite->sprite.setTexture(context.resourceManager->getTexture(Textures::C_BALLOON_0));

            cAnimated* animated = new cAnimated;
            animated->textures.push_back(&context.resourceManager->getTexture(Textures::C_BALLOON_0));
            animated->textures.push_back(&context.resourceManager->getTexture(Textures::C_BALLOON_1));
            animated->textures.push_back(&context.resourceManager->getTexture(Textures::C_BALLOON_2));
            balloon->addComponent(std::type_index(typeid(cAnimated)), std::unique_ptr<Component>(animated));
        }
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

    if(*context.mode != 0 && *context.mode != 1) // not singleplayer or client
    {
        BalloonInfo balloonInfo;

        balloonInfo.xy = (sf::Uint8)((v.x - (float)GRID_OFFSET_X) / GRID_SQUARE_SIZE) + ((sf::Uint8)((v.y - (float)GRID_OFFSET_Y) / GRID_SQUARE_SIZE) * GRID_WIDTH);
        balloonInfo.typeRange = (distance >= 16 ? 0xF0 : distance << 4);
        balloonInfo.typeRange |= (isSuper ? 0x1 : 0x0) | (living.rControlUpgrade > 0 ? 0x2 : 0x0) | (living.pierceUpgrade > 0 ? 0x4 : 0x0) | (living.ghostUpgrade > 0 ? 0x8 : 0x0);

        context.scontext->balloons.insert(std::make_pair(balloon->getID(), balloonInfo));

        context.ecEngine->registerRemoveCall(balloon->getID(), [balloon, context] () {
            context.scontext->balloons.erase(balloon->getID());
        });
    }

    return true;
}

void Utility::createExplosion(const float& x, const float& y, Direction::Direction dir, const Context& context, unsigned char ID)
{
    Entity* splosion = new Entity;

    cPosition* pos = new cPosition;
    pos->x = x;
    pos->y = y;
    pos->rot = 0.0f;
    splosion->addComponent(std::type_index(typeid(cPosition)), std::unique_ptr<Component>(pos));

    cDamage* damage = new cDamage;
    damage->ID = ID;
    splosion->addComponent(std::type_index(typeid(cDamage)), std::unique_ptr<Component>(damage));

    cSprite* sprite = new cSprite;
    switch(dir)
    {
    default:
    case Direction::PLUS:
        sprite->sprite.setTexture(context.resourceManager->getTexture(Textures::SPLOSION_PLUS));
        break;
    case Direction::HORIZONTAL:
        sprite->sprite.setTexture(context.resourceManager->getTexture(Textures::SPLOSION_HORIZ));
        break;
    case Direction::VERTICAL:
        sprite->sprite.setTexture(context.resourceManager->getTexture(Textures::SPLOSION_VERT));
        break;
    }
    splosion->addComponent(std::type_index(typeid(cSprite)), std::unique_ptr<Component>(sprite));

    cTimer* timer = new cTimer;
    timer->time = SPLOSION_LIFETIME;
    splosion->addComponent(std::type_index(typeid(cTimer)), std::unique_ptr<Component>(timer));

    context.ecEngine->addEntity(std::unique_ptr<Entity>(splosion));

    if(*context.mode != 0 && *context.mode != 1) // not singleplayer or client
    {
        sf::Uint8 xy = (sf::Uint8)((x - (float)GRID_OFFSET_X) / GRID_SQUARE_SIZE) + ((sf::Uint8)((y - (float)GRID_OFFSET_Y) / GRID_SQUARE_SIZE) * GRID_WIDTH);

        context.scontext->explosions.insert(std::make_pair(splosion->getID(), xy));

        context.ecEngine->registerRemoveCall(splosion->getID(), [splosion, context] () {
            context.scontext->explosions.erase(splosion->getID());
        });
    }
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

    if(*context.mode != 0 && *context.mode != 1) // not singleplayer or client
    {
        PowerupInfo powerupInfo;
        powerupInfo.xy = (sf::Uint8)((x - (float)GRID_OFFSET_X) / GRID_SQUARE_SIZE) + ((sf::Uint8)((y - (float)GRID_OFFSET_Y) / GRID_SQUARE_SIZE) / GRID_WIDTH);
        powerupInfo.type = (sf::Uint8) powerup.powerup;

        context.scontext->powerups.insert(std::make_pair(epowerup->getID(), powerupInfo));

        context.ecEngine->registerRemoveCall(epowerup->getID(), [epowerup, context] () {
            context.scontext->powerups.erase(epowerup->getID());
        });
    }
}

int Utility::createWIndicator(const float& x, const float& y, Direction::Direction dir, const Context& context, unsigned char ID)
{
    Entity* indicator = new Entity;

    cPosition* pos = new cPosition;
    pos->x = x;
    pos->y = y;
    pos->rot = 0.0f;
    indicator->addComponent(std::type_index(typeid(cPosition)), std::unique_ptr<Component>(pos));

    cWIndicator* windicator = new cWIndicator;
    windicator->dir = dir;
    indicator->addComponent(std::type_index(typeid(cWIndicator)), std::unique_ptr<Component>(windicator));

    context.ecEngine->addEntity(std::unique_ptr<Entity>(indicator));

    context.ecEngine->registerRemoveCall(indicator->getID(), [x, y, dir, context, ID] () {
        Utility::createExplosion(x, y, dir, context, ID);
    });

    return indicator->getID();
}

sf::Vector2f Utility::alignToGrid(const float& x, const float& y)
{
    sf::Vector2f v;

    v.x = (float)((int)((x - (float)GRID_OFFSET_X) / 32.0f + 0.5f)) * 32.0f + (float)GRID_OFFSET_X;
    v.y = (float)((int)((y - (float)GRID_OFFSET_Y) / 32.0f + 0.5f)) * 32.0f + (float)GRID_OFFSET_Y;

    return v;
}

bool Utility::isAligned(const float& x, const float& y)
{
    sf::Vector2f v = alignToGrid(x, y);
    int diffx = std::abs(x - v.x);
    int diffy = std::abs(y - v.y);
    if(diffx <= MAX_MOVE_OFFSET && diffy <= MAX_MOVE_OFFSET)
        return true;
    return false;
}
