
#ifndef SOUND_CONTEXT_HPP
#define SOUND_CONTEXT_HPP

struct SoundContext
{
    enum Events
    {
        BREAKABLE_BROKEN,
        COUNTDOWN_COUNTED,
        COUNTDOWN_ENDED,
        COMBATANT_DIED,
        BALLOON_KICKED,
        SPLOSION,
        GAME_ENDED_BADLY,
        GAME_ENDED_WELL,
        SIZE
    };

    SoundContext();

    bool happened[SIZE];

    void reset();
};

#endif
