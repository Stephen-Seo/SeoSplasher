
#ifndef SPLASH_CLIENT_HPP
#define SPLASH_CLIENT_HPP

#include <string>
#include <queue>
#include <list>
#include <functional>

#include <SFML/System.hpp>

#include "../connection.hpp"
#include "splashNetworkIdentifiers.hpp"
#include "../context.hpp"

class SplashClient : public Connection
{
public:
    SplashClient(Context context);

    void update(sf::Time dt);

    void registerPlayersChangedCall(std::function<void(sf::Uint8)> function);
    void registerGameStartedCall(std::function<void()> function);
private:
    Context context;
    sf::Uint32 address;
    bool connectedToServer;

    float updateTimer;

    sf::Uint8 playerID;

    std::vector<sf::Uint8> breakables;

    sf::Uint8 numberOfPlayers;

    std::queue<int> IDqueue;

    std::list<std::function<void(sf::Uint8)> > playerChangedFunctions;

    void receivedPacket(sf::Packet packet, sf::Uint32 address);

    void connectionMade(sf::Uint32 address);
    void connectionLost(sf::Uint32 address);

    void sendPacket();

    void removeUnmentionedBalloons();
    void removeUnmentionedExplosions();
    void removeUnmentionedPowerups();
};

#endif

