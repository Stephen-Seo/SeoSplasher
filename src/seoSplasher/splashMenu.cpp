
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
server(false),
blinkTime(0.0F),
blinkInterval(0.0F),
blinking(false)
{
    // load needed resources
    fset.insert(Fonts::CLEAR_SANS);

    context.resourceManager->loadResources(getNeededResources());

    // setup gui
    const sf::Font& cSans = context.resourceManager->getFont(Fonts::CLEAR_SANS);

    GuiButton* singlePlayerButton = new GuiButton(context.window, GuiCommand(GuiCommand::VALUE_BOOL, GuiCommand::Value(false), GuiCommand::Ptr(&singlePlayer)), sf::Color(0,192,0), sf::Color::Green, sf::Vector2f(120.0F, 30.0F), sf::Text("SinglePlayer", cSans, 18));

    singlePlayerButton->setPosition((720.0F - 120.0F) / 2.0F, 220.0F);

    guiSystem.add(singlePlayerButton);

    GuiButton* clientConnectButton = new GuiButton(context.window, GuiCommand(GuiCommand::VALUE_BOOL, GuiCommand::Value(false), GuiCommand::Ptr(&client)), sf::Color(0,127,0), sf::Color::Green, sf::Vector2f(120.0F, 30.0F), sf::Text("Multiplayer", cSans, 18));

    clientConnectButton->setPosition((720.0F - 120.0F) / 2.0F, 270.0F);

    guiSystem.add(clientConnectButton);

    GuiButton* serverButton = new GuiButton(context.window, GuiCommand(GuiCommand::VALUE_BOOL, GuiCommand::Value(false), GuiCommand::Ptr(&server)), sf::Color(0,127,0), sf::Color::Green, sf::Vector2f(120.0F, 30.0F), sf::Text("Host a Game", cSans, 18));

    serverButton->setPosition((720.0F - 120.0F) / 2.0F, 320.0F);

    guiSystem.add(serverButton);

    IPText.setString("");
    IPText.setCharacterSize(30);
    IPText.setFont(cSans);
    IPText.setFillColor(sf::Color(220,255,220));
    IPText.setPosition(360.0F, 450.0F);

    infoIPText.setString("Start typing an ip address if you want to connect to a server.");
    infoIPText.setCharacterSize(15);
    infoIPText.setFont(cSans);
    infoIPText.setFillColor(sf::Color::White);
    infoIPText.setPosition(360.0F, 430.0F);
    Utility::centerTextOrigin(infoIPText);
}

void SplashMenu::draw()
{
    guiSystem.draw(*getContext().window);

    if(blinkTime == 0.0F || !blinking)
    {
        getContext().window->draw(infoIPText);
    }
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
        if(getContext().scontext->serverIP.empty())
        {
            client = false;
            blinkTime = BLINK_TIME;
            blinkInterval = BLINK_INTERVAL;
            blinking = true;
        }
        else
        {
            *getContext().mode = 1;
            requestStackClear();
            requestStackPush(States::SPLASH);
        }
    }
    else if(server)
    {
        *getContext().mode = 2;
        requestStackClear();
        requestStackPush(States::SPLASH);
    }

    if(blinkTime > 0.0F)
    {
        blinkTime -= dt.asSeconds();
        if(blinkTime < 0.0F)
        {
            blinkTime = 0.0F;
        }

        blinkInterval -= dt.asSeconds();
        if(blinkInterval <= 0.0F)
        {
            blinking = !blinking;
            blinkInterval = BLINK_INTERVAL;
        }
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
        if(!getContext().scontext->serverIP.empty())
            getContext().scontext->serverIP.pop_back();
        IPText.setString(getContext().scontext->serverIP);
        Utility::centerTextOrigin(IPText);
    }

    return false;
}
