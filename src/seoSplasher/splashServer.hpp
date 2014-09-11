
#ifndef SPLASH_SERVER_HPP
#define SPLASH_SERVER_HPP

#include "../connection.hpp"

struct Context;

#define PACKET_INIT_INFO 0
#define PACKET_PLAYER_INFO 1
#define PACKET_BALLOON_INFO 2
#define PACKET_BRDESTROYED_INFO 3
#define PACKET_PDESTROYED_INFO 4
#define PACKET_BDESTROYED_INFO 5
#define PACKET_PICKUP_INFO 6

// 21 bytes
struct PlayerInfo
{
    unsigned char type;
    float posx;
    float posy;
    float velx;
    float vely;
    float timeToStop;
};

// 3 bytes
struct BalloonInfo
{
    unsigned char xy;
    unsigned char type;
    unsigned char range;
    float timer;
    /** type
     * 0001 - super
     * 0010 - rcontrol
     * 0100 - pierce
     * 1000 - ghost
    **/
};

// 2 bytes
struct BDestroyedInfo
{
    unsigned char xy;
    unsigned char ptype;
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

class SplashServer : public Connection
{
public:
    SplashServer(Context context);

    void sendObjectPosition(EntityInfo info);
    void notifyBreakableDestroyed(BDestroyedInfo);

private:
    Context context;

    void receivedPacket(sf::Packet packet);

};

#endif
