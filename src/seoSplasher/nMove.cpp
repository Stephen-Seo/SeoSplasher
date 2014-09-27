
#include "nMove.hpp"

#include <cmath>

#include "../ec/cPosition.hpp"
#include "../ec/cVelocity.hpp"
#include "../context.hpp"
#include "gridInfo.hpp"
#include "utility.hpp"
#include "cBreakable.hpp"
#include "cWall.hpp"
#include "cBalloon.hpp"
#include "cLiving.hpp"
#include "cMoveDisabler.hpp"
#include "cAIControl.hpp"
#include "soundContext.hpp"

nMove::nMove() :
pos(nullptr),
vel(nullptr),
living(nullptr),
disable(nullptr),
isBalloon(false),
isAI(false)
{}

bool nMove::checkEntity(Entity& entity)
{
    return entity.hasComponent(std::type_index(typeid(cPosition))) &&
           entity.hasComponent(std::type_index(typeid(cVelocity)));
}

std::unique_ptr<Node> nMove::getNewNode()
{
    return std::unique_ptr<Node>(new nMove);
}

void nMove::getCReferencesFromEntity(Entity& entity)
{
    pos = static_cast<cPosition*>(entity.getComponent(std::type_index(typeid(cPosition))));
    vel = static_cast<cVelocity*>(entity.getComponent(std::type_index(typeid(cVelocity))));
    entityRemoved = &entity.removed;
    isBalloon = entity.hasComponent(std::type_index(typeid(cBalloon)));
    isAI = entity.hasComponent(std::type_index(typeid(cAIControl)));
    if(entity.hasComponent(std::type_index(typeid(cLiving))))
    {
        living = static_cast<cLiving*>(entity.getComponent(std::type_index(typeid(cLiving))));
    }
    if(entity.hasComponent(std::type_index(typeid(cMoveDisabler))))
    {
        disable = static_cast<cMoveDisabler*>(entity.getComponent(std::type_index(typeid(cLiving))));
    }
}

