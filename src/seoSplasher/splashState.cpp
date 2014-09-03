
#include "splashState.hpp"

#include <ctime>

#include "nBalloon.hpp"
#include "nPControl.hpp"
#include "nMove.hpp"
#include "nDraw.hpp"
#include "../resourceManager.hpp"
#include "../ec/engine.hpp"
#include "../ec/entity.hpp"
#include "../ec/cPosition.hpp"
#include "cSprite.hpp"
#include "cWall.hpp"
#include "cLiving.hpp"
#include "cPlayerControl.hpp"
#include "cAIControl.hpp"
#include "nSplosion.hpp"
#include "nAnimated.hpp"
#include "cBreakable.hpp"
#include "gridInfo.hpp"
#include "nBreakable.hpp"


SplashState::SplashState(StateStack& stack, Context context) :
State(stack, context),
fieldBG(sf::Vector2f(480.0f,480.0f)),
IDcounter(0),
dir(cPlayerControl::NONE),
placeBalloon(false),
placeAction(false),
kick(false),
kickAction(false),
wUp(false),
aLeft(false),
sDown(false),
dRight(false),
gen((unsigned int)std::time(NULL)),
cFired(false)
{
    // resources
    tset.insert(Textures::WALL);
    tset.insert(Textures::BREAKABLE);
    tset.insert(Textures::PLAYER_ONE);
    tset.insert(Textures::BALLOON_0);
    tset.insert(Textures::BALLOON_1);
    tset.insert(Textures::BALLOON_2);
    tset.insert(Textures::SUPER_BALLOON_0);
    tset.insert(Textures::SUPER_BALLOON_1);
    tset.insert(Textures::SUPER_BALLOON_2);
    tset.insert(Textures::SPLOSION_PLUS);
    tset.insert(Textures::SPLOSION_VERT);
    tset.insert(Textures::SPLOSION_HORIZ);
    tset.insert(Textures::BALLOON_UP_0);
    tset.insert(Textures::BALLOON_UP_1);
    tset.insert(Textures::BALLOON_UP_2);
    tset.insert(Textures::RANGE_UP_0);
    tset.insert(Textures::RANGE_UP_1);
    tset.insert(Textures::RANGE_UP_2);
    tset.insert(Textures::SPEED_UP_0);
    tset.insert(Textures::SPEED_UP_1);
    tset.insert(Textures::SPEED_UP_2);
    tset.insert(Textures::KICK_UPGRADE_0);
    tset.insert(Textures::KICK_UPGRADE_1);
    tset.insert(Textures::KICK_UPGRADE_2);
    tset.insert(Textures::RCONTROL_UPGRADE_0);
    tset.insert(Textures::RCONTROL_UPGRADE_1);
    tset.insert(Textures::RCONTROL_UPGRADE_2);
    tset.insert(Textures::SBALLOON_UPGRADE_0);
    tset.insert(Textures::SBALLOON_UPGRADE_1);
    tset.insert(Textures::SBALLOON_UPGRADE_2);
    tset.insert(Textures::PIERCE_UPGRADE_0);
    tset.insert(Textures::PIERCE_UPGRADE_1);
    tset.insert(Textures::PIERCE_UPGRADE_2);
    tset.insert(Textures::SPREAD_UPGRADE_0);
    tset.insert(Textures::SPREAD_UPGRADE_1);
    tset.insert(Textures::SPREAD_UPGRADE_2);
    tset.insert(Textures::GHOST_UPGRADE_0);
    tset.insert(Textures::GHOST_UPGRADE_1);
    tset.insert(Textures::GHOST_UPGRADE_2);

    context.resourceManager->loadResources(getNeededResources());


    // add systems
    context.ecEngine->addSystem(std::unique_ptr<Node>(new nPControl));
    context.ecEngine->addSystem(std::unique_ptr<Node>(new nMove));
    context.ecEngine->addSystem(std::unique_ptr<Node>(new nBalloon));
    context.ecEngine->addSystem(std::unique_ptr<Node>(new nSplosion));
    context.ecEngine->addSystem(std::unique_ptr<Node>(new nAnimated));
    context.ecEngine->addSystem(std::unique_ptr<Node>(new nBreakable));
    context.ecEngine->addDrawSystem(std::unique_ptr<Node>(new nDraw));

    // add entities
    for(int i = 0; i < 15; ++i)
    {
        addWall(88.0f, 32.0f * (float)i);
        addWall(600.0f, 32.0f * (float)i);
        addWall(120.0f + 32.0f * (float)i, -32.0f);
        addWall(120.0f + 32.0f * (float)i, 480.0f);
        if(i < 7)
        {
            for(int j = 0; j < 7; ++j)
            {
                addWall(152.0f + 64.0f * (float)i, 32.0f + 64.0f * (float)j);
            }
        }
    }

    addCombatant(true);

    initBreakables();

    // other initializations
    fieldBG.setFillColor(sf::Color(127,127,127));
    fieldBG.setPosition(120.0f, 0.0f);
}

