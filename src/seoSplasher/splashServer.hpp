
#ifndef SPLASH_SERVER_HPP
#define SPLASH_SERVER_HPP

#define START_TIMER_SECONDS 3.0f

#include <queue>
#include <functional>

#include "../connection.hpp"
#include "gridInfo.hpp"
#include "../context.hpp"
#include "splashNetworkIdentifiers.hpp"

class SplashServer : public Connection
{
public:
    SplashServer(Context context);

    void update(sf::Time dt);

    void registerConnectionMadeCall(std::function<void(sf::Uint8)> function);
    void registerConnectionLostCall(std::function<void(sf::Uint8)> function);

private:
    Context context;

    float updateTimer;

    bool playerConnected[4];
    sf::Uint32 playerAddresses[4];

    std::list<std::function<void(sf::Uint8)> > connectionMadeFunctions;
    std::list<std::function<void(sf::Uint8)> > connectionLostFunctions;

    void receivedPacket(sf::Packet packet, sf::Uint32 address);

    void connectionMade(sf::Uint32 address);
    void connectionLost(sf::Uint32 address);

    void sendPacket();
};

#endif
