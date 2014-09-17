
#include "splashMenu.hpp"

#include "../context.hpp"
#include "../resourceManager.hpp"

SplashMenu::SplashMenu(StateStack& stack, Context context) :
State(stack, context),
guiSystem(this),
singlePlayer(false)
{
    // load needed resources
    fset.insert(Fonts::CLEAR_SANS);

    context.resourceManager->loadResources(getNeededResources());

    // setup gui
    const sf::Font& cSans = context.resourceManager->getFont(Fonts::CLEAR_SANS);

    GuiButton* singlePlayerButton = new GuiButton(context.window, GuiCommand(GuiCommand::VALUE_BOOL, GuiCommand::Value(false), GuiCommand::Ptr(&singlePlayer)), sf::Color(0,127,0), sf::Color::Green, sf::Vector2f(120.0f, 30.0f), sf::Text("SinglePlayer", cSans, 18));

    singlePlayerButton->setPosition((720.0f - 120.0f) / 2.0f, 220.0f);

    guiSystem.add(singlePlayerButton);
}

void SplashMenu::draw()
{
    guiSystem.draw(*getContext().window);
}

bool SplashMenu::update(sf::Time dt)
{
    guiSystem.update(dt);

    if(singlePlayer)
    {
        *getContext().mode = 0;
        requestStackClear();
        requestStackPush(States::SPLASH);
    }

    return false;
}

bool SplashMenu::handleEvent(const sf::Event& event)
{
    guiSystem.processEvent(event);

    return false;
}
