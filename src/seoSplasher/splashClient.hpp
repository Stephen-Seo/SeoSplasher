
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

    void sendCustomName(std::string name);
    void balloonCreated(BalloonInfo info);
    void remoteToggled(bool pressed);
    std::queue<BalloonInfo> getCreatedBalloons();

private:
    Context context;
    sf::Uint32 address;
    bool connectedToServer;
    std::queue<BalloonInfo> newBalloons;

    void receivedPacket(sf::Packet packet);
    void connectionMade(sf::Uint32 address);
    void connectionLost(sf::Uint32 address);
    void repeatInitInfo(std::list<sf::Uint8> breakables, sf::Uint8 playerInfo);

};

#endif

