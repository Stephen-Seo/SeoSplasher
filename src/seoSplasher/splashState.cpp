
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
#include "../ec/cVelocity.hpp"
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
#include "nDeath.hpp"
#include "cPathFinder.hpp"
#include "cPathFinderRef.hpp"
#include "nAIControl.hpp"
#include "nPFUpdater.hpp"
#include "splashServer.hpp"
#include "splashClient.hpp"
#include "nPickupHit.hpp"

SplashState::SplashState(StateStack& stack, Context context) :
State(stack, context),
fieldBG(sf::Vector2f(480.0f,480.0f)),
IDcounter(0),
dir(Direction::NONE),
placeBalloon(false),
placeAction(false),
kick(false),
kickAction(false),
wUp(false),
aLeft(false),
sDown(false),
dRight(false),
cFired(false),
cpf(nullptr)
{
    // set servers based on mode

    if(*context.mode == 2) // is server
    {
        server = std::unique_ptr<SplashServer>(new SplashServer(context, false));
    }
    else if(*context.mode == 3 || *context.mode == 4) // is dedicated server
    {
        server = std::unique_ptr<SplashServer>(new SplashServer(context, true));
    }
    else if(*context.mode == 1) // is client
    {
        client = std::unique_ptr<SplashClient>(new SplashClient(context));
    }

    // seed random generator
    {
        std::seed_seq seq{std::random_device()(),(unsigned int)std::time(NULL)};
        context.rGen->seed(seq);
    }

    // resources
    if(*context.mode != 4) // not no-draw mode
    {
        tset.insert(Textures::WALL);
        tset.insert(Textures::BREAKABLE);
        tset.insert(Textures::PLAYER_ONE);
        tset.insert(Textures::PLAYER_TWO);
        tset.insert(Textures::PLAYER_THREE);
        tset.insert(Textures::PLAYER_FOUR);
        tset.insert(Textures::C_PLAYER_ONE);
        tset.insert(Textures::C_PLAYER_TWO);
        tset.insert(Textures::C_PLAYER_THREE);
        tset.insert(Textures::C_PLAYER_FOUR);
        tset.insert(Textures::BALLOON_0);
        tset.insert(Textures::BALLOON_1);
        tset.insert(Textures::BALLOON_2);
        tset.insert(Textures::SUPER_BALLOON_0);
        tset.insert(Textures::SUPER_BALLOON_1);
        tset.insert(Textures::SUPER_BALLOON_2);
        tset.insert(Textures::C_BALLOON_0);
        tset.insert(Textures::C_BALLOON_1);
        tset.insert(Textures::C_BALLOON_2);
        tset.insert(Textures::C_SUPER_BALLOON_0);
        tset.insert(Textures::C_SUPER_BALLOON_1);
        tset.insert(Textures::C_SUPER_BALLOON_2);
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
    }

    // add systems

    context.ecEngine->addSystem(std::unique_ptr<Node>(new nPControl));
    context.ecEngine->addSystem(std::unique_ptr<Node>(new nMove));

    if(*context.mode != 1) // not client
    {
        context.ecEngine->addSystem(std::unique_ptr<Node>(new nAIControl));
        context.ecEngine->addSystem(std::unique_ptr<Node>(new nBalloon));
        context.ecEngine->addSystem(std::unique_ptr<Node>(new nSplosion));
        context.ecEngine->addSystem(std::unique_ptr<Node>(new nBreakable));
        context.ecEngine->addSystem(std::unique_ptr<Node>(new nDeath));
        context.ecEngine->addSystem(std::unique_ptr<Node>(new nPickupHit));
        context.ecEngine->addSystem(std::unique_ptr<Node>(new nPFUpdater));
    }

    if(*context.mode != 4) // is not no-draw mode
    {
        context.ecEngine->addSystem(std::unique_ptr<Node>(new nAnimated));
        context.ecEngine->addDrawSystem(std::unique_ptr<Node>(new nDraw));
    }

    // add entities
    addPathFinder();

    for(int i = 0; i < 15; ++i)
    {
        addWall(GRID_OFFSET_X - GRID_SQUARE_SIZE, GRID_OFFSET_Y + GRID_SQUARE_SIZE * i);
        addWall(GRID_RIGHT, GRID_OFFSET_Y + GRID_SQUARE_SIZE * i);
        addWall(GRID_OFFSET_X + GRID_SQUARE_SIZE * i, GRID_OFFSET_Y - GRID_SQUARE_SIZE);
        addWall(GRID_OFFSET_X + GRID_SQUARE_SIZE * i, GRID_BOTTOM);
        if(i < 7)
        {
            for(int j = 0; j < 7; ++j)
            {
                addWall(GRID_OFFSET_X + GRID_SQUARE_SIZE + GRID_SQUARE_SIZE * 2 * i, GRID_OFFSET_Y + GRID_SQUARE_SIZE + GRID_SQUARE_SIZE * 2 * j);
            }
        }
    }

    if(*context.mode == 0) // local singleplayer
    {
        addCombatant(true, true);
        addCombatant(false, false);
        addCombatant(false, false);
        addCombatant(false, false);
    }

    if(*context.mode != 1) // not client
        initBreakables();

    // other initializations
    fieldBG.setFillColor(sf::Color(127,127,127));
    fieldBG.setPosition(120.0f, 0.0f);
}

