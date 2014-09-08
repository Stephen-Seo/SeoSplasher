
#include "nAIControl.hpp"

#include <typeindex>
#include <algorithm>
#include <iostream>
#include <random>

#include "cAIControl.hpp"
#include "cLiving.hpp"
#include "../ec/cPosition.hpp"
#include "../ec/cVelocity.hpp"
#include "AI/pathFinder.hpp"
#include "../context.hpp"
#include "utility.hpp"
#include "cPathFinderRef.hpp"
#include "cPowerup.hpp"
#include "cPickup.hpp"
#include "../ec/engine.hpp"
#include "direction.hpp"
#include "cWIndicator.hpp"
#include "cBalloon.hpp"

nAIControl::nAIControl() :
control(nullptr),
living(nullptr),
pos(nullptr),
vel(nullptr),
prevDest(-1)
{}

bool nAIControl::checkEntity(Entity& entity)
{
    return entity.hasComponent(std::type_index(typeid(cAIControl))) &&
           entity.hasComponent(std::type_index(typeid(cLiving))) &&
           entity.hasComponent(std::type_index(typeid(cPosition))) &&
           entity.hasComponent(std::type_index(typeid(cVelocity))) &&
           entity.hasComponent(std::type_index(typeid(cPathFinderRef)));
}

std::unique_ptr<Node> nAIControl::getNewNode()
{
    return std::unique_ptr<Node>(new nAIControl);
}

void nAIControl::getCReferencesFromEntity(Entity& entity)
{
    control = static_cast<cAIControl*>(entity.getComponent(std::type_index(typeid(cAIControl))));
    living = static_cast<cLiving*>(entity.getComponent(std::type_index(typeid(cLiving))));
    pos = static_cast<cPosition*>(entity.getComponent(std::type_index(typeid(cPosition))));
    vel = static_cast<cVelocity*>(entity.getComponent(std::type_index(typeid(cVelocity))));
    pfref = static_cast<cPathFinderRef*>(entity.getComponent(std::type_index(typeid(cPathFinderRef))));
    entityRemoved = &entity.removed;
}

