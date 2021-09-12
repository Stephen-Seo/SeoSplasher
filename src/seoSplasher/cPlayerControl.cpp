
#include "cPlayerControl.hpp"

cPlayerControl::cPlayerControl(Direction::Direction* dir, bool* place, bool* placeAction, bool* kick, bool* kickAction, unsigned char ID, bool* cFired) :
dir(dir),
ID(ID),
placeBalloon(place),
placeAction(placeAction),
kick(kick),
kickAction(kickAction),
cFired(cFired)
{}
