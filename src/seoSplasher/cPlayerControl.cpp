
#include "cPlayerControl.hpp"

cPlayerControl::cPlayerControl(Direction::Direction* dir, bool* place, bool* placeAction, bool* kick, bool* kickAction, unsigned char ID, bool* cFired) :
dir(dir),
placeBalloon(place),
placeAction(placeAction),
kick(kick),
kickAction(kickAction),
ID(ID),
cFired(cFired)
{}
