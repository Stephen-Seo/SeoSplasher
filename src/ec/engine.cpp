
#include "engine.hpp"

#include <iostream>
#include <algorithm>

#include "../context.hpp"

Engine::Engine()
{
}

void Engine::addSystem(std::unique_ptr<Node> type)
{
    System* newSystem = new System(std::move(type));
    systems.push_back(std::unique_ptr<System>(newSystem));
}

void Engine::addDrawSystem(std::unique_ptr<Node> type)
{
    System* newSystem = new System(std::move(type));
    drawSystems.push_back(std::unique_ptr<System>(newSystem));
}

void Engine::addEntity(std::unique_ptr<Entity> entity)
{
    entity->removed = false;
    bool nodeAdded = false;
    for(auto iter = systems.begin(); iter != systems.end(); ++iter)
    {
        nodeAdded |= (*iter)->checkEntity(*entity.get());
    }
    for(auto iter = drawSystems.begin(); iter != drawSystems.end(); ++iter)
    {
        nodeAdded |= (*iter)->checkEntity(*entity.get());
    }
    if(!nodeAdded)
        std::clog << "WARNING: Entity (" << entity->getID() << ") added without any paired nodes!\n";
    else
        std::clog << "ENGINE: added E(" << entity->getID() << ")\n";
    entityMap.insert(std::make_pair(entity->getID(), std::move(entity)));
}

void Engine::removeEntity(int eID)
{
    bool removed = false;
    {
        auto iter = entityMap.find(eID);
        if(iter != entityMap.end())
        {
            iter->second->removed = true;
            deadQueue.push(eID);
            removed = true;
        }
    }
    if(removed)
    {
        while(!rfMap[eID].empty())
        {
            rfMap[eID].front()();
            rfMap[eID].pop();
        }
        rfMap.erase(eID);
    }
}

std::map<int, std::unique_ptr<Entity> >::iterator Engine::getEntityIterBegin()
{
    return entityMap.begin();
}

std::map<int, std::unique_ptr<Entity> >::iterator Engine::getEntityIterEnd()
{
    return entityMap.end();
}

void Engine::update(sf::Time dt, Context context)
{
    while(!deadQueue.empty())
    {
        int eID = deadQueue.front();
        std::clog << "REMOVING: entity (" << eID << ")\n";
        deadQueue.pop();

        for(auto iter = systems.begin(); iter != systems.end(); ++iter)
        {
            (*iter)->removeEntity(eID);
        }
        for(auto iter = drawSystems.begin(); iter != drawSystems.end(); ++iter)
        {
            (*iter)->removeEntity(eID);
        }

        entityMap.erase(eID);
    }

    for(auto iter = systems.begin(); iter != systems.end(); ++iter)
    {
        (*iter)->update(dt, context);
    }
}

void Engine::draw(Context context)
{
    for(auto iter = drawSystems.begin(); iter != drawSystems.end(); ++iter)
    {
        (*iter)->update(sf::Time::Zero, context);
    }
}

void Engine::registerRemoveCall(int eID, std::function<void()> function)
{
    rfMap[eID].push(function);
}

