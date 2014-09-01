
#ifndef N_DRAW_HPP
#define N_DRAW_HPP

#include "../ec/node.hpp"

#include "../ec/cPosition.hpp"
#include "cSprite.hpp"

struct nDraw : Node
{
    nDraw();

    cPosition* pos;
    cSprite* sprite;

    bool checkEntity(Entity& entity);
    std::unique_ptr<Node> getNewNode();
    void getCReferencesFromEntity(Entity& entity);
    void update(sf::Time dt, Context context);
};

#endif
