
#include "soundEventManager.hpp"

#include "../resourceIdentifiers.hpp"
#include "../resourceManager.hpp"
#include "soundContext.hpp"
#include "../soundPlayer.hpp"

void SoundEventManager::update(sf::Time dt, Context context)
{
    if(context.sfxContext->mute)
        return;

    soundCheckTimer -= dt.asSeconds();
    if(soundCheckTimer <= 0.0f)
    {
        soundCheckTimer = SOUND_CHECK_TIME;
        context.sPlayer->removeStoppedSounds();
    }

    for(int i = 0; i < SoundContext::SIZE; ++i)
    {
        if(context.sfxContext->happened[i])
        {
            switch(i)
            {
            case SoundContext::BREAKABLE_BROKEN:
                context.sPlayer->play(context.resourceManager->getSoundBuffer(Sound::BREAKABLE));
                break;
            case SoundContext::COUNTDOWN_COUNTED:
                context.sPlayer->play(context.resourceManager->getSoundBuffer(Sound::COUNTDOWN_0));
                break;
            case SoundContext::COUNTDOWN_ENDED:
                context.sPlayer->play(context.resourceManager->getSoundBuffer(Sound::COUNTDOWN_1));
                break;
            case SoundContext::COMBATANT_DIED:
                context.sPlayer->play(context.resourceManager->getSoundBuffer(Sound::DEATH));
                break;
            case SoundContext::BALLOON_KICKED:
                context.sPlayer->play(context.resourceManager->getSoundBuffer(Sound::KICK));
                break;
            case SoundContext::SPLOSION:
                context.sPlayer->play(context.resourceManager->getSoundBuffer(Sound::SPLOSION));
                break;
            case SoundContext::PICKUP_GET:
                context.sPlayer->play(context.resourceManager->getSoundBuffer(Sound::POWERUP));
                break;
            case SoundContext::GAME_ENDED_BADLY:
                context.sPlayer->play(context.resourceManager->getSoundBuffer(Sound::TRY_AGAIN_TUNE));
                break;
            case SoundContext::GAME_ENDED_WELL:
                context.sPlayer->play(context.resourceManager->getSoundBuffer(Sound::VICTORY_TUNE));
                break;
            default:
                break;
            }
            context.sfxContext->happened[i] = false;
        }
    }
}