void nAIControl::update(sf::Time dt, Context context)
{
    if(*entityRemoved)
        return;

    bool justPlacedBalloon = false;
    control->timer -= dt.asSeconds();
    if(control->timer <= 0.0f || control->currentAction == AI::NONE)
    {
        control->timer = control->aiTickTime;
        control->currentAction = control->ai.determineAction(*pos, *living, *control->pf, *context.ecEngine, *context.rGen);
        if(Utility::collidesAgainstComponent(pos->x, pos->y, std::type_index(typeid(cWIndicator)), *context.ecEngine))
        {
            if(living->kickUpgrade == 0)
                paths = control->pf->getValidDestinations(*pos, *context.ecEngine, 0x16);
            else
                paths = control->pf->getValidDestinations(*pos, *context.ecEngine, 0x14);
        }
        else
        {
            if(living->kickUpgrade == 0)
                paths = control->pf->getValidDestinations(*pos, *context.ecEngine, 0x36);
            else
                paths = control->pf->getValidDestinations(*pos, *context.ecEngine, 0x34);
        }

        const unsigned char* grid = control->pf->getValidGrid(*context.ecEngine);

        int xy = (int)((pos->x + (float)(-GRID_OFFSET_X + GRID_SQUARE_SIZE / 2)) / GRID_SQUARE_SIZE) + (int)((pos->y + (float)(-GRID_OFFSET_Y + GRID_SQUARE_SIZE / 2)) / GRID_SQUARE_SIZE) * GRID_WIDTH;
        int prev, destination = -1;
        std::uniform_int_distribution<> dist(0,paths.size() - 1);
        int r = dist(*context.rGen);

        std::clog << "STATE: " << control->currentAction << '\n';
        vel->x = 0.0f;
        vel->y = 0.0f;

        if(prevDest == -1)
        {
            switch(control->currentAction)
            {
            case AI::PLACE_BALLOON:
                justPlacedBalloon = true;
                Utility::createBalloon(pos->x, pos->y, *living, context, control->ID, &control->fired, *pfref);
                control->timer = 0.0f;
                control->pf->invalidateValidGrid();
                break;
            case AI::GET_POWERUP:
                destination = -1;
                for(auto iter = paths.begin(); iter != paths.end(); ++iter)
                {
                    if((grid[iter->first] & 0x8) != 0)
                    {
                        destination = iter->first;
                        if(r <= 0)
                            break;
                    }
                    --r;
                }
                if(destination == -1)
                {
                    std::clog << "WARNING: failed to find valid powerup path\n";
                    control->timer = 0.0f;
                    break;
                }
                break;
            case AI::MOVE_TO_ENEMY:
                break;
            case AI::MOVE_TO_BREAKABLE:
                destination = -1;
                for(auto iter = paths.begin(); iter != paths.end(); ++iter)
                {
                    if((iter->first % GRID_WIDTH != 0 && (grid[iter->first - 1] & 0x4) != 0) ||
                       (iter->first % GRID_WIDTH != GRID_WIDTH - 1 && (grid[iter->first + 1] & 0x4) != 0) ||
                       (iter->first - GRID_WIDTH >= 0 && (grid[iter->first - GRID_WIDTH] & 0x4) != 0) ||
                       (iter->first + GRID_WIDTH < GRID_TOTAL && (grid[iter->first + GRID_WIDTH] & 0x4) != 0))
                    {
                        destination = iter->first;
                        if(r <= 0)
                            break;
                    }
                    --r;
                }
                if(destination == -1)
                {
                    std::clog << "WARNING: failed to find adjacent breakable\n";
                    control->timer = 0.0f;
                    break;
                }
                break;
            case AI::MOVE_TO_SAFETY:
                destination = -1;
                for(auto iter = paths.begin(); iter != paths.end(); ++iter)
                {
                    if((grid[iter->first] & 0x36) == 0)
                    {
                        destination = iter->first;
                        if(r <= 0)
                            break;
                    }
                    --r;
                }
                if(destination == -1)
                {
                    std::clog << "WARNING: failed to find safe spot\n";
                    control->timer = 0.0f;
                    break;
                }
                break;
            case AI::KICK_BALLOON:
                break;
            case AI::PANIC:
                break;
            case AI::WAIT:
                control->timer = BALLOON_ALIVE_TIME;
                break;
            default:
                break;
            }
        }
        if(destination != -1 || prevDest != -1)
        {
            if(prevDest != -1)
                destination = prevDest;
            Direction::Direction prevDir = Direction::NONE;
            Direction::Direction destDir = Direction::NONE;
            if(destination != xy && Utility::isAligned(pos->x, pos->y))
            {
                std::map<int,int> rpaths;
                int dest = destination;
                prevDest = destination;
                while(destination != xy)
                {
                    if(paths.find(destination) == paths.end())
                        break;
                    rpaths.insert(std::make_pair(paths[destination], destination));
                    destination = paths[destination];
                }

                int i = GRID_TOTAL;
                destination = xy;
                prev = destination;
                while(destination != dest)
                {
                    destination = rpaths[destination];
                    if(destination % GRID_WIDTH == prev % GRID_WIDTH)
                    {
                        destDir = Direction::VERTICAL;
                    }
                    else
                    {
                        destDir = Direction::HORIZONTAL;
                    }

                    if(prevDir == Direction::NONE)
                        prevDir = destDir;
                    else if(prevDir != destDir)
                        break;
                    prev = destination;

                    if(i-- == 0)
                        break;
                }
                if(prevDir == Direction::NONE)
                    prevDir = destDir;
                if(destination == prevDest)
                    prevDest = -1;
            }
            else
                prevDir = Direction::PLUS;

            if(prevDir == Direction::VERTICAL)
            {
                if(prev < xy)
                {
                    vel->x = 0.0f;
                    vel->y = -DEFAULT_SPEED - SPEED_UP_MULT * (float)living->speedUp;
                    control->timer = (float)((xy / GRID_WIDTH - prev / GRID_WIDTH) * GRID_SQUARE_SIZE) / -vel->y;
                }
                else
                {
                    vel->x = 0.0f;
                    vel->y = DEFAULT_SPEED + SPEED_UP_MULT * (float)living->speedUp;
                    if(prev == xy)
                        control->timer = 0.0f;
                    else
                        control->timer = (float)((prev / GRID_WIDTH - xy / GRID_WIDTH) * GRID_SQUARE_SIZE) / vel->y;
                }
            }
            else if(prevDir == Direction::HORIZONTAL)
            {
                if(prev < xy)
                {
                    vel->x = -DEFAULT_SPEED - SPEED_UP_MULT * (float)living->speedUp;
                    vel->y = 0.0f;
                    control->timer = (float)((xy - prev) * GRID_SQUARE_SIZE) / -vel->x;
                }
                else
                {
                    vel->x = DEFAULT_SPEED + SPEED_UP_MULT * (float)living->speedUp;
                    vel->y = 0.0f;
                    if(prev == xy)
                        control->timer = 0.0f;
                    else
                        control->timer = (float)((prev - xy) * GRID_SQUARE_SIZE) / vel->x;
                }
            }
            else if(prevDir == Direction::PLUS)
            {
                float offsetx = (pos->x - (float)GRID_OFFSET_X) / (float)GRID_SQUARE_SIZE;
                float offsety = (pos->y - (float)GRID_OFFSET_Y) / (float)GRID_SQUARE_SIZE;
                float x = xy % GRID_WIDTH;
                float y = xy / GRID_WIDTH;
                if(offsetx < x)
                {
                    vel->x = -DEFAULT_SPEED - SPEED_UP_MULT * (float)living->speedUp;
                    vel->y = 0.0f;
                    control->timer = (pos->x - offsetx) / -vel->x;
                }
                else if(offsetx > x)
                {
                    vel->x = DEFAULT_SPEED + SPEED_UP_MULT * (float)living->speedUp;
                    vel->y = 0.0f;
                    control->timer = (offsetx - pos->x) / vel->x;
                }
                else if(offsety < y)
                {
                    vel->x = 0.0f;
                    vel->y = -DEFAULT_SPEED - SPEED_UP_MULT * (float)living->speedUp;
                    control->timer = (pos->y - offsety) / -vel->y;
                }
                else if(offsety >= y)
                {
                    vel->x = 0.0f;
                    vel->y = DEFAULT_SPEED + SPEED_UP_MULT * (float)living->speedUp;
                    if(offsety != y)
                        control->timer = (offsety - pos->y) / vel->y;
                    else
                        control->timer = 0.0f;
                }
            }
            else
            {
                std::cerr << "ERROR: Failed to validate next tile on move\n";
                std::clog << "prev is " << prev << '\n';
                control->timer = 0.0f;
            }
        }
    }

    HitInfo powerinfo = Utility::collideAgainstComponent(pos->x, pos->y, std::type_index(typeid(cPickup)), *context.ecEngine);
    for(auto piter = powerinfo.hit.begin(); piter != powerinfo.hit.end(); ++piter)
    {
        cPowerup* powerup = static_cast<cPowerup*>((*piter)->getComponent(std::type_index(typeid(cPowerup))));

        switch(powerup->powerup)
        {
        case cPowerup::BALLOON_UP:
            ++(living->balloonUp);
            break;
        case cPowerup::RANGE_UP:
            ++(living->rangeUp);
            break;
        case cPowerup::SPEED_UP:
            ++(living->speedUp);
            break;
        case cPowerup::KICK_UPGRADE:
            ++(living->kickUpgrade);
            break;
        case cPowerup::RCONTROL_UPGRADE:
            ++(living->rControlUpgrade);
            break;
        case cPowerup::SBALLOON_UPGRADE:
            ++(living->sBalloonUpgrade);
            break;
        case cPowerup::PIERCE_UPGRADE:
            ++(living->pierceUpgrade);
            break;
        case cPowerup::SPREAD_UPGRADE:
            ++(living->spreadUpgrade);
            break;
        case cPowerup::GHOST_UPGRADE:
            ++(living->ghostUpgrade);
            break;
        default:
            break;
        }

        context.ecEngine->removeEntity((*piter)->getID());
    }

    if(living->rControlUpgrade > 0)
    {
        bool inDanger = Utility::collidesAgainstComponent(pos->x, pos->y, std::type_index(typeid(cWIndicator)), *context.ecEngine);
        if(justPlacedBalloon || inDanger)
        {
            control->fired = false;
        }
        else if(!justPlacedBalloon && !inDanger)
        {
            control->fired = true;
        }
    }
}
