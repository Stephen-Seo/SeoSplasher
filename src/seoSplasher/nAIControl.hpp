
#ifndef N_AI_CONTROL_HPP
#define N_AI_CONTROL_HPP

#define TICK_TO_ALIGN 30
#define RCONTROL_TIME 1.5f

#include "../ec/node.hpp"

#include <map>

#include "AI/uai.hpp"

struct cAIControl;
struct cLiving;
struct cPosition;
struct cVelocity;
struct cPathFinderRef;

struct nAIControl : Node
{
    nAIControl();

    cAIControl* control;
    cLiving* living;
    cPosition* pos;
    cVelocity* vel;
    cPathFinderRef* pfref;

    PAMapping pam;

    std::map<int, int> rpaths;

    bool alignAction;

    int tick;

    float controlFireTime;

    sf::Vector2f prevPos;

    bool checkEntity(Entity& entity);
    std::unique_ptr<Node> getNewNode();
    void getCReferencesFromEntity(Entity& entity);
    void update(sf::Time dt, Context context);
};

#endif
