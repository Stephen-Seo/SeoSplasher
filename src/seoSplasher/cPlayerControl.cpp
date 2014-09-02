
#include "cPlayerControl.hpp"

cPlayerControl::cPlayerControl(Direction* dir, bool* place, bool* placeAction, bool* kick, bool* kickAction, unsigned char ID) :
dir(dir),
placeBalloon(place),
placeAction(placeAction),
kick(kick),
kickAction(kickAction),
ID(ID)
{}
