
#include "nMove.hpp"

#include "../context.hpp"
#include "gridInfo.hpp"
#include "utility.hpp"
#include "cBreakable.hpp"
#include "cWall.hpp"

std::list<std::type_index> nMove::colList;

nMove::nMove() :
pos(nullptr),
vel(nullptr)
{}

bool nMove::checkEntity(Entity& entity)
{
    return entity.hasComponent(std::type_index(typeid(cPosition))) &&
           entity.hasComponent(std::type_index(typeid(cVelocity)));
}

std::unique_ptr<Node> nMove::getNewNode()
{
    return std::unique_ptr<Node>(new nMove);
}

void nMove::getCReferencesFromEntity(Entity& entity)
{
    pos = static_cast<cPosition*>(entity.getComponent(std::type_index(typeid(cPosition))));
    vel = static_cast<cVelocity*>(entity.getComponent(std::type_index(typeid(cVelocity))));
    entityRemoved = &entity.removed;
}

void nMove::update(sf::Time dt, Context context)
{
    if(nMove::colList.empty())
    {
        nMove::colList.push_back(std::type_index(typeid(cBreakable)));
        nMove::colList.push_back(std::type_index(typeid(cWall)));
    }

    float prev = pos->x;
    pos->x += vel->x * dt.asSeconds();

    HitInfo info = Utility::collideAgainstComponentList(pos->x, pos->y, nMove::colList, *context.ecEngine);

    if(!info.hit.empty())
    {
        pos->x = prev;
    }

    prev = pos->y;
    pos->y += vel->y * dt.asSeconds();

    info = Utility::collideAgainstComponentList(pos->x, pos->y, nMove::colList, *context.ecEngine);

    if(!info.hit.empty())
    {
        pos->y = prev;
    }
}
