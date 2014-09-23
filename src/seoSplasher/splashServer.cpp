
#include "splashServer.hpp"

#include <algorithm>

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

void SplashServer::registerConnectionMadeCall(std::function<void()> function)
{
    connectionMadeFunctions.push_back(function);
}

void SplashServer::registerConnectionLostCall(std::function<void(sf::Uint8)> function)
{
    connectionLostFunctions.push_back(function);
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

    std::for_each(connectionMadeFunctions.begin(), connectionMadeFunctions.end(), [] (std::function<void()> function) {
        function();
    });
}

void SplashServer::connectionLost(sf::Uint32 address)
{
    int i;
    for(i = 0; i < 5; ++i)
    {
        if(i != 4 && playerConnected[i] && playerAddresses[i] == address)
            break;
    }

    if(i != 4)
    {
        playerConnected[i] = false;

        std::for_each(connectionLostFunctions.begin(), connectionLostFunctions.end(), [i] (std::function<void(sf::Uint8)> function) {
            function(i);
        });
    }
}
