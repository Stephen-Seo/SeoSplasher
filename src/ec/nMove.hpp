
#ifndef NODE_MOVE_HPP
#define NODE_MOVE_HPP

#include "node.hpp"
#include "cPosition.hpp"
#include "cVelocity.hpp"
#include "cAcceleration.hpp"
#include "entity.hpp"

struct nMove : Node
{
    nMove();

    cPosition* position;
    cVelocity* velocity;
    cAcceleration* acceleration;

    bool checkEntity(Entity& entity) override;
    std::unique_ptr<Node> getNewNode() override;
    void getCReferencesFromEntity(Entity& entity) override;
    void update(sf::Time dt, Context context) override;
};

#endif
