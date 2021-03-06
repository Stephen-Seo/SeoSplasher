
#include "splashServer.hpp"

#include <algorithm>

#ifndef NDEBUG
  #include <iostream>
#endif

SplashServer::SplashServer(Context context) :
Connection(Connection::SERVER),
context(context),
updateTimer(CONNECTION_UPDATE_TIME),
prevState(SS::WAITING_FOR_PLAYERS)
{
    ignoreOutOfSequence = true;
    resendTimedOutPackets = false;

    for(int i = 0; i < 4; ++i)
        playerConnected[i] = false;

    if(*context.mode < 3) // not dedicated
        playerConnected[0] = true;

    // set server player's movetime value to something non-zero so s/he can move
    if(*context.mode == 2)
        context.scontext->movementTime[0] = 1.0f;
}

void SplashServer::update(sf::Time dt)
{
    Connection::update(dt);

    if(!acceptNewConnections && (context.scontext->gameState == SS::WAITING_FOR_PLAYERS || context.scontext->gameState == SS::WAITING_FOR_SERVER) && (!playerConnected[0] || !playerConnected[1] || !playerConnected[2] || !playerConnected[3]))
    {
        acceptNewConnections = true;
    }

    for(int i = 0; i < 4; ++i)
    {
        if(i == 0 && *context.mode == 2) // don't stop server player's movement
            continue;
        context.scontext->movementTime[i] -= dt.asSeconds();
        if(context.scontext->movementTime[i] < 0.0f)
            context.scontext->movementTime[i] = 0.0f;
    }
    updateTimer -= dt.asSeconds();
    if(updateTimer <= 0.0f)
    {
        updateTimer = CONNECTION_UPDATE_TIME;
        sendPacket();
    }

    if(prevState != context.scontext->gameState)
    {
        if(prevState == SS::ENDED && (context.scontext->gameState == SS::WAITING_FOR_PLAYERS || context.scontext->gameState == SS::WAITING_FOR_SERVER))
        {

            for(int i = 0; i < 4; ++i)
            {
                if(i == 0 && *context.mode == 2) // is non-spectating server
                    continue;
                if(playerConnected[i])
                {
                    for(auto iter = connectionMadeFunctions.begin(); iter != connectionMadeFunctions.end(); ++iter)
                    {
                        (*iter)(i);
                    }
                }
            }
        }


        prevState = context.scontext->gameState;
    }
}

void SplashServer::registerConnectionMadeCall(std::function<void(sf::Uint8)> function)
{
    connectionMadeFunctions.push_back(function);
}

void SplashServer::registerConnectionLostCall(std::function<void(sf::Uint8)> function)
{
    connectionLostFunctions.push_back(function);
}

void SplashServer::receivedPacket(sf::Packet packet, sf::Uint32 address)
{
    // get player ID from address
    int i;
    for(i = 0; i < 5; ++i)
    {
        if(i != 4 && playerConnected[i] && playerAddresses[i] == address)
            break;
    }

    if(i == 4)
    {
        return;
    }

    // get packet type
    sf::Uint8 packetID;
    if(!(packet >> packetID))
        return;

    sf::Uint8 temp;

    switch(packetID)
    {
    // pre game data
    case SS::WAITING_FOR_PLAYERS:
    case SS::WAITING_FOR_SERVER:
    {
        if(!(packet >> temp))
            return;
        if(temp != 0x0)
        {
            std::string name;
            if(!(packet >> name))
                return;
            context.scontext->customNames[i] = name;
        }
    }
        break;
    // in game data
    case SS::STARTED:
    {
        if(!context.scontext->playersAlive[i])
            return;
        if(!(packet >> temp))
            return;
        context.scontext->input[i] = temp;
        context.scontext->movementTime[i] = MOVEMENT_TIMEOUT_TIME;
    }
        break;
    default:
        return;
    }
}

