
#include "splashState.hpp"

#include "nMove.hpp"
#include "nDraw.hpp"
#include "../resourceManager.hpp"
#include "../ec/engine.hpp"
#include "../ec/entity.hpp"
#include "../ec/cPosition.hpp"
#include "cSprite.hpp"
#include "cWall.hpp"

SplashState::SplashState(StateStack& stack, Context context) :
State(stack, context),
fieldBG(sf::Vector2f(480.0f,480.0f))
{
    // resources
    tset.insert(Textures::WALL);

    context.resourceManager->loadResources(getNeededResources());


    // add systems
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
