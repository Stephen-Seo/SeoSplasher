
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
#include "cBreakable.hpp"
#include "soundContext.hpp"

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
        for(auto type : types)
        {
            if(!eIter->second->hasComponent(type))
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
    float halfSquare = ((float)GRID_SQUARE_SIZE) / 2.0F;
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

    sf::Vector2f v = alignToGrid(x,y);
    // check if balloon already exists in spot
    HitInfo hitInfo = collideAgainstComponent(v.x, v.y, std::type_index(typeid(cBalloon)), *context.ecEngine);
    if(!hitInfo.hit.empty())
        return false;

    ++living.balloonsInPlay;

    bool isSuper = false;

    Entity* balloon = new Entity();

    cPosition* pos = new cPosition();
    pos->x = v.x;
    pos->y = v.y;
    pos->rot = 0.0F;
    balloon->addComponent(std::type_index(typeid(cPosition)), std::unique_ptr<Component>(pos));

    cVelocity* vel = new cVelocity();
    vel->x = 0.0;
    vel->y = 0.0;
    vel->rot = 0.0F;
    balloon->addComponent(std::type_index(typeid(cVelocity)), std::unique_ptr<Component>(vel));

    unsigned char distance;
    cBalloon* cballoon = new cBalloon();
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

    cSprite* sprite = new cSprite();
    if(living.rControlUpgrade == 0)
    {
        if(isSuper)
        {
            sprite->sprite.setTexture(context.resourceManager->getTexture(Textures::SUPER_BALLOON_0));

            cAnimated* animated = new cAnimated();
            animated->textures.push_back(&context.resourceManager->getTexture(Textures::SUPER_BALLOON_0));
            animated->textures.push_back(&context.resourceManager->getTexture(Textures::SUPER_BALLOON_1));
            animated->textures.push_back(&context.resourceManager->getTexture(Textures::SUPER_BALLOON_2));
            balloon->addComponent(std::type_index(typeid(cAnimated)), std::unique_ptr<Component>(animated));
        }
        else
        {
            sprite->sprite.setTexture(context.resourceManager->getTexture(Textures::BALLOON_0));

            cAnimated* animated = new cAnimated();
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

            cAnimated* animated = new cAnimated();
            animated->textures.push_back(&context.resourceManager->getTexture(Textures::C_SUPER_BALLOON_0));
            animated->textures.push_back(&context.resourceManager->getTexture(Textures::C_SUPER_BALLOON_1));
            animated->textures.push_back(&context.resourceManager->getTexture(Textures::C_SUPER_BALLOON_2));
            balloon->addComponent(std::type_index(typeid(cAnimated)), std::unique_ptr<Component>(animated));
        }
        else
        {
            sprite->sprite.setTexture(context.resourceManager->getTexture(Textures::C_BALLOON_0));

            cAnimated* animated = new cAnimated();
            animated->textures.push_back(&context.resourceManager->getTexture(Textures::C_BALLOON_0));
            animated->textures.push_back(&context.resourceManager->getTexture(Textures::C_BALLOON_1));
            animated->textures.push_back(&context.resourceManager->getTexture(Textures::C_BALLOON_2));
            balloon->addComponent(std::type_index(typeid(cAnimated)), std::unique_ptr<Component>(animated));
        }
    }
    balloon->addComponent(std::type_index(typeid(cSprite)), std::unique_ptr<Component>(sprite));

    if(living.rControlUpgrade > 0)
    {
        cControl* ccontrol = new cControl();
        ccontrol->fired = fired;
        balloon->addComponent(std::type_index(typeid(cControl)), std::unique_ptr<Component>(ccontrol));
    }
    else
    {
        cTimer* timer = new cTimer();
        timer->time = BALLOON_ALIVE_TIME;
        balloon->addComponent(std::type_index(typeid(cTimer)), std::unique_ptr<Component>(timer));
    }

    context.ecEngine->addEntity(std::unique_ptr<Entity>(balloon));

    if(*context.mode != 0 && *context.mode != 1) // not singleplayer or client
    {
        BalloonInfo balloonInfo;

        balloonInfo.EID = balloon->getID();
        balloonInfo.posx = &(pos->x);
        balloonInfo.posy = &(pos->y);
        balloonInfo.velx = &(vel->x);
        balloonInfo.vely = &(vel->y);
        balloonInfo.typeRange = (distance >= 16 ? 0xF0 : ((sf::Uint8) distance) << 4);
        balloonInfo.typeRange |= (isSuper ? 0x1 : 0x0) | (living.rControlUpgrade > 0 ? 0x2 : 0x0) | (living.pierceUpgrade > 0 ? 0x4 : 0x0) | (living.ghostUpgrade > 0 ? 0x8 : 0x0);

        context.scontext->balloons.insert(std::make_pair(balloon->getID(), balloonInfo));

        context.ecEngine->registerRemoveCall(balloon->getID(), [balloon, context] () {
            context.scontext->balloons.erase(balloon->getID());
        });
    }

    // trigger explosion on balloon removal
    context.ecEngine->registerRemoveCall(balloon->getID(), [context] () {
        context.sfxContext->happened[SoundContext::SPLOSION] = true;
    });

    return true;
}

