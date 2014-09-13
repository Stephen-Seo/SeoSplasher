
#ifndef SPLASH_CLIENT_HPP
#define SPLASH_CLIENT_HPP

#include <SFML/System.hpp>

#include "../connection.hpp"
#include "splashNetworkIdentifiers.hpp"
#include "../context.hpp"

class SplashClient : public Connection
{
public:
    SplashClient(Context context);

    void update(sf::Time dt);

    void remoteToggled(bool pressed);
    void balloonCreated(BalloonInfo info);
private:
    Context context;

    void receivedPacket(sf::Packet packet);
    void connectionMade(sf::Uint32 address);
    void connectionLost(sf::Uint32 address);

};

#endif

