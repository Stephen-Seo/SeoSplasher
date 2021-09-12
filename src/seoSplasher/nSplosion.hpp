
#ifndef N_SPLOSION_HPP
#define N_SPLOSION_HPP

#include "../ec/node.hpp"

struct cPosition;
struct cDamage;
struct cTimer;

struct nSplosion : Node
{
public:
    nSplosion();

    cPosition* pos;
    cDamage* damage;
    cTimer* timer;

    bool checkEntity(Entity& entity) override;
    std::unique_ptr<Node> getNewNode() override;
    void getCReferencesFromEntity(Entity& entity) override;
    void update(sf::Time dt, Context context) override;
private:
    int ID;
};

#endif