void nMove::update(sf::Time dt, Context context)
{
    if(*entityRemoved)
        return;
    if(disable && disable->disable)
    {
        vel->x = 0.0f;
        vel->y = 0.0f;
        return;
    }
    // don't move continuously if pos/vel of player hasn't been updated for a while
    if(*context.mode != 0 && living && !isAI && context.scontext->movementTime[living->ID] <= 0.0f)
        return;

    bool ignoreBalloons = Utility::collidesAgainstComponent(pos->x, pos->y, std::type_index(typeid(cBalloon)), *context.ecEngine);

    float prev = pos->x;
    pos->x += vel->x * dt.asSeconds();

    if(pos->x < (float)GRID_OFFSET_X || pos->x >= (float)(GRID_RIGHT - GRID_SQUARE_SIZE))
    {
        pos->x = prev;
    }
    else
    {
        HitInfo info = Utility::collideAgainstComponent(pos->x, pos->y, std::type_index(typeid(cWall)), *context.ecEngine);
        HitInfo infoBreak = Utility::collideAgainstComponent(pos->x, pos->y, std::type_index(typeid(cBreakable)), *context.ecEngine);
        HitInfo infoBalloon;
        if(!ignoreBalloons)
            infoBalloon = Utility::collideAgainstComponent(pos->x, pos->y, std::type_index(typeid(cBalloon)), *context.ecEngine);
        if(!info.hit.empty() || !infoBreak.hit.empty() || !infoBalloon.hit.empty())
        {
            if(isBalloon)
            {
                vel->x = 0.0f;
            }
            if(info.hit.size() == 1 && infoBreak.hit.empty())
            {
                cPosition* hpos = static_cast<cPosition*>(info.hit.front()->getComponent(std::type_index(typeid(cPosition))));
                float offset = std::abs(pos->y - hpos->y);
                if(offset >= (float)(GRID_SQUARE_SIZE - MAX_MOVE_OFFSET) && offset <= (float)(GRID_SQUARE_SIZE + MAX_MOVE_OFFSET))
                {
                    if(pos->y > hpos->y)
                    {
                        pos->y = hpos->y + (float)GRID_SQUARE_SIZE;
                    }
                    else
                    {
                        pos->y = hpos->y - (float)GRID_SQUARE_SIZE;
                    }
                }
                else if(offset <= MOVE_SNAP_OFFSET)
                {
                    sf::Vector2f v = Utility::alignToGrid(pos->x, pos->y);
                    pos->x = v.x;
                    pos->y = v.y;
                }
                else
                {
                    pos->x = prev;
                }
            }
            else if(info.hit.empty() && infoBreak.hit.size() == 1)
            {
                cPosition* hpos = static_cast<cPosition*>(infoBreak.hit.front()->getComponent(std::type_index(typeid(cPosition))));
                float offset = std::abs(pos->y - hpos->y);
                if(offset <= MOVE_SNAP_OFFSET)
                {
                    sf::Vector2f v = Utility::alignToGrid(pos->x, pos->y);
                    pos->x = v.x;
                    pos->y = v.y;
                }
            }
            else
            {
                if(!isBalloon && living && living->kickUpgrade > 0 && infoBalloon.hit.size() == 1)
                {
                    context.sfxContext->happened[SoundContext::BALLOON_KICKED] = true;
                    if(prev < pos->x)
                    {
                        static_cast<cVelocity*>(infoBalloon.hit.front()->getComponent(std::type_index(typeid(cVelocity))))->x = BALLOON_KICK_SPEED;
                    }
                    else
                    {
                        static_cast<cVelocity*>(infoBalloon.hit.front()->getComponent(std::type_index(typeid(cVelocity))))->x = -BALLOON_KICK_SPEED;
                    }
                }
                pos->x = prev;
            }
        }
    }

    prev = pos->y;
    pos->y += vel->y * dt.asSeconds();

    if(pos->y < (float)GRID_OFFSET_Y || pos->y >= (float)(GRID_BOTTOM - GRID_SQUARE_SIZE))
    {
        pos->y = prev;
    }
    else
    {
        HitInfo info = Utility::collideAgainstComponent(pos->x, pos->y, std::type_index(typeid(cWall)), *context.ecEngine);
        HitInfo infoBreak = Utility::collideAgainstComponent(pos->x, pos->y, std::type_index(typeid(cBreakable)), *context.ecEngine);
        HitInfo infoBalloon;
        if(!ignoreBalloons)
            infoBalloon = Utility::collideAgainstComponent(pos->x, pos->y, std::type_index(typeid(cBalloon)), *context.ecEngine);
        if(!info.hit.empty() || !infoBreak.hit.empty() || !infoBalloon.hit.empty())
        {
            if(isBalloon)
            {
                vel->y = 0.0f;
            }
            if(info.hit.size() == 1 && infoBreak.hit.empty())
            {
                cPosition* hpos = static_cast<cPosition*>(info.hit.front()->getComponent(std::type_index(typeid(cPosition))));
                float offset = std::abs(pos->x - hpos->x);
                if(offset >= (float)(GRID_SQUARE_SIZE - MAX_MOVE_OFFSET) && offset <= (float)(GRID_SQUARE_SIZE + MAX_MOVE_OFFSET))
                {
                    if(pos->x > hpos->x)
                    {
                        pos->x = hpos->x + (float)GRID_SQUARE_SIZE;
                    }
                    else
                    {
                        pos->x = hpos->x - (float)GRID_SQUARE_SIZE;
                    }
                }
                else if(offset <= MOVE_SNAP_OFFSET)
                {
                    sf::Vector2f v = Utility::alignToGrid(pos->x, pos->y);
                    pos->x = v.x;
                    pos->y = v.y;
                }
                else
                {
                    pos->y = prev;
                }
            }
            else if(info.hit.empty() && infoBreak.hit.size() == 1)
            {
                cPosition* hpos = static_cast<cPosition*>(infoBreak.hit.front()->getComponent(std::type_index(typeid(cPosition))));
                float offset = std::abs(pos->x - hpos->x);
                if(offset <= MOVE_SNAP_OFFSET)
                {
                    sf::Vector2f v = Utility::alignToGrid(pos->x, pos->y);
                    pos->x = v.x;
                    pos->y = v.y;
                }
            }
            else
            {
                if(!isBalloon && living && living->kickUpgrade > 0 && infoBalloon.hit.size() == 1)
                {
                    context.sfxContext->happened[SoundContext::BALLOON_KICKED] = true;
                    if(prev < pos->y)
                    {
                        static_cast<cVelocity*>(infoBalloon.hit.front()->getComponent(std::type_index(typeid(cVelocity))))->y = BALLOON_KICK_SPEED;
                    }
                    else
                    {
                        static_cast<cVelocity*>(infoBalloon.hit.front()->getComponent(std::type_index(typeid(cVelocity))))->y = -BALLOON_KICK_SPEED;
                    }
                }
                pos->y = prev;
            }
        }
    }
}
