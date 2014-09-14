
#ifndef SPLASH_NETWORK_IDENTIFIERS_HPP
#define SPLASH_NETWORK_IDENTIFIERS_HPP

namespace SS
{
    enum Packet
    {
        PACKET_INIT,
        PACKET_PJOINED,
        PACKET_PLAYER,
        PACKET_BALLOON,
        PACKET_BRDESTROYED,
        PACKET_PDESTROYED,
        PACKET_BDESTROYED,
        PACKET_PDEAD,
        PACKET_GAME_STATE
    };
}

/*
PACKET_INIT
    From Server:
        Contains position of all breakables, followed by an end of list notifier
        (0xFF) and a byte of information regarding which players are human.
    From Client:
        On receiving INIT packet from Server, send back exact same information
        to confirm correctness. Server will check if info matches game data,
        and will resend if there is a difference.
PACKET_PJOINED
    From Server:
        Contains the player ID of a new player that joined.
        TODO add info for custom names.
    From Client:
        Client sends custom name information as a PJOINED packet. This is
        optional but will be sent as a string. Will only be accepted by server
        during setup.
        time.
PACKET_PLAYER
    From Server:
        Contains type, pos, vel, and time left for movement information per
        player. Packet size greater than 84 bytes.
    From Client:
        Contains type, pos, vel, and time left for movement information about
        the client.
PACKET_BALLOON
    From Server:
        Contains grid-position, type, range, and time left information for a new
        balloon.
    From Client:
        Contains grid-position, type, range, and time left information for a new
        balloon created by the client. Client will assume that the balloon has
        been created when sending this packet, but will require confirmation
        by the server, otherwise 'placed' balloon will be removed.
PACKET_BRDESTROYED
    From Server:
        Contains grid-position of a breakable that was destroyed and information
        on possibly contained powerups.
    From Client:
        Client does not send BRDESTROYED packets; server handles breakables
        completely.
PACKET_PDESTROYED
    From Server:
        Contains grid-position of a powerup that was consumed/destroyed.
    From Client:
        Client does not send PDESTROYED packets; server checks if powerup is
        collected or destroyed and will notify connected players.
PACKET_BDESTROYED
    From Server:
        Contains balloon info of a balloon that was destroyed.
    From Client:
        Client sends packet to notify when the rcontrol button is pressed.
        Will be sent with a byte of information:
            0 - released
            1 - pressed
PACKET_PDEAD
    From Server:
        Contains ID of a player that just died.
    From Client:
        Client does not send PDEAD packets; server notifies all players when
        a player dies.
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

// 3 bytes
struct BalloonInfo
{
    sf::Uint8 xy;
    sf::Uint8 type;
    sf::Uint8 range;
    float timer;
    /** type
     * 0000 0001 - super
     * 0000 0010 - rcontrol
     * 0000 0100 - pierce
     * 0000 1000 - ghost
    **/
};

// 2 bytes
struct BrDestroyedInfo
{
    sf::Uint8 xy;
    sf::Uint8 ptype;
    /** ptype
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
        WAITING,
        STARTED,
        ENDED
    };
}

#endif