void Utility::createExplosion(const float& x, const float& y, Direction::Direction dir, const Context& context, unsigned char ID)
{
    Entity* splosion = new Entity();

    cPosition* pos = new cPosition();
    pos->x = x;
    pos->y = y;
    pos->rot = 0.0F;
    splosion->addComponent(std::type_index(typeid(cPosition)), std::unique_ptr<Component>(pos));

    cDamage* damage = new cDamage();
    damage->ID = ID;
    splosion->addComponent(std::type_index(typeid(cDamage)), std::unique_ptr<Component>(damage));

    cSprite* sprite = new cSprite();
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

    cTimer* timer = new cTimer();
    timer->time = SPLOSION_LIFETIME;
    splosion->addComponent(std::type_index(typeid(cTimer)), std::unique_ptr<Component>(timer));

    context.ecEngine->addEntity(std::unique_ptr<Entity>(splosion));

    if(*context.mode != 0 && *context.mode != 1) // not singleplayer or client
    {
        ExplosionInfo explosionInfo;
        explosionInfo.xy = (sf::Uint8)((x - (float)GRID_OFFSET_X) / GRID_SQUARE_SIZE) + ((sf::Uint8)((y - (float)GRID_OFFSET_Y) / GRID_SQUARE_SIZE) * GRID_WIDTH);
        switch(dir)
        {
        case Direction::HORIZONTAL:
            explosionInfo.direction = 0;
            break;
        case Direction::VERTICAL:
            explosionInfo.direction = 1;
            break;
        case Direction::PLUS:
            explosionInfo.direction = 2;
            break;
        default:
            explosionInfo.direction = 2;
            break;
        }

        context.scontext->explosions.insert(std::make_pair(splosion->getID(), explosionInfo));

        context.ecEngine->registerRemoveCall(splosion->getID(), [splosion, context] () {
            context.scontext->explosions.erase(splosion->getID());
        });
    }
}

void Utility::createPowerup(const float& x, const float& y, cPowerup& powerup, const Context& context)
{
    if(powerup.powerup == cPowerup::NONE)
        return;

    Entity* epowerup = new Entity();

    cPosition* pos = new cPosition();
    pos->x = x;
    pos->y = y;
    pos->rot = 0.0F;
    epowerup->addComponent(std::type_index(typeid(cPosition)), std::unique_ptr<Component>(pos));

    cPowerup* cpowerup = new cPowerup();
    cpowerup->powerup = powerup.powerup;
    epowerup->addComponent(std::type_index(typeid(cPowerup)), std::unique_ptr<Component>(cpowerup));

    cSprite* sprite = new cSprite();
    cAnimated* animated = new cAnimated();

    animated->frameTime = 0.3F;
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

    epowerup->addComponent(std::type_index(typeid(cPickup)), std::unique_ptr<Component>(new cPickup()));

    context.ecEngine->addEntity(std::unique_ptr<Entity>(epowerup));

    if(*context.mode != 0 && *context.mode != 1) // not singleplayer or client
    {
        PowerupInfo powerupInfo;
        powerupInfo.xy = (sf::Uint8)((x - (float)GRID_OFFSET_X) / GRID_SQUARE_SIZE) + ((sf::Uint8)((y - (float)GRID_OFFSET_Y) / GRID_SQUARE_SIZE) * GRID_WIDTH);
        powerupInfo.type = (sf::Uint8) powerup.powerup;

        context.scontext->powerups.insert(std::make_pair(epowerup->getID(), powerupInfo));

        context.ecEngine->registerRemoveCall(epowerup->getID(), [epowerup, context] () {
            context.scontext->powerups.erase(epowerup->getID());
        });
    }
}

