
#include "splashClient.hpp"

SplashClient::SplashClient(Context context) :
Connection(Connection::CLIENT),
context(context),
connectedToServer(false)
{}

void SplashClient::sendCustomName(std::string name)
{
    if(!connectedToServer)
        return;

    sf::Packet packet;
    sf::Uint32 sequenceID;
    preparePacket(packet, sequenceID, sf::IpAddress(address));
    packet << (sf::Uint8) SS::PACKET_PJOINED;

    packet << name;

    sendPacket(packet, sequenceID, sf::IpAddress(address));
}

void SplashClient::balloonCreated(BalloonInfo info)
{
    if(!connectedToServer)
        return;

    sf::Packet packet;
    sf::Uint32 sequenceID;
    preparePacket(packet, sequenceID, sf::IpAddress(address));
    packet << (sf::Uint8) SS::PACKET_BALLOON;

    packet << info.xy << info.type << info.range << info.timer;

    sendPacket(packet, sequenceID, sf::IpAddress(address));
}

void SplashClient::remoteToggled(bool pressed)
{
    if(!connectedToServer)
        return;

    sf::Packet packet;
    sf::Uint32 sequenceID;
    preparePacket(packet, sequenceID, sf::IpAddress(address));
    packet << (sf::Uint8) SS::PACKET_BDESTROYED;

    packet << (sf::Uint8) (pressed ? 0x1 : 0x0);

    sendPacket(packet, sequenceID, sf::IpAddress(address));
}

std::queue<BalloonInfo> SplashClient::getCreatedBalloons()
{
    std::queue<BalloonInfo> balloons = newBalloons;
    while(!newBalloons.empty())
        newBalloons.pop();
    return balloons;
}

void SplashClient::receivedPacket(sf::Packet packet)
{
    sf::Uint8 packetType;
    packet >> packetType;
    switch(static_cast<SS::Packet>(packetType))
    {
    case SS::PACKET_INIT:
    {
        std::list<sf::Uint8> breakables;
        sf::Uint8 breakable;

        if(!(packet >> breakable))
            return;
        while(breakable != (sf::Uint8) 0xFF)
        {
            breakables.push_back(breakable);
            if(!(packet >> breakable))
                return;
        }

        sf::Uint8 playerInfo;

        if(!(packet >> playerInfo))
            return;

        //TODO update local info

        repeatInitInfo(breakables, playerInfo);
    }
        break;
    case SS::PACKET_PJOINED:
    {
        PlayerJoinInfo info;
        if(!(packet >> info.ID) || !(packet >> info.name))
            return;

        //TODO update local info
    }
        break;
    case SS::PACKET_PLAYER:
    {
        std::array<PlayerInfo, 4> info;
        for(int i = 0; i < 4; ++i)
        {
            if(!(packet >> info[i].type) || !(packet >> info[i].posx) || !(packet >> info[i].posy) || !(packet >> info[i].velx) || !(packet >> info[i].vely) || !(packet >> info[i].timeToStop))
                return;
        }

        //TODO update local info
    }
        break;
    case SS::PACKET_BALLOON:
    {
        BalloonInfo info;
        if(!(packet >> info.xy) || !(packet >> info.type) || !(packet >> info.range) || !(packet >> info.timer))
            return;

        //TODO update local info
    }
        break;
    case SS::PACKET_BRDESTROYED:
    {
        BrDestroyedInfo info;
        if(!(packet >> info.xy) || !(packet >> info.ptype))
            return;

        //TODO update local info
    }
        break;
    case SS::PACKET_PDESTROYED:
    {
        sf::Uint8 xy;
        if(!(packet >> xy))
            return;

        //TODO update local info
    }
        break;
    case SS::PACKET_BDESTROYED:
    {
        BalloonInfo info;
        if(!(packet >> info.xy) || !(packet >> info.type) || !(packet >> info.range))
            return;

        //TODO update local info
    }
        break;
    case SS::PACKET_PDEAD:
    {
        sf::Uint8 ID;
        if(!(packet >> ID))
            return;

        //TODO update local info
    }
        break;
    case SS::PACKET_GAME_STATE:
    {
        sf::Uint8 state;
        if(!(packet >> state))
            return;

        //TODO update local info
    }
        break;
    default:
        break;
    }
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

void SplashClient::repeatInitInfo(std::list<sf::Uint8> breakables, sf::Uint8 playerInfo)
{
    if(!connectedToServer)
        return;

    sf::Packet packet;
    sf::Uint32 sequenceID;
    preparePacket(packet, sequenceID, sf::IpAddress(address));
    packet << (sf::Uint8) SS::PACKET_INIT;
    
    for(auto iter = breakables.begin(); iter != breakables.end(); ++iter)
    {
        packet << *iter;
    }
    packet << (sf::Uint8) 0xFF;

    packet << playerInfo;

    sendPacket(packet, sequenceID, sf::IpAddress(address));
}