void SplashState::draw()
{
    getContext().window->draw(fieldBG);

    getContext().ecEngine->draw(getContext());
}

bool SplashState::update(sf::Time dt)
{
    getContext().ecEngine->update(dt, getContext());

    return false;
}

bool SplashState::handleEvent(const sf::Event& event)
{
    if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::W)
    {
        dir = cPlayerControl::UP;
        wUp = true;
    }
    else if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::A)
    {
        dir = cPlayerControl::LEFT;
        aLeft = true;
    }
    else if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::S)
    {
        dir = cPlayerControl::DOWN;
        sDown = true;
    }
    else if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::D)
    {
        dir = cPlayerControl::RIGHT;
        dRight = true;
    }
    else if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::E)
    {
        cFired = true;
    }
    else if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space)
    {
        placeBalloon = true;
        placeAction = false;
    }
    else if(event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::W)
    {
        wUp = false;
        checkReleasedInput();
    }
    else if(event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::A)
    {
        aLeft = false;
        checkReleasedInput();
    }
    else if(event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::S)
    {
        sDown = false;
        checkReleasedInput();
    }
    else if(event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::D)
    {
        dRight = false;
        checkReleasedInput();
    }
    else if(event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::E)
    {
        cFired = false;
    }
    return false;
}

void SplashState::addWall(float x, float y)
{
    Entity* wall = new Entity;

    cPosition* pos = new cPosition;
    pos->x = x;
    pos->y = y;
    wall->addComponent(std::type_index(typeid(cPosition)), std::unique_ptr<Component>(pos));

    cSprite* sprite = new cSprite;
    sprite->sprite.setTexture(getContext().resourceManager->getTexture(Textures::WALL));
    wall->addComponent(std::type_index(typeid(cSprite)), std::unique_ptr<Component>(sprite));

    cWall* cwall = new cWall;
    wall->addComponent(std::type_index(typeid(cWall)), std::unique_ptr<Component>(cwall));

    getContext().ecEngine->addEntity(std::unique_ptr<Entity>(wall));
}

void SplashState::addCombatant(bool isPlayer)
{
    unsigned int ID = IDcounter++;

    Entity* combatant = new Entity;

    cPosition* pos = new cPosition;
    switch(ID)
    {
    case 0:
        pos->x = 120.0f;
        pos->y = 0.0f;
        pos->rot = 0.0f;
        break;
    case 1:
        pos->x = 568.0f;
        pos->y = 0.0f;
        pos->rot = 0.0f;
        break;
    case 2:
        pos->x = 120.0f;
        pos->y = 448.0f;
        pos->rot = 0.0f;
        break;
    case 3:
        pos->x = 568.0f;
        pos->y = 448.0f;
        pos->rot = 0.0f;
        break;
    default:
        delete pos;
        delete combatant;
        return;
    }
    combatant->addComponent(std::type_index(typeid(cPosition)), std::unique_ptr<Component>(pos));

    cVelocity* vel = new cVelocity;
    vel->x = 0.0f;
    vel->y = 0.0f;
    vel->rot = 0.0f;
    combatant->addComponent(std::type_index(typeid(cVelocity)), std::unique_ptr<Component>(vel));

    cLiving* living = new cLiving;
    living->ID = ID;
    combatant->addComponent(std::type_index(typeid(cLiving)), std::unique_ptr<Component>(living));

    cSprite* sprite = new cSprite;
    sprite->sprite.setTexture(getContext().resourceManager->getTexture(Textures::PLAYER_ONE));
    combatant->addComponent(std::type_index(typeid(cSprite)), std::unique_ptr<Component>(sprite));
    if(isPlayer)
    {
        combatant->addComponent(std::type_index(typeid(cPlayerControl)), std::unique_ptr<Component>(new cPlayerControl(&dir, &placeBalloon, &placeAction, &kick, &kickAction, ID, &cFired)));
    }
    else
    {
        cAIControl* control = new cAIControl;
        control->ID = ID;
        combatant->addComponent(std::type_index(typeid(cAIControl)), std::unique_ptr<Component>(control));
    }

    getContext().ecEngine->addEntity(std::unique_ptr<Entity>(combatant));
}

