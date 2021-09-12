
#include "nCControl.hpp"

#include <typeindex>

#include "cLiving.hpp"
#include "cClientControl.hpp"
#include "utility.hpp"
#include "cPathFinderRef.hpp"
#include "../context.hpp"
#include "serverContext.hpp"
#include "../ec/cPosition.hpp"
#include "../ec/cVelocity.hpp"
#include "cPowerup.hpp"
#include "cPickup.hpp"

nCControl::nCControl() :
pos(nullptr),
vel(nullptr),
living(nullptr),
pfRef(nullptr)
{
}

bool nCControl::checkEntity(Entity& entity)
{
    return entity.hasComponent(std::type_index(typeid(cLiving))) && entity.hasComponent(std::type_index(typeid(cClientControl))) && entity.hasComponent(std::type_index(typeid(cPathFinderRef))) && entity.hasComponent(std::type_index(typeid(cPosition))) && entity.hasComponent(std::type_index(typeid(cVelocity)));
}

std::unique_ptr<Node> nCControl::getNewNode()
{
    return std::unique_ptr<Node>(new nCControl());
}

void nCControl::getCReferencesFromEntity(Entity& entity)
{
    pos = static_cast<cPosition*>(entity.getComponent(std::type_index(typeid(cPosition))));
    vel = static_cast<cVelocity*>(entity.getComponent(std::type_index(typeid(cVelocity))));
    living = static_cast<cLiving*>(entity.getComponent(std::type_index(typeid(cLiving))));
    pfRef = static_cast<cPathFinderRef*>(entity.getComponent(std::type_index(typeid(cPathFinderRef))));
    entityRemoved = &entity.removed;
}

void nCControl::update(sf::Time /*dt*/, Context context)
{
    if(*entityRemoved)
        return;

    float speed = DEFAULT_SPEED + SPEED_UP_MULT * (float)living->speedUp;
    if((context.scontext->input[living->ID] & 0x1) != 0x0)
    {
        // Right
        vel->x = speed;
        vel->y = 0.0F;
    }
    else if((context.scontext->input[living->ID] & 0x2) != 0x0)
    {
        // Up
        vel->x = 0.0F;
        vel->y = -speed;
    }
    else if((context.scontext->input[living->ID] & 0x4) != 0x0)
    {
        // Down
        vel->x = 0.0F;
        vel->y = speed;
    }
    else if((context.scontext->input[living->ID] & 0x8) != 0x0)
    {
        // Left
        vel->x = -speed;
        vel->y = 0.0F;
    }
    else
    {
        vel->x = 0.0F;
        vel->y = 0.0F;
    }

    if((context.scontext->input[living->ID] & 0x20) != 0x0)
        remoteFired = true;
    else
        remoteFired = false;

    if((context.scontext->input[living->ID] & 0x10) != 0x0)
    {
        context.scontext->input[living->ID] &= 0xEF;
        Utility::createBalloon(pos->x, pos->y, *living, context, living->ID, &remoteFired, *pfRef);
    }

    HitInfo powerinfo = Utility::collideAgainstComponent(pos->x, pos->y, std::type_index(typeid(cPickup)), *context.ecEngine);
    for(auto & piter : powerinfo.hit)
    {
        cPickup* pickup = static_cast<cPickup*>(piter->getComponent(std::type_index(typeid(cPickup))));
        if(pickup->hit)
            continue;

        cPowerup* powerup = static_cast<cPowerup*>(piter->getComponent(std::type_index(typeid(cPowerup))));

        switch(powerup->powerup)
        {
        case cPowerup::BALLOON_UP:
            ++(living->balloonUp);
            break;
        case cPowerup::RANGE_UP:
            ++(living->rangeUp);
            break;
        case cPowerup::SPEED_UP:
            ++(living->speedUp);
            break;
        case cPowerup::KICK_UPGRADE:
            ++(living->kickUpgrade);
            break;
        case cPowerup::RCONTROL_UPGRADE:
            ++(living->rControlUpgrade);
            break;
        case cPowerup::SBALLOON_UPGRADE:
            ++(living->sBalloonUpgrade);
            break;
        case cPowerup::PIERCE_UPGRADE:
            ++(living->pierceUpgrade);
            break;
        case cPowerup::SPREAD_UPGRADE:
            ++(living->spreadUpgrade);
            break;
        case cPowerup::GHOST_UPGRADE:
            ++(living->ghostUpgrade);
            break;
        default:
            break;
        }

        pickup->hit = true;
        context.scontext->powerupPickedup[living->ID] = true;
    }
}
