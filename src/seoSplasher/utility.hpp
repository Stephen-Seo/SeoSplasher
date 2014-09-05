
#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <list>
#include <typeindex>

#include <SFML/System.hpp>

#include "direction.hpp"

class Entity;
class Engine;
struct cLiving;
struct Context;
struct cBalloon;
struct cDamage;
struct cPowerup;

struct HitInfo
{
    std::list<Entity*> hit;
};

namespace Utility
{
    HitInfo collideAll(const float& x, const float& y, Engine& engine);
    bool collidesAll(const float& x, const float& y, Engine& engine);
    HitInfo collideAgainstComponent(const float& x, const float& y, const std::type_index& type, Engine& engine);
    HitInfo collideAgainstComponentList(const float& x, const float& y, const std::list<std::type_index>& types, Engine& engine);
    bool collidesAgainstComponent(const float& x, const float& y, const std::type_index& type, Engine& engine);
    bool collide(const float& xOne, const float& yOne, const float& xTwo, const float& yTwo);

    void createBalloon(const float& x, const float& y, cLiving& living, const Context& context, unsigned char ID, bool* fired);
    void createExplosion(const float& x, const float& y, cBalloon& balloon, const Context& context, bool horizontal, bool vertical);
    void createExplosion(const float& x, const float& y, cDamage& damage, const Context& context, bool horizontal, bool vertical);
    void createPowerup(const float& x, const float& y, cPowerup& powerup, const Context& context);
    int createWIndicator(const float& x, const float& y, Direction::Direction dir, const Context& context);

    sf::Vector2f alignToGrid(const float& x, const float& y);
};

#endif
