
#include "nDeath.hpp"

#include <typeindex>

#include "cPowerup.hpp"
#include "cLiving.hpp"
#include "utility.hpp"
#include "gridInfo.hpp"
#include "../context.hpp"
#include "../ec/engine.hpp"

nDeath::nDeath() :
living(nullptr)
{}

bool nDeath::checkEntity(Entity& entity)
{
    return entity.hasComponent(std::type_index(typeid(cLiving)));
}

std::unique_ptr<Node> nDeath::getNewNode()
{
    return std::unique_ptr<Node>(new nDeath);
}

void nDeath::getCReferencesFromEntity(Entity& entity)
{
    living = static_cast<cLiving*>(entity.getComponent(std::type_index(typeid(cLiving))));
    entityRemoved = &entity.removed;
    ID = entity.getID();
}

void nDeath::update(sf::Time dt, Context context)
{
    if(*entityRemoved)
        return;

    if(living->hit)
    {
        if(living->balloonUp + living->rangeUp + living->kickUpgrade + living->rControlUpgrade + living->sBalloonUpgrade + living->pierceUpgrade + living->spreadUpgrade + living->ghostUpgrade > 0)
        {
            std::list<sf::Vector2i> validPoints;
            for(int x = 0; x < GRID_WIDTH; ++x)
            {
                for(int y = 0; y < GRID_HEIGHT; ++y)
                {
                    if(!Utility::collidesAll((float)(GRID_OFFSET_X + x * GRID_SQUARE_SIZE), (float)(GRID_OFFSET_Y + y * GRID_SQUARE_SIZE), *context.ecEngine))
                    {
                        validPoints.push_back(sf::Vector2i(x,y));
                    }
                }
            }

            cPowerup powerup;
            // ballon up
            powerup.powerup = cPowerup::BALLOON_UP;
            for(int i = living->balloonUp; i > 0; --i)
            {
                if(validPoints.empty())
                    break;
                std::uniform_int_distribution<> dist(0, validPoints.size() - 1);
                auto iter = validPoints.begin();
                for(int j = dist(*context.rGen); j > 0; --j)
                {
                    ++iter;
                }
                Utility::createPowerup((float)(GRID_OFFSET_X + iter->x * GRID_SQUARE_SIZE), (float)(GRID_OFFSET_Y + iter->y * GRID_SQUARE_SIZE), powerup, context);
                validPoints.erase(iter);
            }

            // range up
            powerup.powerup = cPowerup::RANGE_UP;
            for(int i = living->rangeUp; i > 0; --i)
            {
                if(validPoints.empty())
                    break;
                std::uniform_int_distribution<> dist(0, validPoints.size() - 1);
                auto iter = validPoints.begin();
                for(int j = dist(*context.rGen); j > 0; --j)
                {
                    ++iter;
                }
                Utility::createPowerup((float)(GRID_OFFSET_X + iter->x * GRID_SQUARE_SIZE), (float)(GRID_OFFSET_Y + iter->y * GRID_SQUARE_SIZE), powerup, context);
                validPoints.erase(iter);
            }

            // speed up
            powerup.powerup = cPowerup::SPEED_UP;
            for(int i = living->speedUp; i > 0; --i)
            {
                if(validPoints.empty())
                    break;
                std::uniform_int_distribution<> dist(0, validPoints.size() - 1);
                auto iter = validPoints.begin();
                for(int j = dist(*context.rGen); j > 0; --j)
                {
                    ++iter;
                }
                Utility::createPowerup((float)(GRID_OFFSET_X + iter->x * GRID_SQUARE_SIZE), (float)(GRID_OFFSET_Y + iter->y * GRID_SQUARE_SIZE), powerup, context);
                validPoints.erase(iter);
            }

            // kick upgrade
            powerup.powerup = cPowerup::KICK_UPGRADE;
            for(int i = living->kickUpgrade; i > 0; --i)
            {
                if(validPoints.empty())
                    break;
                std::uniform_int_distribution<> dist(0, validPoints.size() - 1);
                auto iter = validPoints.begin();
                for(int j = dist(*context.rGen); j > 0; --j)
                {
                    ++iter;
                }
                Utility::createPowerup((float)(GRID_OFFSET_X + iter->x * GRID_SQUARE_SIZE), (float)(GRID_OFFSET_Y + iter->y * GRID_SQUARE_SIZE), powerup, context);
                validPoints.erase(iter);
            }

            // rcontrol upgrade
            powerup.powerup = cPowerup::RCONTROL_UPGRADE;
            for(int i = living->rControlUpgrade; i > 0; --i)
            {
                if(validPoints.empty())
                    break;
                std::uniform_int_distribution<> dist(0, validPoints.size() - 1);
                auto iter = validPoints.begin();
                for(int j = dist(*context.rGen); j > 0; --j)
                {
                    ++iter;
                }
                Utility::createPowerup((float)(GRID_OFFSET_X + iter->x * GRID_SQUARE_SIZE), (float)(GRID_OFFSET_Y + iter->y * GRID_SQUARE_SIZE), powerup, context);
                validPoints.erase(iter);
            }

            // sBalloon upgrade
            powerup.powerup = cPowerup::SBALLOON_UPGRADE;
            for(int i = living->sBalloonUpgrade; i > 0; --i)
            {
                if(validPoints.empty())
                    break;
                std::uniform_int_distribution<> dist(0, validPoints.size() - 1);
                auto iter = validPoints.begin();
                for(int j = dist(*context.rGen); j > 0; --j)
                {
                    ++iter;
                }
                Utility::createPowerup((float)(GRID_OFFSET_X + iter->x * GRID_SQUARE_SIZE), (float)(GRID_OFFSET_Y + iter->y * GRID_SQUARE_SIZE), powerup, context);
                validPoints.erase(iter);
            }

            // pierce upgrade
            powerup.powerup = cPowerup::PIERCE_UPGRADE;
            for(int i = living->pierceUpgrade; i > 0; --i)
            {
                if(validPoints.empty())
                    break;
                std::uniform_int_distribution<> dist(0, validPoints.size() - 1);
                auto iter = validPoints.begin();
                for(int j = dist(*context.rGen); j > 0; --j)
                {
                    ++iter;
                }
                Utility::createPowerup((float)(GRID_OFFSET_X + iter->x * GRID_SQUARE_SIZE), (float)(GRID_OFFSET_Y + iter->y * GRID_SQUARE_SIZE), powerup, context);
                validPoints.erase(iter);
            }

            // spread upgrade
            powerup.powerup = cPowerup::SPREAD_UPGRADE;
            for(int i = living->spreadUpgrade; i > 0; --i)
            {
                if(validPoints.empty())
                    break;
                std::uniform_int_distribution<> dist(0, validPoints.size() - 1);
                auto iter = validPoints.begin();
                for(int j = dist(*context.rGen); j > 0; --j)
                {
                    ++iter;
                }
                Utility::createPowerup((float)(GRID_OFFSET_X + iter->x * GRID_SQUARE_SIZE), (float)(GRID_OFFSET_Y + iter->y * GRID_SQUARE_SIZE), powerup, context);
                validPoints.erase(iter);
            }

            // ghost upgrade
            powerup.powerup = cPowerup::GHOST_UPGRADE;
            for(int i = living->ghostUpgrade; i > 0; --i)
            {
                if(validPoints.empty())
                    break;
                std::uniform_int_distribution<> dist(0, validPoints.size() - 1);
                auto iter = validPoints.begin();
                for(int j = dist(*context.rGen); j > 0; --j)
                {
                    ++iter;
                }
                Utility::createPowerup((float)(GRID_OFFSET_X + iter->x * GRID_SQUARE_SIZE), (float)(GRID_OFFSET_Y + iter->y * GRID_SQUARE_SIZE), powerup, context);
                validPoints.erase(iter);
            }
        }

        context.ecEngine->removeEntity(ID);
    }
}
