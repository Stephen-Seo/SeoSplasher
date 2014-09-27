
#include "splashMenu.hpp"

#include "../context.hpp"
#include "../resourceManager.hpp"
#include "splashNetworkIdentifiers.hpp"
#include "utility.hpp"

SplashMenu::SplashMenu(StateStack& stack, Context context) :
State(stack, context),
guiSystem(this),
singlePlayer(false),
client(false),
server(false)
{
    // load needed resources
    fset.insert(Fonts::CLEAR_SANS);

    context.resourceManager->loadResources(getNeededResources());

    // setup gui
    const sf::Font& cSans = context.resourceManager->getFont(Fonts::CLEAR_SANS);

    GuiButton* singlePlayerButton = new GuiButton(context.window, GuiCommand(GuiCommand::VALUE_BOOL, GuiCommand::Value(false), GuiCommand::Ptr(&singlePlayer)), sf::Color(0,192,0), sf::Color::Green, sf::Vector2f(120.0f, 30.0f), sf::Text("SinglePlayer", cSans, 18));

    singlePlayerButton->setPosition((720.0f - 120.0f) / 2.0f, 220.0f);

    guiSystem.add(singlePlayerButton);

    GuiButton* clientConnectButton = new GuiButton(context.window, GuiCommand(GuiCommand::VALUE_BOOL, GuiCommand::Value(false), GuiCommand::Ptr(&client)), sf::Color(0,127,0), sf::Color::Green, sf::Vector2f(120.0f, 30.0f), sf::Text("Multiplayer", cSans, 18));

    clientConnectButton->setPosition((720.0f - 120.0f) / 2.0f, 270.0f);

    guiSystem.add(clientConnectButton);

    GuiButton* serverButton = new GuiButton(context.window, GuiCommand(GuiCommand::VALUE_BOOL, GuiCommand::Value(false), GuiCommand::Ptr(&server)), sf::Color(0,127,0), sf::Color::Green, sf::Vector2f(120.0f, 30.0f), sf::Text("Host a Game", cSans, 18));

    serverButton->setPosition((720.0f - 120.0f) / 2.0f, 320.0f);

    guiSystem.add(serverButton);

    IPText.setString("");
    IPText.setCharacterSize(30);
    IPText.setFont(cSans);
    IPText.setColor(sf::Color(255,240,240));
    IPText.setPosition(360.0f, 450.0f);
}

void SplashMenu::draw()
{
    guiSystem.draw(*getContext().window);

    getContext().window->draw(IPText);
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
    else if(client)
    {
        *getContext().mode = 1;
        requestStackClear();
        requestStackPush(States::SPLASH);
    }
    else if(server)
    {
        *getContext().mode = 2;
        requestStackClear();
        requestStackPush(States::SPLASH);
    }

    return false;
}

bool SplashMenu::handleEvent(const sf::Event& event)
{
    guiSystem.processEvent(event);

    if(event.type == sf::Event::TextEntered && ((event.text.unicode >= 48 && event.text.unicode <= 57) || event.text.unicode == 46))
    {
        getContext().scontext->serverIP.push_back((unsigned char) event.text.unicode);
        IPText.setString(getContext().scontext->serverIP);
        Utility::centerTextOrigin(IPText);
    }
    if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::BackSpace)
    {
        if(getContext().scontext->serverIP.size() > 0)
            getContext().scontext->serverIP.pop_back();
        IPText.setString(getContext().scontext->serverIP);
        Utility::centerTextOrigin(IPText);
    }

    return false;
}
