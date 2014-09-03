
#ifndef UAI_HPP
#define UAI_HPP

struct cPosition;
struct cLiving;
struct Context;

class UAI
{
public:
    enum Action {
        PLACE_BALLOON,
        GET_POWERUP,
        MOVE_TO_ENEMY,
        MOVE_TO_BREAKABLE,
        MOVE_TO_SAFETY,
        KICK_BALLOON,
        PANIC
    };

    Action determineAction(const cPosition& pos, const cLiving& living, Context context);
private:

};

#endif