void SplashState::draw()
{
    if(*getContext().mode != 4) // not no-draw mode
    {
        getContext().window->draw(fieldBG);

        getContext().ecEngine->draw(getContext());
    }
}

bool SplashState::update(sf::Time dt)
{
    if(server)
    {
        server->update(dt);
    }
    else if(client)
    {
        client->update(dt);
    }
    getContext().ecEngine->update(dt, getContext());

    return false;
}

bool SplashState::handleEvent(const sf::Event& event)
{
    if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::W)
    {
        dir = Direction::UP;
        wUp = true;
    }
    else if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::A)
    {
        dir = Direction::LEFT;
        aLeft = true;
    }
    else if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::S)
    {
        dir = Direction::DOWN;
        sDown = true;
    }
    else if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::D)
    {
        dir = Direction::RIGHT;
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
    else if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
    {
        getContext().ecEngine->clear();
        requestStackClear();
        requestStackPush(States::MENU);
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

    if(*getContext().mode != 4)
    {
        cSprite* sprite = new cSprite;
        sprite->sprite.setTexture(getContext().resourceManager->getTexture(Textures::WALL));
        wall->addComponent(std::type_index(typeid(cSprite)), std::unique_ptr<Component>(sprite));
    }

    cWall* cwall = new cWall;
    wall->addComponent(std::type_index(typeid(cWall)), std::unique_ptr<Component>(cwall));

    getContext().ecEngine->addEntity(std::unique_ptr<Entity>(wall));
}

void SplashState::addCombatant(bool isPlayer, bool isPlayerLocallyControlled, int forceID)
{
    unsigned int ID;
    if(forceID >= 0 && forceID < 4)
        ID = forceID;
    else
        ID = IDcounter++;

    std::unique_ptr<Entity> combatant = std::unique_ptr<Entity>(new Entity);

    std::unique_ptr<Component> posP = std::unique_ptr<Component>(new cPosition);
    cPosition* pos = static_cast<cPosition*>(posP.get());

    std::unique_ptr<Component> spriteP;
    if(*getContext().mode != 4)
        spriteP = std::unique_ptr<Component>(new cSprite);

    switch(ID)
    {
    case 0:
        pos->x = 120.0f;
        pos->y = 0.0f;
        pos->rot = 0.0f;
        if(spriteP)
        {
            cSprite* sprite = static_cast<cSprite*>(spriteP.get());
            if(isPlayer)
                sprite->sprite.setTexture(getContext().resourceManager->getTexture(Textures::PLAYER_ONE));
            else
                sprite->sprite.setTexture(getContext().resourceManager->getTexture(Textures::C_PLAYER_ONE));
        }
        break;
    case 1:
        pos->x = 568.0f;
        pos->y = 0.0f;
        pos->rot = 0.0f;
        if(spriteP)
        {
            cSprite* sprite = static_cast<cSprite*>(spriteP.get());
            if(isPlayer)
                sprite->sprite.setTexture(getContext().resourceManager->getTexture(Textures::PLAYER_TWO));
            else
                sprite->sprite.setTexture(getContext().resourceManager->getTexture(Textures::C_PLAYER_TWO));
        }
        break;
    case 2:
        pos->x = 120.0f;
        pos->y = 448.0f;
        pos->rot = 0.0f;
        if(spriteP)
        {
            cSprite* sprite = static_cast<cSprite*>(spriteP.get());
            if(isPlayer)
                sprite->sprite.setTexture(getContext().resourceManager->getTexture(Textures::PLAYER_THREE));
            else
                sprite->sprite.setTexture(getContext().resourceManager->getTexture(Textures::C_PLAYER_THREE));
        }
        break;
    case 3:
        pos->x = 568.0f;
        pos->y = 448.0f;
        pos->rot = 0.0f;
        if(spriteP)
        {
            cSprite* sprite = static_cast<cSprite*>(spriteP.get());
            if(isPlayer)
                sprite->sprite.setTexture(getContext().resourceManager->getTexture(Textures::PLAYER_FOUR));
            else
                sprite->sprite.setTexture(getContext().resourceManager->getTexture(Textures::C_PLAYER_FOUR));
        }
        break;
    default:
        return;
    }

    playerIDToEntityID.insert(std::make_pair(ID, combatant->getID()));

    combatant->addComponent(std::type_index(typeid(cPosition)), std::move(posP));
    combatant->addComponent(std::type_index(typeid(cSprite)), std::move(spriteP));

    std::unique_ptr<Component> velP = std::unique_ptr<Component>(new cVelocity);
    cVelocity* vel = static_cast<cVelocity*>(velP.get());
    vel->x = 0.0f;
    vel->y = 0.0f;
    vel->rot = 0.0f;
    combatant->addComponent(std::type_index(typeid(cVelocity)), std::move(velP));

    std::unique_ptr<Component> livingP = std::unique_ptr<Component>(new cLiving);
    cLiving* living = static_cast<cLiving*>(livingP.get());
    living->ID = ID;
    combatant->addComponent(std::type_index(typeid(cLiving)), std::move(livingP));

    if(isPlayer && isPlayerLocallyControlled)
    {
        combatant->addComponent(std::type_index(typeid(cPlayerControl)), std::unique_ptr<Component>(new cPlayerControl(&dir, &placeBalloon, &placeAction, &kick, &kickAction, ID, &cFired)));
    }
    else if(!isPlayer && *getContext().mode != 1) // not client
    {
        std::unique_ptr<Component> controlP = std::unique_ptr<Component>(new cAIControl);
        cAIControl* control = static_cast<cAIControl*>(controlP.get());
        control->ID = ID;
        control->pf = &cpf->pf;
        combatant->addComponent(std::type_index(typeid(cAIControl)), std::move(controlP));
    }

    combatant->addComponent(std::type_index(typeid(cPathFinderRef)), std::unique_ptr<Component>(new cPathFinderRef(cpf)));

    getContext().ecEngine->addEntity(std::move(combatant));
}

void SplashState::addPathFinder()
{
    Entity* pfHolder = new Entity;

    cpf = new cPathFinder;
    pfHolder->addComponent(std::type_index(typeid(cPathFinder)), std::unique_ptr<Component>(cpf));

    getContext().ecEngine->addEntity(std::unique_ptr<Entity>(pfHolder));
}

void SplashState::checkReleasedInput()
{
    if(wUp || aLeft || sDown || dRight)
    {
        if(wUp)
        {
            dir = Direction::UP;
        }
        else if(aLeft)
        {
            dir = Direction::LEFT;
        }
        else if(sDown)
        {
            dir = Direction::DOWN;
        }
        else if(dRight)
        {
            dir = Direction::RIGHT;
        }
    }
    else
    {
        dir = Direction::NONE;
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
        for(r = dist(*getContext().rGen); r != 0; ++iter)
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
        for(r = dist(*getContext().rGen); r != 0; ++iter)
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
        for(r = dist(*getContext().rGen); r != 0; ++iter)
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
        for(r = dist(*getContext().rGen); r != 0; ++iter)
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
        for(r = dist(*getContext().rGen); r != 0; ++iter)
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
        for(r = dist(*getContext().rGen); r != 0; ++iter)
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
        for(r = dist(*getContext().rGen); r != 0; ++iter)
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
        for(r = dist(*getContext().rGen); r != 0; ++iter)
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
        for(r = dist(*getContext().rGen); r != 0; ++iter)
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
        for(r = dist(*getContext().rGen); r != 0; ++iter)
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
