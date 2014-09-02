
#include "nPControl.hpp"

#include <typeindex>

#include "../context.hpp"
#include "cPlayerControl.hpp"
#include "../ec/cPosition.hpp"
#include "../ec/cVelocity.hpp"
#include "cLiving.hpp"
#include "utility.hpp"

nPControl::nPControl() :
control(nullptr),
pos(nullptr),
vel(nullptr),
living(nullptr)
{}

bool nPControl::checkEntity(Entity& entity)
{
    return entity.hasComponent(std::type_index(typeid(cPlayerControl))) &&
           entity.hasComponent(std::type_index(typeid(cPosition))) &&
           entity.hasComponent(std::type_index(typeid(cVelocity))) &&
           entity.hasComponent(std::type_index(typeid(cLiving)));
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

    entityRemoved = &entity.removed;
}

void nPControl::update(sf::Time dt, Context context)
{
    if(*entityRemoved)
        return;

    float speed = DEFAULT_SPEED + SPEED_UP_MULT * (float)living->speedUp;
    switch(*control->dir)
    {
    case cPlayerControl::RIGHT:
        vel->x = speed;
        vel->y = 0.0f;
        break;
    case cPlayerControl::LEFT:
        vel->x = -speed;
        vel->y = 0.0f;
        break;
    case cPlayerControl::UP:
        vel->x = 0.0f;
        vel->y = -speed;
        break;
    case cPlayerControl::DOWN:
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
        Utility::createBalloon(pos->x, pos->y, *living, context, control->ID);
    }
}
