
#ifndef N_PCONTROL_HPP
#define N_PCONTROL_HPP

#include "../ec/node.hpp"

#include <list>
#include <typeindex>

struct cPlayerControl;
struct cPosition;
struct cVelocity;
struct cLiving;
struct cPathFinderRef;

struct nPControl : Node
{
public:
    nPControl();

    cPlayerControl* control;
    cPosition* pos;
    cVelocity* vel;
    cLiving* living;
    cPathFinderRef* pfref;

    bool checkEntity(Entity& entity);
    std::unique_ptr<Node> getNewNode();
    void getCReferencesFromEntity(Entity& entity);
    void update(sf::Time dt, Context context);
private:
    static std::list<std::type_index> powerupFilter;
};

#endif
