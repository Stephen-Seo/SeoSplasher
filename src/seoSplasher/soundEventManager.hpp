
#ifndef SOUND_EVENT_MANAGER_HPP
#define SOUND_EVENT_MANAGER_HPP

#define SOUND_CHECK_TIME 1.0F

#include <SFML/System.hpp>
#include "../context.hpp"

class SoundEventManager
{
public:
    SoundEventManager();

    void update(sf::Time dt, Context context);

private:
    float soundCheckTimer;

};

#endif
