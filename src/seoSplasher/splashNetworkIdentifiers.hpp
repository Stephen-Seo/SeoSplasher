
#ifndef SPLASH_NETWORK_IDENTIFIERS_HPP
#define SPLASH_NETWORK_IDENTIFIERS_HPP

/*
PACKET INFORMATION:
    Two packet 'types'
        - waiting
        - in-game

Client waiting packet format:
    1 byte boolean has custom name
    1-? bytes custom name

Client in-game packet format:
    4 bytes posx
    4 bytes posy
    4 bytes velx
    4 bytes vely
    1 byte boolean placing balloon

Server waiting packet format:
    1 byte State Identifier (One of "GameState" enum)
    1 byte # of connected players
    1 byte # of breakables on field (usually 100)
    ~100 bytes of breakable pos (each 1 byte)
    1 byte players that have custom name
        - 0001 P1 has custom name, 0010 P2 has custom name, etc.
    1-? bytes custom name per player (up to 4 times)

Server in-game packet format:
    1 byte State Identifier (One of "GameState" enum)
    1 byte # of connected players, who's alive
        - least significant 4 bits, # of connected players
        - most significant 4 bits, 1 bit per living player
            - 0001 P1 alive, 0010 P2 alive, etc.
    21 bytes PlayerInfo struct per player (up to 4 times)
    1 byte # of balloons on field
    2 bytes BallonInfo struct per balloon (up to ? times)
    1 byte # of explosions on field
    1 byte explosion pos per explosion (up to ? times)
    1 byte # of powerups on field
    2 bytes PowerupInfo struct per powerup (up to ? times)
*/

struct PlayerJoinInfo
{
    sf::Uint8 ID;
    std::string name;
};

// 21 bytes
struct PlayerInfo
{
    /**
     * type
     * 0000 0001 player 1
     * 0000 0010 player 2
     * 0000 0100 player 3
     * 0000 1000 player 4
     * 0001 0000 is computer
    **/
    sf::Uint8 type;
    float posx;
    float posy;
    float velx;
    float vely;
    float timeToStop;
};

// 2 bytes
struct BalloonInfo
{
    sf::Uint8 xy;
    sf::Uint8 typeRange;
    /** typeRange
     * 0000 0001 - super
     * 0000 0010 - rcontrol
     * 0000 0100 - pierce
     * 0000 1000 - ghost
     * 4 most significant bits is a 4-bit number defining range
    **/
};

// 2 bytes
struct PowerupInfo
{
    sf::Uint8 xy;
    sf::Uint8 type;
    /** type
    0 - nothing
    1 - balloon up
    2 - range up
    3 - speed up
    4 - kick upgrade
    5 - rcontrol upgrade
    6 - sballoon upgrade
    7 - pierce upgrade
    8 - spread upgrade
    9 - ghost upgrade
    **/
};

namespace SS
{
    enum GameState
    {
        WAITING_FOR_PLAYERS,
        WAITING_FOR_SERVER,
        STARTED,
        ENDED
    };
}

#endif
