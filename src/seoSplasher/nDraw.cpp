
#include "nDraw.hpp"

#include <typeindex>

#include <SFML/Graphics.hpp>

#include "../context.hpp"

nDraw::nDraw() :
sprite(nullptr)
{}

bool nDraw::checkEntity(Entity& entity)
{
    return entity.hasComponent(std::type_index(typeid(cSprite))) &&
           entity.hasComponent(std::type_index(typeid(cPosition)));
}

std::unique_ptr<Node> nDraw::getNewNode()
{
    return std::unique_ptr<Node>(new nDraw);
}

void nDraw::getCReferencesFromEntity(Entity& entity)
{
    pos = static_cast<cPosition*>(entity.getComponent(std::type_index(typeid(cPosition))));
    sprite = static_cast<cSprite*>(entity.getComponent(std::type_index(typeid(cSprite))));
    entityRemoved = &entity.removed;
}

void nDraw::update(sf::Time dt, Context context)
{
    sf::Transform transform;
    transform.translate(pos->x, pos->y);

    context.window->draw(sprite->sprite, transform);
}
