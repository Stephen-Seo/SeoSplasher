
#ifndef N_ANIMATED_HPP
#define N_ANIMATED_HPP

#include "../ec/node.hpp"

struct cAnimated;
struct cSprite;
struct cTimer;

struct nAnimated : Node
{
    nAnimated();

    cAnimated* sprites;
    cSprite* sprite;
    cTimer* timer;

    bool checkEntity(Entity& entity);
    std::unique_ptr<Node> getNewNode();
    void getCReferencesFromEntity(Entity& entity);
    void update(sf::Time dt, Context context);
};

#endif
