
#include "soundContext.hpp"

SoundContext::SoundContext() :
mute(false)
{
    reset();
}

void SoundContext::reset()
{
    for(int i = 0; i < SIZE; ++i)
        happened[i] = false;
}
