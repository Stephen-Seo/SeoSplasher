
#include "splashClient.hpp"

SplashClient::SplashClient(Context context) :
Connection(Connection::CLIENT),
context(context)
{}

void SplashClient::sendCustomName(std::string name)
{
}

void SplashClient::balloonCreated(BalloonInfo info)
{
}

void SplashClient::remoteToggled(bool pressed)
{
}

void SplashClient::receivedPacket(sf::Packet packet)
{
}

void SplashClient::connectionMade(sf::Uint32 address)
{
}

void SplashClient::connectionLost(sf::Uint32 address)
{
}

