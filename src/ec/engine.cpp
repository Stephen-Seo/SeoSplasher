
#include "engine.hpp"

#include <algorithm>
#include <iostream>

#include "../context.hpp"
#include "../debugPrint.hpp"

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
    for(auto & system : systems)
    {
        nodeAdded |= system->checkEntity(*entity);
    }
    for(auto & drawSystem : drawSystems)
    {
        nodeAdded |= drawSystem->checkEntity(*entity);
    }
    if(!nodeAdded) {
        SS_DPRINT(DPLevel::DP_WARNING, "Entity (", entity->getID(), ") added without any paired nodes!");
    }
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

Entity* Engine::getEntity(int eID)
{
    auto findIter = entityMap.find(eID);
    if(findIter == entityMap.end())
        return nullptr;
    else
        return findIter->second.get();
}

void Engine::clear()
{
    systems.clear();
    drawSystems.clear();
    entityMap.clear();
    rfMap.clear();
    while(!deadQueue.empty())
        deadQueue.pop();
}

void Engine::clearEntities()
{
    for(auto & system : systems)
        system->clearEntities();
    for(auto & drawSystem : drawSystems)
        drawSystem->clearEntities();
    entityMap.clear();
    rfMap.clear();
    while(!deadQueue.empty())
        deadQueue.pop();
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
        deadQueue.pop();

        for(auto & system : systems)
        {
            system->removeEntity(eID);
        }
        for(auto & drawSystem : drawSystems)
        {
            drawSystem->removeEntity(eID);
        }

        entityMap.erase(eID);
    }

    for(auto & system : systems)
    {
        system->update(dt, context);
    }
}

void Engine::draw(Context context)
{
    for(auto & drawSystem : drawSystems)
    {
        drawSystem->update(sf::Time::Zero, context);
    }
}

void Engine::registerRemoveCall(int eID, const std::function<void()> &function)
{
    rfMap[eID].push(function);
}

