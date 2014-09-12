
#ifndef SPLASH_SERVER_HPP
#define SPLASH_SERVER_HPP

#define START_TIMER_SECONDS 3.0f

#include <queue>

#include "../connection.hpp"
#include "gridInfo.hpp"
#include "../context.hpp"

#define PACKET_INIT 0
#define PACKET_PJOINED 1
#define PACKET_PLAYER 2
#define PACKET_BALLOON 3
#define PACKET_BRDESTROYED 4
#define PACKET_PDESTROYED 5
#define PACKET_BDESTROYED 6
#define PACKET_PICKUP 7
#define PACKET_PDEAD 8

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
     * 0001 - super
     * 0010 - rcontrol
     * 0100 - pierce
     * 1000 - ghost
    **/
};

// 2 bytes
struct BDestroyedInfo
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
    enum GameStatus
    {
        WAITING,
        STARTED,
        ENDED
    };
}

class SplashServer : public Connection
{
public:
    SplashServer(Context context, bool isSpectating = true);

    void update(sf::Time dt);

    void gridUpdate(const unsigned char* grid);
    void notifyBalloonInfo(BalloonInfo info);
    void notifyBreakableDestroyed(BDestroyedInfo info);
    void notifyPowerupDestroyed(sf::Uint8 xy);
    void notifyPlayerDead(sf::Uint8 ID);

private:
    Context context;
    bool isSpectating;
    bool startTimer;
    float timer;
    unsigned char grid[GRID_TOTAL];

    bool playerConnected[4];
    sf::Uint32 playerAddresses[4];

    std::queue<BalloonInfo> newBalloons;
    std::queue<BDestroyedInfo> destBreakables;
    std::queue<sf::Uint8> destPowerups;
    std::queue<sf::Uint8> deadPlayers;

    void notifyFinalSetup();
    void notifyPlayerJoined(sf::Uint8 ID);
    void sendPlayerInfo(PlayerInfo info1, PlayerInfo info2, PlayerInfo info3, PlayerInfo info4);

    void receivedPacket(sf::Packet packet);

    void connectionMade(sf::Uint32 address);
    void connectionLost(sf::Uint32 address);
};

#endif
