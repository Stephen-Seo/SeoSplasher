
#include "nBalloon.hpp"

#include <typeindex>

#include "../ec/cPosition.hpp"
#include "cBalloon.hpp"
#include "cControl.hpp"
#include "cTimer.hpp"
#include "../context.hpp"
#include "../ec/engine.hpp"
#include "utility.hpp"
#include "cPathFinderRef.hpp"
#include "AI/pathFinder.hpp"
#include "cPathFinder.hpp"

nBalloon::nBalloon() :
pos(nullptr),
balloon(nullptr),
control(nullptr),
timer(nullptr),
pfRef(nullptr)
{}

bool nBalloon::checkEntity(Entity& entity)
{
    return entity.hasComponent(std::type_index(typeid(cPosition))) &&
           entity.hasComponent(std::type_index(typeid(cBalloon))) &&
           entity.hasComponent(std::type_index(typeid(cPathFinderRef))) &&
          (entity.hasComponent(std::type_index(typeid(cControl))) ||
           entity.hasComponent(std::type_index(typeid(cTimer))));
}

std::unique_ptr<Node> nBalloon::getNewNode()
{
    return std::unique_ptr<Node>(new nBalloon());
}

void nBalloon::getCReferencesFromEntity(Entity& entity)
{
    pos = static_cast<cPosition*>(entity.getComponent(std::type_index(typeid(cPosition))));
    balloon = static_cast<cBalloon*>(entity.getComponent(std::type_index(typeid(cBalloon))));
    pfRef = static_cast<cPathFinderRef*>(entity.getComponent(std::type_index(typeid(cPathFinderRef))));
    if(entity.hasComponent(std::type_index(typeid(cControl))))
    {
        control = static_cast<cControl*>(entity.getComponent(std::type_index(typeid(cControl))));
    }
    else
    {
        timer = static_cast<cTimer*>(entity.getComponent(std::type_index(typeid(cTimer))));
    }
    entityRemoved = &entity.removed;

    ID = entity.getID();
}

void nBalloon::update(sf::Time dt, Context context)
{
    if(*entityRemoved)
        return;

    if(balloon->hit)
    {
        --(*balloon->balloonsInPlay);
        if(balloon->super)
            --(*balloon->sBalloonsInPlay);
        context.ecEngine->removeEntity(ID);
        generateExplosions(context);
    }
    else if(control)
    {
        if(*control->fired)
        {
            --(*balloon->balloonsInPlay);
            if(balloon->super)
                --(*balloon->sBalloonsInPlay);
            context.ecEngine->removeEntity(ID);
            generateExplosions(context);
        }
    }
    else
    {
        timer->time -= dt.asSeconds();
        if(timer->time <= 0.0F)
        {
            --(*balloon->balloonsInPlay);
            if(balloon->super)
                --(*balloon->sBalloonsInPlay);
            context.ecEngine->removeEntity(ID);
            generateExplosions(context);
        }
    }
}

void nBalloon::generateExplosions(Context context)
{
    sf::Vector2f v = Utility::alignToGrid(pos->x, pos->y);
    pfRef->cpf->pf.invalidateValidGrid();
    const unsigned char* grid = pfRef->cpf->pf.getValidGrid(*context.ecEngine);
    int xy = (int)((v.x - (float)GRID_OFFSET_X) / GRID_SQUARE_SIZE) + (int)((v.y - (float)GRID_OFFSET_Y) / GRID_SQUARE_SIZE) * GRID_WIDTH;

    Utility::createExplosion(v.x, v.y, Direction::PLUS, context, balloon->ID);

    int distance = balloon->range;
    for(int i = xy - 1; (i + 1) % GRID_WIDTH != 0; --i)
    {
        if(!balloon->ghosting && (grid[i] & 0x10) != 0)
            break;
        Utility::createExplosion(v.x - (float)(xy - i) * GRID_SQUARE_SIZE, v.y, Direction::HORIZONTAL, context, balloon->ID);
        if(--distance == 0 || (!balloon->piercing && (grid[i] & 0x4) != 0))
            break;
    }

    distance = balloon->range;
    for(int i = xy + 1; (i - 1) % GRID_WIDTH != GRID_WIDTH - 1; ++i)
    {
        if(!balloon->ghosting && (grid[i] & 0x10) != 0)
            break;
        Utility::createExplosion(v.x + (float)(i - xy) * GRID_SQUARE_SIZE, v.y, Direction::HORIZONTAL, context, balloon->ID);
        if(--distance == 0 || (!balloon->piercing && (grid[i] & 0x4) != 0))
            break;
    }

    distance = balloon->range;
    for(int i = xy - GRID_WIDTH; i >= 0; i -= GRID_WIDTH)
    {
        if(!balloon->ghosting && (grid[i] & 0x10) != 0)
            break;
        Utility::createExplosion(v.x, v.y - (float)((int)(xy / GRID_WIDTH) - (int)(i / GRID_WIDTH)) * GRID_SQUARE_SIZE, Direction::VERTICAL, context, balloon->ID);
        if(--distance == 0 || (!balloon->piercing && (grid[i] & 0x4) != 0))
            break;
    }

    distance = balloon->range;
    for(int i = xy + GRID_WIDTH; i < GRID_TOTAL; i += GRID_WIDTH)
    {
        if(!balloon->ghosting && (grid[i] & 0x10) != 0)
            break;
        Utility::createExplosion(v.x, v.y + (float)((int)(i / GRID_WIDTH) - (int)(xy / GRID_WIDTH)) * GRID_SQUARE_SIZE, Direction::VERTICAL, context, balloon->ID);
        if(--distance == 0 || (!balloon->piercing && (grid[i] & 0x4) != 0))
            break;
    }
}
