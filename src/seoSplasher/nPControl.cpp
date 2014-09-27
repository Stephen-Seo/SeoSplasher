
#include "nPControl.hpp"

#include "../context.hpp"
#include "cPlayerControl.hpp"
#include "../ec/cPosition.hpp"
#include "../ec/cVelocity.hpp"
#include "cLiving.hpp"
#include "utility.hpp"
#include "cPowerup.hpp"
#include "cPickup.hpp"
#include "../ec/engine.hpp"
#include "cPathFinderRef.hpp"
#include "soundContext.hpp"

std::list<std::type_index> nPControl::powerupFilter;

nPControl::nPControl() :
control(nullptr),
pos(nullptr),
vel(nullptr),
living(nullptr),
pfref(nullptr)
{
    if(nPControl::powerupFilter.empty())
    {
        nPControl::powerupFilter.push_back(std::type_index(typeid(cPowerup)));
        nPControl::powerupFilter.push_back(std::type_index(typeid(cPickup)));
    }
}

bool nPControl::checkEntity(Entity& entity)
{
    return entity.hasComponent(std::type_index(typeid(cPlayerControl))) &&
           entity.hasComponent(std::type_index(typeid(cPosition))) &&
           entity.hasComponent(std::type_index(typeid(cVelocity))) &&
           entity.hasComponent(std::type_index(typeid(cLiving))) &&
           entity.hasComponent(std::type_index(typeid(cPathFinderRef)));
}

std::unique_ptr<Node> nPControl::getNewNode()
{
    return std::unique_ptr<Node>(new nPControl);
}

void nPControl::getCReferencesFromEntity(Entity& entity)
{
    control = static_cast<cPlayerControl*>(entity.getComponent(std::type_index(typeid(cPlayerControl))));
    pos = static_cast<cPosition*>(entity.getComponent(std::type_index(typeid(cPosition))));
    vel = static_cast<cVelocity*>(entity.getComponent(std::type_index(typeid(cVelocity))));
    living = static_cast<cLiving*>(entity.getComponent(std::type_index(typeid(cLiving))));
    pfref = static_cast<cPathFinderRef*>(entity.getComponent(std::type_index(typeid(cPathFinderRef))));

    entityRemoved = &entity.removed;
}

void nPControl::update(sf::Time dt, Context context)
{
    if(*entityRemoved)
        return;

    float speed = DEFAULT_SPEED + SPEED_UP_MULT * (float)living->speedUp;
    switch(*control->dir)
    {
    case Direction::RIGHT:
        vel->x = speed;
        vel->y = 0.0f;
        break;
    case Direction::LEFT:
        vel->x = -speed;
        vel->y = 0.0f;
        break;
    case Direction::UP:
        vel->x = 0.0f;
        vel->y = -speed;
        break;
    case Direction::DOWN:
        vel->x = 0.0f;
        vel->y = speed;
        break;
    default:
        vel->x = 0.0f;
        vel->y = 0.0f;
        break;
    }

    if(*control->placeBalloon && !*control->placeAction)
    {
        *control->placeAction = true;
        Utility::createBalloon(pos->x, pos->y, *living, context, control->ID, control->cFired, *pfref);
    }

    HitInfo powerinfo = Utility::collideAgainstComponentList(pos->x, pos->y, nPControl::powerupFilter, *context.ecEngine);
    for(auto piter = powerinfo.hit.begin(); piter != powerinfo.hit.end(); ++piter)
    {
        cPickup* pickup = static_cast<cPickup*>((*piter)->getComponent(std::type_index(typeid(cPickup))));
        if(pickup->hit)
            continue;

        cPowerup* powerup = static_cast<cPowerup*>((*piter)->getComponent(std::type_index(typeid(cPowerup))));

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
        context.sfxContext->happened[SoundContext::PICKUP_GET] = true;
    }
}