void SplashState::checkReleasedInput()
{
    if(wUp || aLeft || sDown || dRight)
    {
        if(wUp)
        {
            dir = cPlayerControl::UP;
        }
        else if(aLeft)
        {
            dir = cPlayerControl::LEFT;
        }
        else if(sDown)
        {
            dir = cPlayerControl::DOWN;
        }
        else if(dRight)
        {
            dir = cPlayerControl::RIGHT;
        }
    }
    else
    {
        dir = cPlayerControl::NONE;
    }
}

void SplashState::addBreakable(float x, float y, cPowerup::Powerup powerup)
{
    Entity* breakable = new Entity;

    cPosition* pos = new cPosition;
    pos->x = x;
    pos->y = y;
    pos->rot = 0.0f;
    breakable->addComponent(std::type_index(typeid(cPosition)), std::unique_ptr<Component>(pos));

    cPowerup* cpowerup = new cPowerup;
    cpowerup->powerup = powerup;
    breakable->addComponent(std::type_index(typeid(cPowerup)), std::unique_ptr<Component>(cpowerup));

    breakable->addComponent(std::type_index(typeid(cBreakable)), std::unique_ptr<Component>(new cBreakable));

    cSprite* sprite = new cSprite;
    sprite->sprite.setTexture(getContext().resourceManager->getTexture(Textures::BREAKABLE));
    breakable->addComponent(std::type_index(typeid(cSprite)), std::unique_ptr<Component>(sprite));

    getContext().ecEngine->addEntity(std::unique_ptr<Entity>(breakable));
}

