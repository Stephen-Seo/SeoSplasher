
#include "nAIControl.hpp"

#include <typeindex>
#include <algorithm>
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
#include "cPathFinder.hpp"
#include "../debugPrint.hpp"

nAIControl::nAIControl() :
control(nullptr),
living(nullptr),
pos(nullptr),
vel(nullptr),
alignAction(false),
tick(0),
controlFireTime(0.0F)
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
    return std::unique_ptr<Node>(new nAIControl());
}

void nAIControl::getCReferencesFromEntity(Entity& entity)
{
    control = static_cast<cAIControl*>(entity.getComponent(std::type_index(typeid(cAIControl))));
    living = static_cast<cLiving*>(entity.getComponent(std::type_index(typeid(cLiving))));
    pos = static_cast<cPosition*>(entity.getComponent(std::type_index(typeid(cPosition))));
    vel = static_cast<cVelocity*>(entity.getComponent(std::type_index(typeid(cVelocity))));
    pfref = static_cast<cPathFinderRef*>(entity.getComponent(std::type_index(typeid(cPathFinderRef))));
    entityRemoved = &entity.removed;
#ifndef NDEBUG
    eid = entity.getID();
#endif
}

void nAIControl::update(sf::Time dt, Context context)
{
    if(*entityRemoved)
        return;

    if(prevPos.x == pos->x && prevPos.y == pos->y)
        ++tick;
    else
        tick = 0;
    prevPos.x = pos->x;
    prevPos.y = pos->y;

    if(alignAction || tick == TICK_TO_ALIGN)
    {
        sf::Vector2f aligned = Utility::alignToGrid(pos->x, pos->y);
        pos->x = aligned.x;
        pos->y = aligned.y;
        alignAction = false;
        if(tick == TICK_TO_ALIGN)
            control->timer = 0.4F;
        tick = 0;
    }

    bool justPlacedBalloon = false;
    control->timer -= dt.asSeconds();
    if(control->timer <= 0.0F || control->currentAction == AI::NONE)
    {
        control->timer = control->aiTickTime;
        pam = control->ai.determineAction(*pos, *living, *control->pf, *context.ecEngine, *context.rGen);
        control->currentAction = pam.action;
        //const unsigned char* grid = control->pf->getValidGrid(*context.ecEngine);

        int xy = (int)((pos->x + (float)(-GRID_OFFSET_X + GRID_SQUARE_SIZE / 2)) / GRID_SQUARE_SIZE) + (int)((pos->y + (float)(-GRID_OFFSET_Y + GRID_SQUARE_SIZE / 2)) / GRID_SQUARE_SIZE) * GRID_WIDTH;
        int prev;

        //int r;
        //if(pam.paths.size() <= 1)
        //    r = 0;
        //else
        //{
        //    std::uniform_int_distribution<> dist(0, pam.paths.size() - 1);
        //    r = dist(*context.rGen);
        //}

#ifndef NDEBUG
        switch(control->currentAction) {
        case AI::PLACE_BALLOON:
            SS_DPRINT(DPLevel::DP_DEBUG, '[', eid, "] STATE: PLACE_BALLOON");
            break;
        case AI::GET_POWERUP:
            SS_DPRINT(DPLevel::DP_DEBUG, '[', eid, "] STATE: GET_POWERUP");
            break;
        case AI::MOVE_TO_ENEMY:
            SS_DPRINT(DPLevel::DP_DEBUG, '[', eid, "] STATE: MOVE_TO_ENEMY");
            break;
        case AI::MOVE_TO_BREAKABLE:
            SS_DPRINT(DPLevel::DP_DEBUG, '[', eid, "] STATE: MOVE_TO_BREAKABLE");
            break;
        case AI::MOVE_TO_SAFETY:
            SS_DPRINT(DPLevel::DP_DEBUG, '[', eid, "] STATE: MOVE_TO_SAFETY");
            break;
        case AI::KICK_BALLOON:
            SS_DPRINT(DPLevel::DP_DEBUG, '[', eid, "] STATE: KICK_BALLOON");
            break;
        case AI::PANIC:
            SS_DPRINT(DPLevel::DP_DEBUG, '[', eid, "] STATE: PANIC");
            break;
        case AI::WAIT:
            SS_DPRINT(DPLevel::DP_DEBUG, '[', eid, "] STATE: WAIT");
            break;
        default:
            SS_DPRINT(DPLevel::DP_DEBUG, '[', eid, "] STATE: INVALID/UNKNOWN");
            break;
        }
#endif
        vel->x = 0.0F;
        vel->y = 0.0F;

        if(pam.destination == -1)
        {
            rpaths.clear();
            switch(control->currentAction)
            {
            case AI::PLACE_BALLOON:
                justPlacedBalloon = true;
                Utility::createBalloon(pos->x, pos->y, *living, context, control->ID, &control->fired, *pfref);
                control->timer = 0.0F;
                control->pf->invalidateValidGrid();
                break;
            case AI::GET_POWERUP:
                if(pam.destination == -1)
                {
#ifndef NDEBUG
                    SS_DPRINT(DPLevel::DP_DEBUG, '[', eid, "] WARNING: failed to find valid powerup path");
#endif
                    control->timer = 0.0F;
                    break;
                }
                break;
            case AI::MOVE_TO_ENEMY:
                if(pam.destination == -1)
                {
#ifndef NDEBUG
                    SS_DPRINT(DPLevel::DP_DEBUG, '[', eid, "] WARNING: failed to find valid path to enemy");
#endif
                    control->timer = 0.0F;
                    break;
                }
                break;
            case AI::MOVE_TO_BREAKABLE:
                if(pam.destination == -1)
                {
#ifndef NDEBUG
                    SS_DPRINT(DPLevel::DP_DEBUG, '[', eid, "] WARNING: failed to find adjacent breakable");
#endif
                    control->timer = 0.0F;
                    break;
                }
                break;
            case AI::MOVE_TO_SAFETY:
                if(pam.destination == -1)
                {
#ifndef NDEBUG
                    SS_DPRINT(DPLevel::DP_DEBUG, '[', eid, "] WARNING: failed to find safe spot");
#endif
                    control->timer = 0.0F;
                    break;
                }
                break;
            case AI::KICK_BALLOON:
                break;
            case AI::PANIC:
                break;
            case AI::WAIT:
                control->timer = BALLOON_ALIVE_TIME - 0.3F;
                alignAction = true;
                break;
            default:
                break;
            }
        }
        // destination set
        if(pam.destination != -1)
        {
            Direction::Direction prevDir = Direction::NONE;
            Direction::Direction destDir = Direction::NONE;
            if(pam.destination != xy || !Utility::isAligned(pos->x, pos->y))
            {
                int dest = pam.destination;
                if(rpaths.empty())
                {
                    while(dest != xy)
                    {
                        if(pam.paths.find(dest) == pam.paths.end())
                            break;
                        rpaths.insert(std::make_pair(pam.paths[dest], dest));
                        dest = pam.paths[dest];
                    }
                }

                int i = GRID_TOTAL;
                dest = xy;
                prev = dest;
                while(dest != pam.destination)
                {
                    if(rpaths.find(dest) == rpaths.end())
                    {
#ifndef NDEBUG
                        SS_DPRINT(DPLevel::DP_DEBUG, '[', eid, "] WARNING: dest not in rpaths");
#endif
                        break;
                    }
                    dest = rpaths[dest];
                    if(dest % GRID_WIDTH == prev % GRID_WIDTH)
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
                    prev = dest;

                    if(i-- == 0)
                        break;
                }
                if(prevDir == Direction::NONE)
                    prevDir = destDir;
                if(dest == pam.destination)
                {
                    pam.destination = -1;
                }
            }
            else
                prevDir = Direction::PLUS;

            if(prevDir == Direction::VERTICAL)
            {
                if(prev < xy)
                {
                    vel->x = 0.0F;
                    vel->y = -DEFAULT_SPEED - SPEED_UP_MULT * (float)living->speedUp;
                    control->timer = (float)((xy / GRID_WIDTH - prev / GRID_WIDTH) * GRID_SQUARE_SIZE) / -vel->y;
                }
                else
                {
                    vel->x = 0.0F;
                    vel->y = DEFAULT_SPEED + SPEED_UP_MULT * (float)living->speedUp;
                    if(prev == xy)
                        control->timer = 0.0F;
                    else
                        control->timer = (float)((prev / GRID_WIDTH - xy / GRID_WIDTH) * GRID_SQUARE_SIZE) / vel->y;
                }
            }
            else if(prevDir == Direction::HORIZONTAL)
            {
                if(prev < xy)
                {
                    vel->x = -DEFAULT_SPEED - SPEED_UP_MULT * (float)living->speedUp;
                    vel->y = 0.0F;
                    control->timer = (float)((xy - prev) * GRID_SQUARE_SIZE) / -vel->x;
                }
                else
                {
                    vel->x = DEFAULT_SPEED + SPEED_UP_MULT * (float)living->speedUp;
                    vel->y = 0.0F;
                    if(prev == xy)
                        control->timer = 0.0F;
                    else
                        control->timer = (float)((prev - xy) * GRID_SQUARE_SIZE) / vel->x;
                }
            }
            else if(prevDir == Direction::PLUS || prevDir == Direction::NONE)
            {
                if(prevDir == Direction::NONE)
                {
#ifndef NDEBUG
                    SS_DPRINT(DPLevel::DP_ERROR, "Failed to validate next tile on move, aligning to grid");
#endif
                    rpaths.clear();
                }
                float offsetx = (pos->x - (float)GRID_OFFSET_X) / (float)GRID_SQUARE_SIZE;
                float offsety = (pos->y - (float)GRID_OFFSET_Y) / (float)GRID_SQUARE_SIZE;
                float x = xy % GRID_WIDTH;
                float y = xy / GRID_WIDTH;
                if(offsetx < x)
                {
                    vel->x = -DEFAULT_SPEED - SPEED_UP_MULT * (float)living->speedUp;
                    vel->y = 0.0F;
                    control->timer = (pos->x - offsetx) / -vel->x;
                }
                else if(offsetx > x)
                {
                    vel->x = DEFAULT_SPEED + SPEED_UP_MULT * (float)living->speedUp;
                    vel->y = 0.0F;
                    control->timer = (offsetx - pos->x) / vel->x;
                }
                else if(offsety < y)
                {
                    vel->x = 0.0F;
                    vel->y = -DEFAULT_SPEED - SPEED_UP_MULT * (float)living->speedUp;
                    control->timer = (pos->y - offsety) / -vel->y;
                }
                else if(offsety > y)
                {
                    vel->x = 0.0F;
                    vel->y = DEFAULT_SPEED + SPEED_UP_MULT * (float)living->speedUp;
                    control->timer = (offsety - pos->y) / vel->y;
                }
                else
                    control->timer = 0.0F;
                alignAction = true;
            }
        }
    }

    HitInfo powerinfo = Utility::collideAgainstComponent(pos->x, pos->y, std::type_index(typeid(cPickup)), *context.ecEngine);
    for(auto & piter : powerinfo.hit)
    {
        cPickup* pickup = static_cast<cPickup*>(piter->getComponent(std::type_index(typeid(cPickup))));
        if(pickup->hit)
            continue;

        cPowerup* powerup = static_cast<cPowerup*>(piter->getComponent(std::type_index(typeid(cPowerup))));

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

        pickup->hit = true;
    }

    if(living->rControlUpgrade > 0) //TODO fix inDanger detection
    {
        bool inDanger = Utility::livingInDanger(pos->x, pos->y, pfref->cpf->pf, context);
        if(justPlacedBalloon || inDanger)
        {
            controlFireTime = 0.0F;
            control->fired = false;
        }
        else if(!justPlacedBalloon && !inDanger)
        {
            controlFireTime += dt.asSeconds();
            if(controlFireTime >= RCONTROL_TIME)
                control->fired = true;
            else
            {
                std::uniform_real_distribution<float> dist(0,RCONTROL_TIME);
                if(dist(*context.rGen) <= controlFireTime)
                    control->fired = true;
            }
        }
    }
}
