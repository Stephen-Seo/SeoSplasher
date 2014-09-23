
#include "splashClient.hpp"

SplashClient::SplashClient(Context context) :
Connection(Connection::CLIENT),
context(context),
connectedToServer(false)
{}

void SplashClient::update(sf::Time dt)
{
    Connection::update(dt);
}

void SplashClient::receivedPacket(sf::Packet packet)
{
    sf::Uint8 packetType;
    packet >> packetType;
}

void SplashClient::connectionMade(sf::Uint32 address)
{
    this->address = address;
    connectedToServer = true;
}

void SplashClient::connectionLost(sf::Uint32 address)
{
    connectedToServer = false;
}

