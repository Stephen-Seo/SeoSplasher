
#ifndef SPLASH_CLIENT_HPP
#define SPLASH_CLIENT_HPP

#include <string>
#include <queue>
#include <list>
#include <SFML/System.hpp>

#include "../connection.hpp"
#include "splashNetworkIdentifiers.hpp"
#include "../context.hpp"

class SplashClient : public Connection
{
public:
    SplashClient(Context context);

    void update(sf::Time dt);

private:
    Context context;
    sf::Uint32 address;
    bool connectedToServer;

    void receivedPacket(sf::Packet packet);
    void connectionMade(sf::Uint32 address);
    void connectionLost(sf::Uint32 address);

};

#endif