int Utility::createWIndicator(const float& x, const float& y, Direction::Direction dir, const Context& context, unsigned char ID)
{
    Entity* indicator = new Entity();

    cPosition* pos = new cPosition();
    pos->x = x;
    pos->y = y;
    pos->rot = 0.0F;
    indicator->addComponent(std::type_index(typeid(cPosition)), std::unique_ptr<Component>(pos));

    cWIndicator* windicator = new cWIndicator();
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

    v.x = (float)((int)((x - (float)GRID_OFFSET_X) / 32.0F + 0.5F)) * 32.0F + (float)GRID_OFFSET_X;
    v.y = (float)((int)((y - (float)GRID_OFFSET_Y) / 32.0F + 0.5F)) * 32.0F + (float)GRID_OFFSET_Y;

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

BalloonInfo Utility::clientCreateBalloon(const float& x, const float& y, sf::Uint8 typeRange, Context context)
{
    BalloonInfo balloonInfo;
    balloonInfo.typeRange = typeRange;

    std::unique_ptr<Entity> balloon(new Entity());
    balloonInfo.EID = balloon->getID();

    std::unique_ptr<Component> pos(new cPosition());
    cPosition* cpos = static_cast<cPosition*>(pos.get());
    cpos->x = x;
    cpos->y = y;
    balloonInfo.posx = &(cpos->x);
    balloonInfo.posy = &(cpos->y);
    balloon->addComponent(std::type_index(typeid(cPosition)), std::move(pos));

    std::unique_ptr<Component> vel(new cVelocity());
    cVelocity* cvel = static_cast<cVelocity*>(vel.get());
    cvel->x = 0.0F;
    cvel->y = 0.0F;
    balloonInfo.velx = &(cvel->x);
    balloonInfo.vely = &(cvel->y);
    balloon->addComponent(std::type_index(typeid(cVelocity)), std::move(vel));

    std::unique_ptr<Component> sprite(new cSprite());
    cSprite* csprite = static_cast<cSprite*>(sprite.get());
    if((typeRange & 0x2) == 0)
    {
        if((typeRange & 0x1) != 0)
        {
            csprite->sprite.setTexture(context.resourceManager->getTexture(Textures::SUPER_BALLOON_0));

            std::unique_ptr<Component> animated(new cAnimated());
            cAnimated* canimated = static_cast<cAnimated*>(animated.get());
            canimated->textures.push_back(&context.resourceManager->getTexture(Textures::SUPER_BALLOON_0));
            canimated->textures.push_back(&context.resourceManager->getTexture(Textures::SUPER_BALLOON_1));
            canimated->textures.push_back(&context.resourceManager->getTexture(Textures::SUPER_BALLOON_2));
            balloon->addComponent(std::type_index(typeid(cAnimated)), std::move(animated));
        }
        else
        {
            csprite->sprite.setTexture(context.resourceManager->getTexture(Textures::BALLOON_0));

            std::unique_ptr<Component> animated(new cAnimated());
            cAnimated* canimated = static_cast<cAnimated*>(animated.get());
            canimated->textures.push_back(&context.resourceManager->getTexture(Textures::BALLOON_0));
            canimated->textures.push_back(&context.resourceManager->getTexture(Textures::BALLOON_1));
            canimated->textures.push_back(&context.resourceManager->getTexture(Textures::BALLOON_2));
            balloon->addComponent(std::type_index(typeid(cAnimated)), std::move(animated));
        }
    }
    else
    {
        if((typeRange & 0x1) != 0)
        {
            csprite->sprite.setTexture(context.resourceManager->getTexture(Textures::C_SUPER_BALLOON_0));

            std::unique_ptr<Component> animated(new cAnimated());
            cAnimated* canimated = static_cast<cAnimated*>(animated.get());
            canimated->textures.push_back(&context.resourceManager->getTexture(Textures::C_SUPER_BALLOON_0));
            canimated->textures.push_back(&context.resourceManager->getTexture(Textures::C_SUPER_BALLOON_1));
            canimated->textures.push_back(&context.resourceManager->getTexture(Textures::C_SUPER_BALLOON_2));
            balloon->addComponent(std::type_index(typeid(cAnimated)), std::move(animated));
        }
        else
        {
            csprite->sprite.setTexture(context.resourceManager->getTexture(Textures::C_BALLOON_0));

            std::unique_ptr<Component> animated(new cAnimated());
            cAnimated* canimated = static_cast<cAnimated*>(animated.get());
            canimated->textures.push_back(&context.resourceManager->getTexture(Textures::C_BALLOON_0));
            canimated->textures.push_back(&context.resourceManager->getTexture(Textures::C_BALLOON_1));
            canimated->textures.push_back(&context.resourceManager->getTexture(Textures::C_BALLOON_2));
            balloon->addComponent(std::type_index(typeid(cAnimated)), std::move(animated));
        }
    }
    balloon->addComponent(std::type_index(typeid(cSprite)), std::move(sprite));

    balloon->addComponent(std::type_index(typeid(cBalloon)), std::unique_ptr<Component>(new cBalloon()));

    // trigger explosion on balloon removal
    context.ecEngine->registerRemoveCall(balloon->getID(), [context] () {
        context.sfxContext->happened[SoundContext::SPLOSION] = true;
    });

    context.ecEngine->addEntity(std::move(balloon));

    return balloonInfo;
}

int Utility::clientCreateExplosion(sf::Uint8 xy, sf::Uint8 direction, Context context)
{
    std::unique_ptr<Entity> explosion(new Entity());

    std::unique_ptr<Component> pos(new cPosition());
    cPosition* cpos = static_cast<cPosition*>(pos.get());
    cpos->x = (xy % GRID_WIDTH) * GRID_SQUARE_SIZE + GRID_OFFSET_X;
    cpos->y = (xy / GRID_WIDTH) * GRID_SQUARE_SIZE + GRID_OFFSET_Y;
    explosion->addComponent(std::type_index(typeid(cPosition)), std::move(pos));

    std::unique_ptr<Component> sprite(new cSprite());
    cSprite* csprite = static_cast<cSprite*>(sprite.get());
    switch(direction)
    {
    case 0:
        csprite->sprite.setTexture(context.resourceManager->getTexture(Textures::SPLOSION_HORIZ));
        break;
    case 1:
        csprite->sprite.setTexture(context.resourceManager->getTexture(Textures::SPLOSION_VERT));
        break;
    default:
    case 2:
        csprite->sprite.setTexture(context.resourceManager->getTexture(Textures::SPLOSION_PLUS));
        break;
    }
    explosion->addComponent(std::type_index(typeid(cSprite)), std::move(sprite));

    int EID = explosion->getID();

    context.ecEngine->addEntity(std::move(explosion));

    return EID;
}

int Utility::clientCreatePowerup(sf::Uint8 xy, sf::Uint8 type, Context context)
{
    std::unique_ptr<Entity> powerup(new Entity());

    std::unique_ptr<Component> pos(new cPosition());
    cPosition* cpos = static_cast<cPosition*>(pos.get());
    cpos->x = (xy % GRID_WIDTH) * GRID_SQUARE_SIZE + GRID_OFFSET_X;
    cpos->y = (xy / GRID_WIDTH) * GRID_SQUARE_SIZE + GRID_OFFSET_Y;
    powerup->addComponent(std::type_index(typeid(cPosition)), std::move(pos));

    std::unique_ptr<Component> sprite(new cSprite());
    std::unique_ptr<Component> animated(new cAnimated());
    cSprite* csprite = static_cast<cSprite*>(sprite.get());
    cAnimated* canimated = static_cast<cAnimated*>(animated.get());

    canimated->frameTime = 0.3F;
    switch(type)
    {
    case cPowerup::BALLOON_UP:
        csprite->sprite.setTexture(context.resourceManager->getTexture(Textures::BALLOON_UP_0));
        canimated->textures.push_back(&context.resourceManager->getTexture(Textures::BALLOON_UP_0));
        canimated->textures.push_back(&context.resourceManager->getTexture(Textures::BALLOON_UP_1));
        canimated->textures.push_back(&context.resourceManager->getTexture(Textures::BALLOON_UP_2));
        break;
    case cPowerup::RANGE_UP:
        csprite->sprite.setTexture(context.resourceManager->getTexture(Textures::RANGE_UP_0));
        canimated->textures.push_back(&context.resourceManager->getTexture(Textures::RANGE_UP_0));
        canimated->textures.push_back(&context.resourceManager->getTexture(Textures::RANGE_UP_1));
        canimated->textures.push_back(&context.resourceManager->getTexture(Textures::RANGE_UP_2));
        break;
    case cPowerup::SPEED_UP:
        csprite->sprite.setTexture(context.resourceManager->getTexture(Textures::SPEED_UP_0));
        canimated->textures.push_back(&context.resourceManager->getTexture(Textures::SPEED_UP_0));
        canimated->textures.push_back(&context.resourceManager->getTexture(Textures::SPEED_UP_1));
        canimated->textures.push_back(&context.resourceManager->getTexture(Textures::SPEED_UP_2));
        break;
    case cPowerup::KICK_UPGRADE:
        csprite->sprite.setTexture(context.resourceManager->getTexture(Textures::KICK_UPGRADE_0));
        canimated->textures.push_back(&context.resourceManager->getTexture(Textures::KICK_UPGRADE_0));
        canimated->textures.push_back(&context.resourceManager->getTexture(Textures::KICK_UPGRADE_1));
        canimated->textures.push_back(&context.resourceManager->getTexture(Textures::KICK_UPGRADE_2));
        break;
    case cPowerup::RCONTROL_UPGRADE:
        csprite->sprite.setTexture(context.resourceManager->getTexture(Textures::RCONTROL_UPGRADE_0));
        canimated->textures.push_back(&context.resourceManager->getTexture(Textures::RCONTROL_UPGRADE_0));
        canimated->textures.push_back(&context.resourceManager->getTexture(Textures::RCONTROL_UPGRADE_1));
        canimated->textures.push_back(&context.resourceManager->getTexture(Textures::RCONTROL_UPGRADE_2));
        break;
    case cPowerup::SBALLOON_UPGRADE:
        csprite->sprite.setTexture(context.resourceManager->getTexture(Textures::SBALLOON_UPGRADE_0));
        canimated->textures.push_back(&context.resourceManager->getTexture(Textures::SBALLOON_UPGRADE_0));
        canimated->textures.push_back(&context.resourceManager->getTexture(Textures::SBALLOON_UPGRADE_1));
        canimated->textures.push_back(&context.resourceManager->getTexture(Textures::SBALLOON_UPGRADE_2));
        break;
    case cPowerup::PIERCE_UPGRADE:
        csprite->sprite.setTexture(context.resourceManager->getTexture(Textures::PIERCE_UPGRADE_0));
        canimated->textures.push_back(&context.resourceManager->getTexture(Textures::PIERCE_UPGRADE_0));
        canimated->textures.push_back(&context.resourceManager->getTexture(Textures::PIERCE_UPGRADE_1));
        canimated->textures.push_back(&context.resourceManager->getTexture(Textures::PIERCE_UPGRADE_2));
        break;
    case cPowerup::SPREAD_UPGRADE:
        csprite->sprite.setTexture(context.resourceManager->getTexture(Textures::SPREAD_UPGRADE_0));
        canimated->textures.push_back(&context.resourceManager->getTexture(Textures::SPREAD_UPGRADE_0));
        canimated->textures.push_back(&context.resourceManager->getTexture(Textures::SPREAD_UPGRADE_1));
        canimated->textures.push_back(&context.resourceManager->getTexture(Textures::SPREAD_UPGRADE_2));
        break;
    case cPowerup::GHOST_UPGRADE:
        csprite->sprite.setTexture(context.resourceManager->getTexture(Textures::GHOST_UPGRADE_0));
        canimated->textures.push_back(&context.resourceManager->getTexture(Textures::GHOST_UPGRADE_0));
        canimated->textures.push_back(&context.resourceManager->getTexture(Textures::GHOST_UPGRADE_1));
        canimated->textures.push_back(&context.resourceManager->getTexture(Textures::GHOST_UPGRADE_2));
        break;
    default:
        break;
    }
    powerup->addComponent(std::type_index(typeid(cSprite)), std::move(sprite));
    powerup->addComponent(std::type_index(typeid(cAnimated)), std::move(animated));

    int EID = powerup->getID();
    context.ecEngine->addEntity(std::move(powerup));
    return EID;
}

int Utility::clientCreateBreakable(sf::Uint8 xy, Context context)
{
    std::unique_ptr<Entity> breakable(new Entity());

    std::unique_ptr<Component> pos(new cPosition());
    cPosition* cpos = static_cast<cPosition*>(pos.get());
    cpos->x = (xy % GRID_WIDTH) * GRID_SQUARE_SIZE + GRID_OFFSET_X;
    cpos->y = (xy / GRID_WIDTH) * GRID_SQUARE_SIZE + GRID_OFFSET_Y;
    breakable->addComponent(std::type_index(typeid(cPosition)), std::move(pos));

    std::unique_ptr<Component> sprite(new cSprite());
    cSprite* csprite = static_cast<cSprite*>(sprite.get());
    csprite->sprite.setTexture(context.resourceManager->getTexture(Textures::BREAKABLE));
    breakable->addComponent(std::type_index(typeid(cSprite)), std::move(sprite));

    breakable->addComponent(std::type_index(typeid(cBreakable)), std::unique_ptr<Component>(new cBreakable()));

    int EID = breakable->getID();
    context.ecEngine->addEntity(std::move(breakable));

    context.ecEngine->registerRemoveCall(EID, [context] () {
        context.sfxContext->happened[SoundContext::BREAKABLE_BROKEN] = true;
    });

    return EID;
}

void Utility::centerTextOrigin(sf::Text& text)
{
    sf::FloatRect rect = text.getLocalBounds();
    text.setOrigin(rect.width / 2.0F, rect.height / 2.0F);
}

bool Utility::livingInDanger(const float& x, const float& y, PathFinder& pf, Context context)
{
    unsigned char postGrid[GRID_TOTAL];
    const unsigned char* grid = pf.getValidGrid(*context.ecEngine);

    // get copy of grid
    for(int i = 0; i < GRID_TOTAL; ++i)
    {
        postGrid[i] = grid[i];
    }

    // populate copy with WIndicators
    for(auto iter = context.ecEngine->getEntityIterBegin(); iter != context.ecEngine->getEntityIterEnd(); ++iter)
    {
        if(iter->second->hasComponent(std::type_index(typeid(cBalloon))) && iter->second->hasComponent(std::type_index(typeid(cPosition))))
        {
            cBalloon* balloon = static_cast<cBalloon*>(iter->second->getComponent(std::type_index(typeid(cBalloon))));
            cPosition* pos = static_cast<cPosition*>(iter->second->getComponent(std::type_index(typeid(cPosition))));

            unsigned char xy = (unsigned char)((pos->x - (float)GRID_OFFSET_X) / GRID_SQUARE_SIZE) + ((unsigned char)((pos->y - (float)GRID_OFFSET_Y) / GRID_SQUARE_SIZE) * GRID_WIDTH);

            // right
            for(int i = xy + 1; i <= xy + balloon->range; ++i)
            {
                if((i - 1) % GRID_WIDTH != GRID_WIDTH - 1 && ((postGrid[i] & 0x10) == 0x0 || balloon->ghosting))
                {
                    postGrid[i] |= 0x20;
                    if((postGrid[i] & 0x4) != 0x0 && !balloon->piercing)
                        break;
                }
                else
                    break;
            }
            // left
            for(int i = xy - 1; i >= xy - balloon->range; --i)
            {
                if((i + 1) % GRID_WIDTH != 0 && ((postGrid[i] & 0x10) == 0x0 || balloon->ghosting))
                {
                    postGrid[i] |= 0x20;
                    if((postGrid[i] & 0x4) != 0x0 && !balloon->piercing)
                        break;
                }
                else
                    break;
            }
            // up
            for(int i = xy - GRID_WIDTH; i >= (int)xy - (int)(GRID_WIDTH * balloon->range) ; i -= GRID_WIDTH)
            {
                if(i >= 0 && ((postGrid[i] & 0x10) == 0x0 || balloon->ghosting))
                {
                    postGrid[i] |= 0x20;
                    if((postGrid[i] & 0x4) != 0x0 && !balloon->piercing)
                        break;
                }
                else
                    break;
            }
            // down
            for(int i = xy + GRID_WIDTH; i <= (int)xy + (int)(GRID_WIDTH * balloon->range); i += GRID_WIDTH)
            {
                if(i < GRID_TOTAL && ((postGrid[i] & 0x10) == 0x0 || balloon->ghosting))
                {
                    postGrid[i] |= 0x20;
                    if((postGrid[i] & 0x4) != 0x0 && !balloon->piercing)
                        break;
                }
                else
                    break;
            }
        }
    }

    /*
    // get approximate grid coordinate of living entity
    int xy = (int)((x - (float)GRID_OFFSET_X) / GRID_SQUARE_SIZE + 0.5F) + ((int)((y - (float)GRID_OFFSET_Y) / GRID_SQUARE_SIZE + 0.5F) * GRID_WIDTH);

    // check 9 valid grid cells at/around xy for collision
    for(int i = -1; i <= 1; ++i)
    {
        if(xy % GRID_WIDTH == 0 && i == -1)
            continue;
        else if(xy % GRID_WIDTH == GRID_WIDTH - 1 && i == 1)
            break;
        for(int j = -GRID_WIDTH; j <= GRID_WIDTH; j += GRID_WIDTH)
        {
            if(xy / GRID_WIDTH == 0 && j == -GRID_WIDTH)
                continue;
            else if(xy / GRID_WIDTH == GRID_HEIGHT - 1)
                break;
            // convert valid coordinate to global coordinates to check against global coordinate living entity
            float wx = ((xy + i + j) % GRID_WIDTH) * GRID_SQUARE_SIZE + GRID_OFFSET_X;
            float wy = ((xy + i + j) / GRID_WIDTH) * GRID_SQUARE_SIZE + GRID_OFFSET_Y;
            if(collide(x, y, wx, wy))
                return true;
        }
    }

    */

    int xCoord0 = (int)((x - (float)GRID_OFFSET_X) / GRID_SQUARE_SIZE + 0.5F);
    int yCoord0 = (int)((y - (float)GRID_OFFSET_Y) / GRID_SQUARE_SIZE + 0.5F);
    int xCoord1;
    int yCoord1;
    float alignedX = xCoord0 * GRID_SQUARE_SIZE + GRID_OFFSET_X;
    float alignedY = yCoord0 * GRID_SQUARE_SIZE + GRID_OFFSET_Y;

    if(alignedX > x)
        xCoord1 = xCoord0 - 1;
    else if(alignedX < x)
        xCoord1 = xCoord0 + 1;
    else
        xCoord1 = xCoord0;
    if(alignedY > y)
        yCoord1 = yCoord0 - 1;
    else if(alignedY < y)
        yCoord1 = yCoord0 + 1;
    else
        yCoord1 = yCoord0;

    int xy0 = xCoord0 + yCoord0 * GRID_WIDTH;
    int xy1 = xCoord1 + yCoord1 * GRID_WIDTH;

    // check xy0 and xy1
    if(xy0 >= 0 && xy0 < GRID_TOTAL && (postGrid[xy0] & 0x20) != 0)
        return true;
    else if(xy1 >= 0 && xy1 < GRID_TOTAL && (postGrid[xy1] & 0x20) != 0)
        return true;

    return false;
}
