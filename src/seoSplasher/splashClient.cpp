
#include "splashClient.hpp"

#ifndef NDEBUG
  #include <iostream>
#endif

#include "../ec/engine.hpp"
#include "utility.hpp"
#include "soundContext.hpp"

SplashClient::SplashClient(Context context) :
Connection(Connection::CLIENT),
context(context),
connectedToServer(false),
updateTimer(CONNECTION_UPDATE_TIME),
playerID(0xFF),
breakables(100),
numberOfPlayers(0),
endMusicPlayed(false)
{
    ignoreOutOfSequence = true;
    resendTimedOutPackets = false;
}

void SplashClient::update(sf::Time dt)
{
    Connection::update(dt);

    if(connectedToServer)
    {
        for(int i = 0; i < 4; ++i)
        {
            context.scontext->movementTime[i] -= dt.asSeconds();
            if(context.scontext->movementTime[i] < 0.0f)
                context.scontext->movementTime[i] = 0.0f;
        }

        updateTimer -= dt.asSeconds();
        if(updateTimer <= 0.0f)
        {
            updateTimer = CONNECTION_UPDATE_TIME;
            sendPacket();

            if(!context.scontext->breakablesSet && breakables.size() > 0)
            {
                context.scontext->breakablesSet = true;
                for(int i = 0; i < breakables.size(); ++i)
                {
                    context.scontext->breakableXYToEID.insert(std::make_pair(breakables[i], Utility::clientCreateBreakable(breakables[i], context)));
                }
            }
        }

    }
}

void SplashClient::registerPlayersChangedCall(std::function<void(sf::Uint8)> function)
{
    playerChangedFunctions.push_back(function);
}

void SplashClient::registerGameRestartedCall(std::function<void()> function)
{
    gameRestartedFunctions.push_back(function);
}

