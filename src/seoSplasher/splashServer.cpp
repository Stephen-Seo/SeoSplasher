
#include "splashServer.hpp"

SplashServer::SplashServer(Context context, bool isSpectating) :
Connection(Connection::SERVER),
context(context),
isSpectating(isSpectating)
{
    for(int i = 0; i < 4; ++i)
        playerConnected[i] = false;

    for(int i = 0; i < GRID_TOTAL; ++i)
        grid[i] = 0;
}

void SplashServer::gridUpdate(const unsigned char* grid)
{
    for(int i = 0; i < GRID_TOTAL; ++i)
        this->grid[i] = grid[i];
}

void SplashServer::notifyBalloonInfo(BalloonInfo info)
{
    for(int i = 0; i < GRID_TOTAL; ++i)
    {
        if(!playerConnected[i])
            continue;

        sf::Packet packet;
        sf::Uint32 sequenceID;
        preparePacket(packet, sequenceID, sf::IpAddress(playerAddresses[i]));
        packet << (sf::Uint8) SS::PACKET_BALLOON;

        packet << info.xy << info.type << info.range << info.timer;

        sendPacket(packet, sequenceID, sf::IpAddress(playerAddresses[i]));
    }
}

void SplashServer::notifyBalloonDestroyed(BalloonInfo info)
{
    for(int i = 0; i < GRID_TOTAL; ++i)
    {
        if(!playerConnected[i])
            continue;

        sf::Packet packet;
        sf::Uint32 sequenceID;
        preparePacket(packet, sequenceID, sf::IpAddress(playerAddresses[i]));
        packet << (sf::Uint8) SS::PACKET_BDESTROYED;

        packet << info.xy << info.type << info.range << info.timer;

        sendPacket(packet, sequenceID, sf::IpAddress(playerAddresses[i]));
    }
}

void SplashServer::notifyBreakableDestroyed(BDestroyedInfo info)
{
    for(int i = 0; i < GRID_TOTAL; ++i)
    {
        if(!playerConnected[i])
            continue;

        sf::Packet packet;
        sf::Uint32 sequenceID;
        preparePacket(packet, sequenceID, sf::IpAddress(playerAddresses[i]));
        packet << (sf::Uint8) SS::PACKET_BRDESTROYED;

        packet << info.xy << info.ptype;

        sendPacket(packet, sequenceID, sf::IpAddress(playerAddresses[i]));
    }
}

void SplashServer::notifyPowerupDestroyed(sf::Uint8 xy)
{
    for(int i = 0; i < GRID_TOTAL; ++i)
    {
        if(!playerConnected[i])
            continue;

        sf::Packet packet;
        sf::Uint32 sequenceID;
        preparePacket(packet, sequenceID, sf::IpAddress(playerAddresses[i]));
        packet << (sf::Uint8) SS::PACKET_PDESTROYED;

        packet << xy;

        sendPacket(packet, sequenceID, sf::IpAddress(playerAddresses[i]));
    }
}

void SplashServer::notifyPlayerDead(sf::Uint8 ID)
{
    for(int i = 0; i < GRID_TOTAL; ++i)
    {
        if(!playerConnected[i])
            continue;

        sf::Packet packet;
        sf::Uint32 sequenceID;
        preparePacket(packet, sequenceID, sf::IpAddress(playerAddresses[i]));
        packet << (sf::Uint8) SS::PACKET_PDEAD;

        packet << ID;

        sendPacket(packet, sequenceID, sf::IpAddress(playerAddresses[i]));
    }
}

