
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

    bool checkEntity(Entity& entity) override;
    std::unique_ptr<Node> getNewNode() override;
    void getCReferencesFromEntity(Entity& entity) override;
    void update(sf::Time dt, Context context) override;
};

#endif