void SplashClient::receivedPacket(sf::Packet packet, sf::Uint32 address)
{
    bool playersChanged = false;
    sf::Uint8 playerInfo;

    sf::Uint8 packetType, temp;
    if(!(packet >> packetType))
        return;

    SS::GameState gs = static_cast<SS::GameState>(packetType);
    if(context.scontext->gameState != gs)
    {
        if(context.scontext->gameState == SS::ENDED && (gs == SS::WAITING_FOR_PLAYERS || gs == SS::WAITING_FOR_SERVER))
        {
            for(auto iter = gameRestartedFunctions.begin(); iter != gameRestartedFunctions.end(); ++iter)
            {
                (*iter)();
            }
            numberOfPlayers = 0;
            breakables.clear();
            endMusicPlayed = false;
            context.scontext->explosionXYToEID.clear();
            context.scontext->powerupXYToEID.clear();
            context.scontext->breakableXYToEID.clear();
        }
        else if((context.scontext->gameState == SS::WAITING_FOR_PLAYERS || context.scontext->gameState == SS::WAITING_FOR_SERVER) && gs == SS::STARTED)
        {
            context.sfxContext->happened[SoundContext::COUNTDOWN_ENDED] = true;
            context.scontext->startTimer = 0.0f;
        }

        context.scontext->gameState = gs;
    }

    switch(packetType)
    {
    case SS::WAITING_FOR_PLAYERS:
    case SS::WAITING_FOR_SERVER:
    {
        // byte timer
        if(!(packet >> temp))
            return;
        if(temp != 0xFF)
        {
            context.scontext->startTimer = (float) temp;
        }
        else
        {
            context.scontext->startTimer = -1.0f;
        }

        // # of connected players
        if(!(packet >> temp))
            return;
        playerInfo = temp;
        if(numberOfPlayers != (temp & 0xF))
            playersChanged = true;
        numberOfPlayers = (temp & 0xF);
        if(playerID == 0xFF)
        {
            if((temp & 0x10) != 0x0)
                playerID = 0;
            else if((temp & 0x20) != 0x0)
                playerID = 1;
            else if((temp & 0x40) != 0x0)
                playerID = 2;
            else if((temp & 0x80) != 0x0)
                playerID = 3;
        }

        // # of breakables
        if(!(packet >> temp))
            return;
        if(temp > breakables.size())
            breakables.resize(temp);
        // assign breakable positions
        for(int i = 0; i < temp; ++i)
        {
            if(!(packet >> breakables[i]))
                return;
        }

        // custom name info byte
        if(!(packet >> temp))
            return;

        // get custom names
        if((temp & 0x1) != 0x0)
        {
            if(!(packet >> context.scontext->customNames[0]))
                return;
        }
        if((temp & 0x2) != 0x0)
        {
            if(!(packet >> context.scontext->customNames[1]))
                return;
        }
        if((temp & 0x4) != 0x0)
        {
            if(!(packet >> context.scontext->customNames[2]))
                return;
        }
        if((temp & 0x8) != 0x0)
        {
            if(!(packet >> context.scontext->customNames[3]))
                return;
        }


        if(playersChanged)
        {
            for(auto iter = playerChangedFunctions.begin(); iter != playerChangedFunctions.end(); ++iter)
            {
                (*iter)(playerInfo);
            }
        }
    }
        break;
    case SS::STARTED:
    case SS::ENDED:
    {
        // alive/connected player info
        if(!(packet >> temp))
            return;
        // remove if marked dead
        if(context.scontext->playersAlive[0] && (temp & 0x10) == 0)
        {
            context.ecEngine->removeEntity(context.scontext->playerEID[0]);
        }
        if(context.scontext->playersAlive[1] && (temp & 0x20) == 0)
        {
            context.ecEngine->removeEntity(context.scontext->playerEID[1]);
        }
        if(context.scontext->playersAlive[2] && (temp & 0x40) == 0)
        {
            context.ecEngine->removeEntity(context.scontext->playerEID[2]);
        }
        if(context.scontext->playersAlive[3] && (temp & 0x80) == 0)
        {
            context.ecEngine->removeEntity(context.scontext->playerEID[3]);
        }

        if(gs == SS::ENDED && !endMusicPlayed)
        {
            endMusicPlayed = true;
            if((temp & (0x10 << playerID)) != 0x0)
            {
                context.sfxContext->happened[SoundContext::GAME_ENDED_WELL] = true;
            }
            else
            {
                context.sfxContext->happened[SoundContext::GAME_ENDED_BADLY] = true;
            }
        }

        // playerInfo data
        if((temp & 0x10) != 0)
        {
            if(!(packet >> context.scontext->ppositions[0]->x))
                return;
            if(!(packet >> context.scontext->ppositions[0]->y))
                return;
            if(!(packet >> context.scontext->pvelocities[0]->x))
                return;
            if(!(packet >> context.scontext->pvelocities[0]->y))
                return;
            context.scontext->movementTime[0] = MOVEMENT_TIMEOUT_TIME;
        }
        if((temp & 0x20) != 0)
        {
            if(!(packet >> context.scontext->ppositions[1]->x))
                return;
            if(!(packet >> context.scontext->ppositions[1]->y))
                return;
            if(!(packet >> context.scontext->pvelocities[1]->x))
                return;
            if(!(packet >> context.scontext->pvelocities[1]->y))
                return;
            context.scontext->movementTime[1] = MOVEMENT_TIMEOUT_TIME;
        }
        if((temp & 0x40) != 0)
        {
            if(!(packet >> context.scontext->ppositions[2]->x))
                return;
            if(!(packet >> context.scontext->ppositions[2]->y))
                return;
            if(!(packet >> context.scontext->pvelocities[2]->x))
                return;
            if(!(packet >> context.scontext->pvelocities[2]->y))
                return;
            context.scontext->movementTime[2] = MOVEMENT_TIMEOUT_TIME;
        }
        if((temp & 0x80) != 0)
        {
            if(!(packet >> context.scontext->ppositions[3]->x))
                return;
            if(!(packet >> context.scontext->ppositions[3]->y))
                return;
            if(!(packet >> context.scontext->pvelocities[3]->x))
                return;
            if(!(packet >> context.scontext->pvelocities[3]->y))
                return;
            context.scontext->movementTime[3] = MOVEMENT_TIMEOUT_TIME;
        }

        // # of balloons
        if(!(packet >> temp))
            return;

        // balloon info
        sf::Uint8 direction, typeRange;
        int SEID;
        float posx, posy, velx = 0.0f, vely = 0.0f;
        while(!IDqueue.empty())
        {
            IDqueue.pop();
        }
        for(int i = 0; i < temp; ++i)
        {
            if(!(packet >> SEID))
                return;

            IDqueue.push(SEID);

            if(!(packet >> posx))
                return;
            if(!(packet >> posy))
                return;
            if(!(packet >> direction))
                return;
            if(direction == 0x0)
            {
                if(!(packet >> velx))
                    return;
            }
            else
            {
                if(!(packet >> vely))
                    return;
            }
            if(!(packet >> typeRange))
                return;

            auto findIter = context.scontext->balloons.find(SEID);
            if(findIter == context.scontext->balloons.end())
            {
                context.scontext->balloons.insert(std::make_pair(SEID, Utility::clientCreateBalloon(posx, posy, typeRange, context)));
            }
            else if((typeRange & 0x10) != 0x0)
            {
                Entity* balloonEntity = context.ecEngine->getEntity(findIter->second.EID);
                if(balloonEntity != nullptr)
                {
                    cPosition* pos = static_cast<cPosition*>(balloonEntity->getComponent(std::type_index(typeid(cPosition))));
                    cVelocity* vel = static_cast<cVelocity*>(balloonEntity->getComponent(std::type_index(typeid(cVelocity))));

                    pos->x = posx;
                    pos->y = posy;
                    vel->x = velx;
                    vel->y = vely;
                }
#ifndef NDEBUG
                else
                {
                    std::clog << "SplashClient> WARNING: Balloon EID " << findIter->second.EID << " not found in engine!\n";
                }
#endif
            }
        }
        removeUnmentionedBalloons();

        // # of explosions
        if(!(packet >> temp))
            return;
        // explosion data
        sf::Uint8 xy;
        while(!IDqueue.empty())
        {
            IDqueue.pop();
        }

        for(int i = 0; i < temp; ++i)
        {
            if(!(packet >> xy))
                return;

            IDqueue.push(xy);

            if(!(packet >> direction))
                return;

            if(context.scontext->explosionXYToEID.find(xy) == context.scontext->explosionXYToEID.end())
            {
                context.scontext->explosionXYToEID.insert(std::make_pair(xy, Utility::clientCreateExplosion(xy, direction, context)));
                auto findIter = context.scontext->breakableXYToEID.find(xy);
                if(findIter != context.scontext->breakableXYToEID.end())
                {
                    context.ecEngine->removeEntity(findIter->second);
                    context.scontext->breakableXYToEID.erase(findIter);
                }
            }
        }
        removeUnmentionedExplosions();

        // # of powerups
        if(!(packet >> temp))
            return;

        // powerup data
        while(!IDqueue.empty())
        {
            IDqueue.pop();
        }

        for(int i = 0; i < temp; ++i)
        {
            if(!(packet >> xy))
                return;

            IDqueue.push(xy);

            if(!(packet >> typeRange))
                return;

            if(context.scontext->powerupXYToEID.find(xy) == context.scontext->powerupXYToEID.end())
            {
                context.scontext->powerupXYToEID.insert(std::make_pair(xy, Utility::clientCreatePowerup(xy, typeRange, context)));
            }
        }
        removeUnmentionedPowerups();

        // sound info byte
        if(!(packet >> temp))
            return;
        if(temp != 0x0) // if sound byte is changed, then TODO change here!
        {
            context.sfxContext->happened[SoundContext::PICKUP_GET] = true;
        }
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
    context.scontext->gameState = SS::CONNECTION_LOST;
}

void SplashClient::sendPacket()
{
    sf::Packet packet;
    sf::Uint32 sequenceID;
    sf::IpAddress address(this->address);

    preparePacket(packet, sequenceID, address);

    // state identifier byte
    packet << (sf::Uint8) context.scontext->gameState;

    switch(context.scontext->gameState)
    {
    case SS::WAITING_FOR_PLAYERS:
    case SS::WAITING_FOR_SERVER:
    {
        if(context.scontext->ownName.size() == 0)
        {
            // has custom name
            packet << (sf::Uint8) 0x1;

            // sending custom name
            packet << context.scontext->ownName;
        }
        else
        {
            // no custom name
            packet << (sf::Uint8) 0x0;
        }
    }
        break;
    case SS::STARTED:
    {
        // send input
        packet << context.scontext->input[playerID];
    }
        break;
    case SS::ENDED: // nothing to send
        break;
    default:
        break;
    }

    Connection::sendPacket(packet, sequenceID, address);
}

void SplashClient::removeUnmentionedBalloons()
{
    std::map<int, BalloonInfo> removalMap = context.scontext->balloons;

    while(!IDqueue.empty())
    {
        removalMap.erase(IDqueue.front());
        IDqueue.pop();
    }

    for(auto iter = removalMap.begin(); iter != removalMap.end(); ++iter)
    {
        context.ecEngine->removeEntity(context.scontext->balloons[iter->first].EID);
        context.scontext->balloons.erase(iter->first);
    }
}

void SplashClient::removeUnmentionedExplosions()
{
    std::map<sf::Uint8, int> removalMap = context.scontext->explosionXYToEID;

    while(!IDqueue.empty())
    {
        removalMap.erase(IDqueue.front());
        IDqueue.pop();
    }

    for(auto iter = removalMap.begin(); iter != removalMap.end(); ++iter)
    {
        context.ecEngine->removeEntity(context.scontext->explosionXYToEID[iter->first]);
        context.scontext->explosionXYToEID.erase(iter->first);
    }
}

void SplashClient::removeUnmentionedPowerups()
{
    std::map<sf::Uint8, int> removalMap = context.scontext->powerupXYToEID;

    while(!IDqueue.empty())
    {
        removalMap.erase(IDqueue.front());
        IDqueue.pop();
    }

    for(auto iter = removalMap.begin(); iter != removalMap.end(); ++iter)
    {
        context.ecEngine->removeEntity(context.scontext->powerupXYToEID[iter->first]);
        context.scontext->powerupXYToEID.erase(iter->first);
    }
}
