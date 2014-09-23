
#include "splashServer.hpp"

SplashServer::SplashServer(Context context, bool isSpectating) :
Connection(Connection::SERVER),
context(context),
isSpectating(isSpectating)
{
    ignoreOutOfSequence = true;
    resendTimedOutPackets = false;

    for(int i = 0; i < 4; ++i)
        playerConnected[i] = false;
}

void SplashServer::update(sf::Time dt)
{
    Connection::update(dt);
}

void SplashServer::setGridReference(const unsigned char* grid)
{
    this->grid = grid;
}

void SplashServer::receivedPacket(sf::Packet packet)
{
    sf::Uint8 packetID;
    if(!(packet >> packetID))
        return;

    switch(packetID) //TODO
    {
    default:
        return;
    }
}

void SplashServer::connectionMade(sf::Uint32 address)
{
    int i;
    for(i = 0; i < 4; ++i)
        if(!playerConnected[i])
            break;

    playerConnected[i] = true;
    playerAddresses[i] = address;

    if(playerConnected[0] && playerConnected[1] && playerConnected[2] && playerConnected[3])
        acceptNewConnections = false;
}

void SplashServer::connectionLost(sf::Uint32 address)
{
    int i;
    for(i = 0; i < 4; ++i)
        if(playerConnected[i] && playerAddresses[i] == address)
            break;

    playerConnected[i] = false;
    acceptNewConnections = true;
}
