
#ifndef SPLASH_SERVER_HPP
#define SPLASH_SERVER_HPP

#define START_TIMER_SECONDS 3.0F

#include <queue>
#include <functional>

#include "../connection.hpp"
#include "gridInfo.hpp"
#include "../context.hpp"
#include "splashNetworkIdentifiers.hpp"

class SplashServer : public Connection
{
public:
    explicit SplashServer(Context context);

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

    SS::GameState prevState;

    void receivedPacket(sf::Packet packet, sf::Uint32 address) override;

    void connectionMade(sf::Uint32 address) override;
    void connectionLost(sf::Uint32 address) override;

    void sendPacket();
};

#endif
