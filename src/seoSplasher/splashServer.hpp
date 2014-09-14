
#ifndef SPLASH_SERVER_HPP
#define SPLASH_SERVER_HPP

#define START_TIMER_SECONDS 3.0f

#include <queue>

#include "../connection.hpp"
#include "gridInfo.hpp"
#include "../context.hpp"
#include "splashNetworkIdentifiers.hpp"

class SplashServer : public Connection
{
public:
    SplashServer(Context context, bool isSpectating = true);

    void update(sf::Time dt);

    void gridUpdate(const unsigned char* grid);
    void notifyBalloonInfo(BalloonInfo info);
    void notifyBalloonDestroyed(BalloonInfo info);
    void notifyBreakableDestroyed(BrDestroyedInfo info);
    void notifyPowerupDestroyed(sf::Uint8 xy);
    void notifyPlayerDead(sf::Uint8 ID);
    void notifyGameState(SS::GameState state);

private:
    Context context;
    bool isSpectating;
    bool startTimer;
    float timer;
    unsigned char grid[GRID_TOTAL];

    bool playerConnected[4];
    sf::Uint32 playerAddresses[4];

    std::queue<BalloonInfo> newBalloons;
    std::queue<BrDestroyedInfo> destBreakables;
    std::queue<sf::Uint8> destPowerups;
    std::queue<sf::Uint8> deadPlayers;

    void notifyFinalSetup();
    void notifyPlayerJoined(PlayerJoinInfo info);
    void sendPlayerInfo(PlayerInfo info1, PlayerInfo info2, PlayerInfo info3, PlayerInfo info4);

    void receivedPacket(sf::Packet packet);

    void connectionMade(sf::Uint32 address);
    void connectionLost(sf::Uint32 address);
};

#endif
