
#include "splashState.hpp"

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
dRight(false)
{
    // resources
    tset.insert(Textures::WALL);
    tset.insert(Textures::BREAKABLE);
    tset.insert(Textures::PLAYER_ONE);
    tset.insert(Textures::BALLOON_0);
    tset.insert(Textures::SUPER_BALLOON_0);

    context.resourceManager->loadResources(getNeededResources());


    // add systems
    context.ecEngine->addSystem(std::unique_ptr<Node>(new nPControl));
    context.ecEngine->addSystem(std::unique_ptr<Node>(new nMove));
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
        combatant->addComponent(std::type_index(typeid(cPlayerControl)), std::unique_ptr<Component>(new cPlayerControl(&dir, &placeBalloon, &placeAction, &kick, &kickAction, ID)));
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