void SplashServer::connectionMade(sf::Uint32 address)
{
    int i;
    for(i = 0; i < 4; ++i)
    {
        if(i == 0 && *context.mode < 3) // not dedicated
            continue;
        if(!playerConnected[i])
            break;
    }

    playerConnected[i] = true;
    playerAddresses[i] = address;

    if(playerConnected[0] && playerConnected[1] && playerConnected[2] && playerConnected[3])
        acceptNewConnections = false;

    std::for_each(connectionMadeFunctions.begin(), connectionMadeFunctions.end(), [i] (std::function<void(sf::Uint8)> function) {
        function(i);
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

        if((context.scontext->gameState == SS::WAITING_FOR_PLAYERS || context.scontext->gameState == SS::WAITING_FOR_SERVER) && (!playerConnected[0] || !playerConnected[1] || !playerConnected[2] || !playerConnected[3]))
        {
            acceptNewConnections = true;
        }

        std::for_each(connectionLostFunctions.begin(), connectionLostFunctions.end(), [i] (std::function<void(sf::Uint8)> function) {
            function(i);
        });
    }
}

void SplashServer::sendPacket()
{
    for(int i = 0; i < 4; ++i)
    {
        if(!playerConnected[i])
            continue;

        if(i == 0 && *context.mode == 2)
            continue;

        sf::Packet packet;
        sf::Uint32 sequenceID;
        sf::IpAddress address(playerAddresses[i]);
        sf::Uint8 temp;

        preparePacket(packet, sequenceID, address);

        // state identifier byte
        packet << (sf::Uint8) context.scontext->gameState;

        switch(context.scontext->gameState)
        {
        case SS::WAITING_FOR_PLAYERS:
        case SS::WAITING_FOR_SERVER:
        {
            // start timer byte
            packet << (context.scontext->startTimer >= 0.0f ? (sf::Uint8) (context.scontext->startTimer + 1.0f) : (sf::Uint8) 0xFF);

            // connected players byte
            temp = 0;
            for(int j = 0; j < 4; ++j)
            {
                if(playerConnected[j])
                    ++temp;
            }
            temp |= (0x10 << i);
            packet << temp;

            // number of breakables byte
            packet << (sf::Uint8) context.scontext->breakables.size();

            // breakable position bytes
            for(int j = 0; j < context.scontext->breakables.size(); ++j)
            {
                packet << context.scontext->breakables[j];
            }

            // players that have custom name byte
            temp = (context.scontext->customNames[0].size() != 0 ? 0x1 : 0x0);
            temp |= (context.scontext->customNames[1].size() != 0 ? 0x2 : 0x0);
            temp |= (context.scontext->customNames[2].size() != 0 ? 0x4 : 0x0);
            temp |= (context.scontext->customNames[3].size() != 0 ? 0x8 : 0x0);
            packet << temp;

            // custom names
            for(int j = 0; j < 4; ++j)
            {
                if(context.scontext->customNames[j].size() != 0)
                    packet << context.scontext->customNames[j];
            }
        }
            break;
        case SS::STARTED:
        case SS::ENDED:
        {
            // connected/living players byte
            temp = (playerConnected[0] ? 0x1 : 0x0);
            temp |= (playerConnected[1] ? 0x2 : 0x0);
            temp |= (playerConnected[2] ? 0x4 : 0x0);
            temp |= (playerConnected[3] ? 0x8 : 0x0);

            temp |= (context.scontext->playersAlive[0] ? 0x10 : 0x0);
            temp |= (context.scontext->playersAlive[1] ? 0x20 : 0x0);
            temp |= (context.scontext->playersAlive[2] ? 0x40 : 0x0);
            temp |= (context.scontext->playersAlive[3] ? 0x80 : 0x0);
            packet << temp;

            // player info bytes
            for(int j = 0; j < 4; ++j)
            {
                if(context.scontext->playersAlive[j])
                {
                    packet << context.scontext->ppositions[j]->x;
                    packet << context.scontext->ppositions[j]->y;
                    packet << context.scontext->pvelocities[j]->x;
                    packet << context.scontext->pvelocities[j]->y;
                }
            }

            // # of ballons byte
            packet << (sf::Uint8) context.scontext->balloons.size();

            // BallonInfo bytes
            for(auto iter = context.scontext->balloons.begin(); iter != context.scontext->balloons.end(); ++iter)
            {
                packet << iter->second.EID;
                packet << *(iter->second.posx);
                packet << *(iter->second.posy);
                if(*(iter->second.velx) != 0.0f)
                {
                    packet << (sf::Uint8) 0x0;
                    packet << *(iter->second.velx);
                    iter->second.typeRange |= 0x10;
                }
                else if(*(iter->second.vely) != 0.0f)
                {
                    packet << (sf::Uint8) 0x1;
                    packet << *(iter->second.vely);
                    iter->second.typeRange |= 0x10;
                }
                else
                {
                    packet << (sf::Uint8) 0x0;
                    packet << 0.0f;
                }
                packet << iter->second.typeRange;
            }

            // # of explosions byte
            packet << (sf::Uint8) context.scontext->explosions.size();

            // explosion pos bytes
            for(auto iter = context.scontext->explosions.begin(); iter != context.scontext->explosions.end(); ++iter)
            {
                packet << iter->second.xy;
                packet << iter->second.direction;
            }

            // # of powerups byte
            packet << (sf::Uint8) context.scontext->powerups.size();

            // PowerupInfo bytes
            for(auto iter = context.scontext->powerups.begin(); iter != context.scontext->powerups.end(); ++iter)
            {
                packet << iter->second.xy;
                packet << iter->second.type;
            }

            // sound info byte
            if(context.scontext->powerupPickedup[i])
            {
                context.scontext->powerupPickedup[i] = false;
                packet << (sf::Uint8) 0x1;
            }
            else
            {
                packet << (sf::Uint8) 0x0;
            }
        }
            break;
        default:
#ifndef NDEBUG
            std::clog << "WARNING: server in state CONNECTION_LOST, this should never happen!\n";
#endif
            break;
        }

        Connection::sendPacket(packet, sequenceID, address);
    }
}
