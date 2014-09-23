
#ifndef SPLASH_SERVER_HPP
#define SPLASH_SERVER_HPP

#define START_TIMER_SECONDS 3.0f
#define SERVER_UPDATE_TIME (1.0f / 15.0f)

#include <queue>
#include <functional>

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

    void registerConnectionMadeCall(std::function<void()> function);
    void registerConnectionLostCall(std::function<void(sf::Uint8)> function);

private:
    Context context;

    bool isSpectating;
    bool startTimer;
    float timer;
    const unsigned char* grid;

    bool playerConnected[4];
    sf::Uint32 playerAddresses[4];

    std::list<std::function<void()> > connectionMadeFunctions;
    std::list<std::function<void(sf::Uint8)> > connectionLostFunctions;

    void receivedPacket(sf::Packet packet);

    void connectionMade(sf::Uint32 address);
    void connectionLost(sf::Uint32 address);
};

#endif