void SplashServer::notifyFinalSetup()
{
    for(int i = 0; i < 4; ++i)
    {
        if(!playerConnected[i])
            continue;

        sf::Packet packet;
        sf::Uint32 sequenceID;

        preparePacket(packet, sequenceID, sf::IpAddress(playerAddresses[i]));

        packet << (sf::Uint8) SS::PACKET_INIT;

        // Get and send position of every breakable
        for(sf::Uint8 j = 0; j < GRID_TOTAL; ++j)
        {
            if((grid[j] & 0x4) != 0)
                packet << j;
        }

        // End of breakable list notifier
        packet << (sf::Uint8) 0xFF;

        // Notify amount of players/computers
        // 0000 - all computers (not likely to ever be sent)
        // 0001 - player 1
        // 0010 - player 2
        // 0100 - player 3
        // 1000 - player 4

        sf::Uint8 playerInfo = 0;
        playerInfo |= (playerConnected[0] ? 0x1 : 0x0);
        playerInfo |= (playerConnected[1] ? 0x2 : 0x0);
        playerInfo |= (playerConnected[2] ? 0x4 : 0x0);
        playerInfo |= (playerConnected[3] ? 0x8 : 0x0);
        packet << playerInfo;

        sendPacket(packet, sequenceID, sf::IpAddress(playerAddresses[i]));
    }
}

void SplashServer::notifyPlayerJoined(sf::Uint8 ID)
{
    for(int i = 0; i < 4; ++i)
    {
        if(!playerConnected[i] || ID == i)
            continue;

        sf::Packet packet;
        sf::Uint32 sequenceID;
        preparePacket(packet, sequenceID, sf::IpAddress(playerAddresses[i]));

        packet << (sf::Uint8) SS::PACKET_PJOINED;

        packet << ID;

        sendPacket(packet, sequenceID, sf::IpAddress(playerAddresses[i]));
    }

    for(int i = 0; i < 4; ++i)
    {
        if(!playerConnected[i] || ID == i)
            continue;

        sf::Packet packet;
        sf::Uint32 sequenceID;
        preparePacket(packet, sequenceID, sf::IpAddress(playerAddresses[ID]));

        packet << (sf::Uint8) SS::PACKET_PJOINED;

        packet << (sf::Uint8) i;

        sendPacket(packet, sequenceID, sf::IpAddress(playerAddresses[ID]));
    }
}

void SplashServer::sendPlayerInfo(PlayerInfo info1, PlayerInfo info2, PlayerInfo info3, PlayerInfo info4)
{
    for(int i = 0; i < 4; ++i)
    {
        if(!playerConnected[i])
            continue;

        sf::Packet packet;
        sf::Uint32 sequenceID;
        preparePacket(packet, sequenceID, sf::IpAddress(playerAddresses[i]));

        packet << (sf::Uint8) SS::PACKET_PLAYER;

        packet << info1.type;
        packet << info1.posx;
        packet << info1.posy;
        packet << info1.velx;
        packet << info1.vely;
        packet << info1.timeToStop;

        packet << info2.type;
        packet << info2.posx;
        packet << info2.posy;
        packet << info2.velx;
        packet << info2.vely;
        packet << info2.timeToStop;

        packet << info3.type;
        packet << info3.posx;
        packet << info3.posy;
        packet << info3.velx;
        packet << info3.vely;
        packet << info3.timeToStop;

        packet << info4.type;
        packet << info4.posx;
        packet << info4.posy;
        packet << info4.velx;
        packet << info4.vely;
        packet << info4.timeToStop;

        sendPacket(packet, sequenceID, sf::IpAddress(playerAddresses[i]));
    }
}

void SplashServer::receivedPacket(sf::Packet packet)
{
    sf::Uint8 packetID;
    if(!(packet >> packetID))
        return;

    switch(packetID) //TODO
    {
    case SS::PACKET_INIT:
        break;
    case SS::PACKET_PJOINED:
        break;
    case SS::PACKET_PLAYER:
        break;
    case SS::PACKET_BALLOON:
        break;
    case SS::PACKET_BRDESTROYED:
        break;
    case SS::PACKET_PDESTROYED:
        break;
    case SS::PACKET_BDESTROYED:
        break;
    case SS::PACKET_PDEAD:
        break;
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

    notifyPlayerJoined(i);
}

void SplashServer::connectionLost(sf::Uint32 address)
{
    int i;
    for(i = 0; i < 4; ++i)
        if(playerConnected[i] && playerAddresses[i] == address)
            break;

    playerConnected[i] = false;
    acceptNewConnections = true;

    notifyPlayerDead(i);
}
