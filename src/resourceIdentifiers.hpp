
#ifndef RESOURCE_IDENTIFIERS_HPP
#define RESOURCE_IDENTIFIERS_HPP

#include <cstdlib>
#include <set>

namespace sf
{
    class Texture;
    class Font;
    class SoundBuffer;
}

namespace Textures
{
    enum ID
    {
        WALL,
        BREAKABLE,
        BALLOON_0,
        BALLOON_1,
        BALLOON_2,
        SUPER_BALLOON_0,
        SUPER_BALLOON_1,
        SUPER_BALLOON_2,
        C_BALLOON_0,
        C_BALLOON_1,
        C_BALLOON_2,
        C_SUPER_BALLOON_0,
        C_SUPER_BALLOON_1,
        C_SUPER_BALLOON_2,
        PLAYER_ONE,
        PLAYER_TWO,
        PLAYER_THREE,
        PLAYER_FOUR,
        C_PLAYER_ONE,
        C_PLAYER_TWO,
        C_PLAYER_THREE,
        C_PLAYER_FOUR,
        SPLOSION_PLUS,
        SPLOSION_VERT,
        SPLOSION_HORIZ,
        BALLOON_UP_0,
        BALLOON_UP_1,
        BALLOON_UP_2,
        RANGE_UP_0,
        RANGE_UP_1,
        RANGE_UP_2,
        SPEED_UP_0,
        SPEED_UP_1,
        SPEED_UP_2,
        KICK_UPGRADE_0,
        KICK_UPGRADE_1,
        KICK_UPGRADE_2,
        RCONTROL_UPGRADE_0,
        RCONTROL_UPGRADE_1,
        RCONTROL_UPGRADE_2,
        SBALLOON_UPGRADE_0,
        SBALLOON_UPGRADE_1,
        SBALLOON_UPGRADE_2,
        PIERCE_UPGRADE_0,
        PIERCE_UPGRADE_1,
        PIERCE_UPGRADE_2,
        SPREAD_UPGRADE_0,
        SPREAD_UPGRADE_1,
        SPREAD_UPGRADE_2,
        GHOST_UPGRADE_0,
        GHOST_UPGRADE_1,
        GHOST_UPGRADE_2
    };
}

namespace Fonts
{
    enum ID
    {
    };
}

namespace Music
{
    enum ID
    {
    };
}

namespace Sound
{
    enum ID
    {
    };
}

namespace GameResources
{
    enum LoadingMode
    {
        DEFAULT,
        PACKFILE
    };
}

template<class Resource, class Identifier>
class ResourceHolder;

typedef ResourceHolder<sf::Texture, Textures::ID> TextureHolder;
typedef ResourceHolder<sf::Font, Fonts::ID> FontHolder;
typedef ResourceHolder<sf::SoundBuffer, Sound::ID> SoundBufferHolder;

typedef std::set<Textures::ID> TextureSet;
typedef std::set<Fonts::ID> FontSet;
typedef std::set<Sound::ID> SoundSet;

struct ResourcesSet
{
    ResourcesSet();

    ResourcesSet(TextureSet& tset, FontSet& fset, SoundSet& sset);

    TextureSet* tset;
    FontSet* fset;
    SoundSet* sset;
};
#endif
