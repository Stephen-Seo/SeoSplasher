
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

    void setGridReference(const unsigned char* grid);

private:
    Context context;

    bool isSpectating;
    bool startTimer;
    float timer;
    const unsigned char* grid;

    bool playerConnected[4];
    sf::Uint32 playerAddresses[4];

    void receivedPacket(sf::Packet packet);

    void connectionMade(sf::Uint32 address);
    void connectionLost(sf::Uint32 address);
};

#endif