void SplashState::initBreakables()
{
    unsigned char powerups;

    std::list<sf::Vector2i> validPoints;
    for(int i = 0; i < GRID_WIDTH; ++i)
    {
        for(int j = 0; j < GRID_HEIGHT; ++j)
        {
            if(validBreakableCoordinate(i,j))
                validPoints.push_back(sf::Vector2i(i,j));
        }
    }

    unsigned char maxValid = validPoints.size();
    sf::Vector2i v;
    unsigned int r;
    // balloon up
    for(powerups = NUM_BALLOON_UP; powerups > 0; --powerups)
    {
        std::uniform_int_distribution<> dist(0,validPoints.size() - 1);
        auto iter = validPoints.begin();
        for(r = dist(gen); r != 0; ++iter)
        {
            --r;
        }
        v = *iter;
        validPoints.erase(iter);

        addBreakable(GRID_OFFSET_X + (float)(v.x * GRID_SQUARE_SIZE), GRID_OFFSET_Y + (float)(v.y * GRID_SQUARE_SIZE), cPowerup::BALLOON_UP);
    }

    // range up
    for(powerups = NUM_RANGE_UP; powerups > 0; --powerups)
    {
        std::uniform_int_distribution<> dist(0,validPoints.size() - 1);
        auto iter = validPoints.begin();
        for(r = dist(gen); r != 0; ++iter)
        {
            --r;
        }
        v = *iter;
        validPoints.erase(iter);

        addBreakable(GRID_OFFSET_X + (float)(v.x * GRID_SQUARE_SIZE), GRID_OFFSET_Y + (float)(v.y * GRID_SQUARE_SIZE), cPowerup::RANGE_UP);
    }

    // speed up
    for(powerups = NUM_SPEED_UP; powerups > 0; --powerups)
    {
        std::uniform_int_distribution<> dist(0,validPoints.size() - 1);
        auto iter = validPoints.begin();
        for(r = dist(gen); r != 0; ++iter)
        {
            --r;
        }
        v = *iter;
        validPoints.erase(iter);

        addBreakable(GRID_OFFSET_X + (float)(v.x * GRID_SQUARE_SIZE), GRID_OFFSET_Y + (float)(v.y * GRID_SQUARE_SIZE), cPowerup::SPEED_UP);
    }

    // kick upgrade
    for(powerups = NUM_KICK_UPGRADE; powerups > 0; --powerups)
    {
        std::uniform_int_distribution<> dist(0,validPoints.size() - 1);
        auto iter = validPoints.begin();
        for(r = dist(gen); r != 0; ++iter)
        {
            --r;
        }
        v = *iter;
        validPoints.erase(iter);

        addBreakable(GRID_OFFSET_X + (float)(v.x * GRID_SQUARE_SIZE), GRID_OFFSET_Y + (float)(v.y * GRID_SQUARE_SIZE), cPowerup::KICK_UPGRADE);
    }

    // remote control
    for(powerups = NUM_RCONTROL_UPGRADE; powerups > 0; --powerups)
    {
        std::uniform_int_distribution<> dist(0,validPoints.size() - 1);
        auto iter = validPoints.begin();
        for(r = dist(gen); r != 0; ++iter)
        {
            --r;
        }
        v = *iter;
        validPoints.erase(iter);

        addBreakable(GRID_OFFSET_X + (float)(v.x * GRID_SQUARE_SIZE), GRID_OFFSET_Y + (float)(v.y * GRID_SQUARE_SIZE), cPowerup::RCONTROL_UPGRADE);
    }

    // super balloon
    for(powerups = NUM_SBALLOON_UPGRADE; powerups > 0; --powerups)
    {
        std::uniform_int_distribution<> dist(0,validPoints.size() - 1);
        auto iter = validPoints.begin();
        for(r = dist(gen); r != 0; ++iter)
        {
            --r;
        }
        v = *iter;
        validPoints.erase(iter);

        addBreakable(GRID_OFFSET_X + (float)(v.x * GRID_SQUARE_SIZE), GRID_OFFSET_Y + (float)(v.y * GRID_SQUARE_SIZE), cPowerup::SBALLOON_UPGRADE);
    }

    // pierce upgrade
    for(powerups = NUM_PIERCE_UPGRADE; powerups > 0; --powerups)
    {
        std::uniform_int_distribution<> dist(0,validPoints.size() - 1);
        auto iter = validPoints.begin();
        for(r = dist(gen); r != 0; ++iter)
        {
            --r;
        }
        v = *iter;
        validPoints.erase(iter);

        addBreakable(GRID_OFFSET_X + (float)(v.x * GRID_SQUARE_SIZE), GRID_OFFSET_Y + (float)(v.y * GRID_SQUARE_SIZE), cPowerup::PIERCE_UPGRADE);
    }

    // spread upgrade
    for(powerups = NUM_SPREAD_UPGRADE; powerups > 0; --powerups)
    {
        std::uniform_int_distribution<> dist(0,validPoints.size() - 1);
        auto iter = validPoints.begin();
        for(r = dist(gen); r != 0; ++iter)
        {
            --r;
        }
        v = *iter;
        validPoints.erase(iter);

        addBreakable(GRID_OFFSET_X + (float)(v.x * GRID_SQUARE_SIZE), GRID_OFFSET_Y + (float)(v.y * GRID_SQUARE_SIZE), cPowerup::SPREAD_UPGRADE);
    }

    // ghost upgrade
    for(powerups = NUM_GHOST_UPGRADE; powerups > 0; --powerups)
    {
        std::uniform_int_distribution<> dist(0,validPoints.size() - 1);
        auto iter = validPoints.begin();
        for(r = dist(gen); r != 0; ++iter)
        {
            --r;
        }
        v = *iter;
        validPoints.erase(iter);

        addBreakable(GRID_OFFSET_X + (float)(v.x * GRID_SQUARE_SIZE), GRID_OFFSET_Y + (float)(v.y * GRID_SQUARE_SIZE), cPowerup::GHOST_UPGRADE);
    }

    int remaining = 100 - (maxValid - validPoints.size());
    for(; remaining > 0; --remaining)
    {
        std::uniform_int_distribution<> dist(0,validPoints.size() - 1);
        auto iter = validPoints.begin();
        for(r = dist(gen); r != 0; ++iter)
        {
            --r;
        }
        v = *iter;
        validPoints.erase(iter);

        addBreakable(GRID_OFFSET_X + (float)(v.x * GRID_SQUARE_SIZE), GRID_OFFSET_Y + (float)(v.y * GRID_SQUARE_SIZE));
    }
}

bool SplashState::validBreakableCoordinate(int x, int y)
{
    if((x == 0 && (y == 0 || y == 1 || y == GRID_HEIGHT - 2 || y == GRID_HEIGHT - 1)) ||
       (x == 1 && (y == 0 || y == GRID_HEIGHT - 1)) ||
       (x == GRID_WIDTH - 2 && (y == 0 || y == GRID_HEIGHT - 1)) ||
       (x == GRID_WIDTH - 1 && (y == 0 || y == 1 || y == GRID_HEIGHT - 2 || y == GRID_HEIGHT - 1)) ||
       (x % 2 == 1 && y % 2 == 1))
        return false;

    return true;
}
