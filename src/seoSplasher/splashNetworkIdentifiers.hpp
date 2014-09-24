
#ifndef SPLASH_NETWORK_IDENTIFIERS_HPP
#define SPLASH_NETWORK_IDENTIFIERS_HPP

#define SERVER_UPDATE_TIME (1.0f / 15.0f)

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
    1 byte timer until game start (if not 0xFF)
    1 byte # of connected players
        - least significant 4 bits, 4 bit number of connected players
        - most significant 4 bits, bitfield indicating which player is the reciever
    1 byte # of breakables on field (usually 100)
    ~100 bytes of breakable pos (each 1 byte)
    1 byte players that have custom name
        - 0001 P1 has custom name, 0010 P2 has custom name, etc.
    1-? bytes custom name per player (up to 4 times)

Server in-game packet format:
    1 byte State Identifier (One of "GameState" enum)
    1 byte # of connected players, who's alive
        - least significant 4 bits, 1 bit per connected player
            - 0001 P1 connected, 0010 P2 connected, etc.
        - most significant 4 bits, 1 bit per living player
            - 0001 P1 alive, 0010 P2 alive, etc.
    20 bytes PlayerInfo struct per player (up to 4 times)
    1 byte # of balloons on field
    18 bytes BallonInfo per balloon (up to ? times)
    1 byte # of explosions on field
    2 byte explosionInfo per explosion (up to ? times)
    1 byte # of powerups on field
    2 bytes PowerupInfo per powerup (up to ? times)
*/

/*
Will be sent as 18 bytes as:
    4 byte EID
    4 byte posx
    4 byte posy
    1 byte velDirection (horizontal/vertical - 0/1)
    4 byte vel
    1 byte typeRange
*/
struct BalloonInfo
{
    int EID;
    float* posx;
    float* posy;
    float* velx;
    float* vely;
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
struct ExplosionInfo
{
    sf::Uint8 xy;
    sf::Uint8 direction;
};

// 2 bytes
struct PowerupInfo
{
    sf::Uint8 xy;
    sf::Uint8 type;
    /** type (same as cPowerup::Powerup enum)
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
        ENDED,
        CONNECTION_LOST
    };
}

#endif
