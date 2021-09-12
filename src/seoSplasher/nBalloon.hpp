
#ifndef N_BALLOON_HPP
#define N_BALLOON_HPP

#include "../ec/node.hpp"

struct cPosition;
struct cBalloon;
struct cControl;
struct cTimer;
struct cPathFinderRef;

struct nBalloon : Node
{
public:
    nBalloon();

    cPosition* pos;
    cBalloon* balloon;
    cControl* control;
    cTimer* timer;
    cPathFinderRef* pfRef;

    bool checkEntity(Entity& entity) override;
    std::unique_ptr<Node> getNewNode() override;
    void getCReferencesFromEntity(Entity& entity) override;
    void update(sf::Time dt, Context context) override;
private:
    int ID;

    void generateExplosions(Context context);
};

#endif
