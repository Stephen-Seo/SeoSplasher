
#ifndef N_MOVE_HPP
#define N_MOVE_HPP

#include <list>
#include <typeindex>

#include "../ec/node.hpp"

#include "../ec/cPosition.hpp"
#include "../ec/cVelocity.hpp"

struct nMove : Node
{
public:
    nMove();

    cPosition* pos;
    cVelocity* vel;

    bool checkEntity(Entity& entity);
    std::unique_ptr<Node> getNewNode();
    void getCReferencesFromEntity(Entity& entity);
    void update(sf::Time dt, Context context);
private:
    static std::list<std::type_index> colList;
};

#endif
