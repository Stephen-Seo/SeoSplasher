
#include "nAIControl.hpp"

#include <typeindex>

#include "cAIControl.hpp"
#include "cLiving.hpp"
#include "../ec/cPosition.hpp"
#include "../ec/cVelocity.hpp"
#include "AI/pathFinder.hpp"
#include "../context.hpp"
#include "utility.hpp"
#include "cPathFinderRef.hpp"

nAIControl::nAIControl() :
control(nullptr),
living(nullptr),
pos(nullptr),
vel(nullptr),
pfref(nullptr)
{}

bool nAIControl::checkEntity(Entity& entity)
{
    return entity.hasComponent(std::type_index(typeid(cAIControl))) &&
           entity.hasComponent(std::type_index(typeid(cLiving))) &&
           entity.hasComponent(std::type_index(typeid(cPosition))) &&
           entity.hasComponent(std::type_index(typeid(cVelocity))) &&
           entity.hasComponent(std::type_index(typeid(cPathFinderRef)));
}

std::unique_ptr<Node> nAIControl::getNewNode()
{
    return std::unique_ptr<Node>(new nAIControl);
}

void nAIControl::getCReferencesFromEntity(Entity& entity)
{
    control = static_cast<cAIControl*>(entity.getComponent(std::type_index(typeid(cAIControl))));
    living = static_cast<cLiving*>(entity.getComponent(std::type_index(typeid(cLiving))));
    pos = static_cast<cPosition*>(entity.getComponent(std::type_index(typeid(cPosition))));
    vel = static_cast<cVelocity*>(entity.getComponent(std::type_index(typeid(cVelocity))));
    pfref = static_cast<cPathFinderRef*>(entity.getComponent(std::type_index(typeid(cPathFinderRef))));
    entityRemoved = &entity.removed;
}

void nAIControl::update(sf::Time dt, Context context)
{
    if(*entityRemoved)
        return;

    control->timer -= dt.asSeconds();
    if(control->timer <= 0.0f || control->currentAction == AI::NONE)
    {
        control->timer = control->aiTickTime;
        control->currentAction = control->ai.determineAction(*pos, *living, *control->pf, *context.ecEngine, *context.rGen);
    }

    switch(control->currentAction)
    {
    case AI::PLACE_BALLOON:
        Utility::createBalloon(pos->x, pos->y, *living, context, control->ID, &control->fired, *pfref);
        break;
    case AI::GET_POWERUP:
        break;
    case AI::MOVE_TO_ENEMY:
        break;
    case AI::MOVE_TO_BREAKABLE:
        break;
    case AI::MOVE_TO_SAFETY:
        break;
    case AI::KICK_BALLOON:
        break;
    case AI::PANIC:
        break;
    default:
        break;
    }
}
