
#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <list>
#include <typeindex>

class Entity;
class Engine;

struct HitInfo
{
    std::list<Entity*> hit;
};

namespace Utility
{
    HitInfo collideAll(const float& x, const float& y, Engine& engine);
    HitInfo collideAgainstComponent(const float& x, const float& y, const std::type_index& type, Engine& engine);
    HitInfo collideAgainstComponentList(const float& x, const float& y, const std::list<std::type_index>& types, Engine& engine);
    bool collide(const float& xOne, const float& yOne, const float& xTwo, const float& yTwo);
};

#endif
