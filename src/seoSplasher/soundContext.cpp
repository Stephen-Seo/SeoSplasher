
#include "soundContext.hpp"

SoundContext::SoundContext()
{
    reset();
}

void SoundContext::reset()
{
    for(int i = 0; i < SIZE; ++i)
        happened[i] = false;
}
