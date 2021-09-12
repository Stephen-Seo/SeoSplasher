
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
#include "nCControl.hpp"
#include "cClientControl.hpp"
#include "utility.hpp"
#include "soundContext.hpp"

SplashState::SplashState(StateStack& stack, Context context) :
State(stack, context),
fieldBG(sf::Vector2f(480.0F,480.0F)),
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
cpf(nullptr),
controllingPlayerID(0),
guiSystem(this),
startPressed(false),
countdownNumber(4),
prevState(SS::GameState::WAITING_FOR_PLAYERS)
{
    // init server context in case of resetting of this state
    for(int i = 0; i < 4; ++i)
    {
        context.scontext->playersAlive[i] = false;
        context.scontext->ppositions[i] = nullptr;
        context.scontext->pvelocities[i] = nullptr;
        context.scontext->movementTime[i] = 0.0F;
        context.scontext->input[i] = 0;
        context.scontext->customNames[i] = "";
        context.scontext->powerupPickedup[i] = false;
    }
    context.scontext->balloons.clear();
    context.scontext->explosions.clear();
    context.scontext->explosionXYToEID.clear();
    context.scontext->powerups.clear();
    context.scontext->powerupXYToEID.clear();
    context.scontext->breakables.clear();
    context.scontext->breakableXYToEID.clear();
    context.scontext->breakablesSet = false;
    context.scontext->gameState = SS::WAITING_FOR_PLAYERS;

    if(*context.mode == 0)
    {
        context.scontext->startTimer = 2.99F;
        startPressed = true;
    }
    else
    {
        context.scontext->startTimer = -1.0F;
    }

    // set servers based on mode

    if(*context.mode != 0 && *context.mode != 1) // is server
    {
        server = std::unique_ptr<SplashServer>(new SplashServer(context));
    }
    else if(*context.mode == 1) // is client
    {
        client = std::unique_ptr<SplashClient>(new SplashClient(context));
    }

    // seed random generator
    {
        std::seed_seq seq{std::random_device()(),(unsigned int)std::time(nullptr)};
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

        fset.insert(Fonts::CLEAR_SANS);

        sset.insert(Sound::BREAKABLE);
        sset.insert(Sound::COUNTDOWN_0);
        sset.insert(Sound::COUNTDOWN_1);
        sset.insert(Sound::DEATH);
        sset.insert(Sound::KICK);
        sset.insert(Sound::SPLOSION);
        sset.insert(Sound::POWERUP);
        sset.insert(Sound::TRY_AGAIN_TUNE);
        sset.insert(Sound::VICTORY_TUNE);

        context.resourceManager->loadResources(getNeededResources());
    }

    // add systems

    context.ecEngine->addSystem(std::unique_ptr<Node>(new nPControl()));
    context.ecEngine->addSystem(std::unique_ptr<Node>(new nMove()));

    if(!client)
    {
        if(*context.mode != 0) // not singleplayer)
        {
            context.ecEngine->addSystem(std::unique_ptr<Node>(new nCControl()));
        }
        context.ecEngine->addSystem(std::unique_ptr<Node>(new nAIControl()));
        context.ecEngine->addSystem(std::unique_ptr<Node>(new nBalloon()));
        context.ecEngine->addSystem(std::unique_ptr<Node>(new nSplosion()));
        context.ecEngine->addSystem(std::unique_ptr<Node>(new nBreakable()));
        context.ecEngine->addSystem(std::unique_ptr<Node>(new nDeath()));
        context.ecEngine->addSystem(std::unique_ptr<Node>(new nPickupHit()));
        context.ecEngine->addSystem(std::unique_ptr<Node>(new nPFUpdater()));
    }

    if(*context.mode != 4) // is not no-draw mode
    {
        context.ecEngine->addSystem(std::unique_ptr<Node>(new nAnimated()));
        context.ecEngine->addDrawSystem(std::unique_ptr<Node>(new nDraw()));
    }

    // init playerInfo text
    playerInfoText.setFont(context.resourceManager->getFont(Fonts::CLEAR_SANS));
    playerInfoText.setCharacterSize(40);
    if(*context.mode == 0)
        playerInfoText.setPosition(360.0F, 360.0F);
    else
        playerInfoText.setPosition(360.0F, 240.0F);

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
    else if(*context.mode == 2) // server multiplayer
    {
        addCombatant(true, true);
    }

    if(!client)
    {
        if(server)
        {
            context.scontext->breakables.reserve(100);

            // register server callbacks
            server->registerConnectionMadeCall( [this] (sf::Uint8 playerID) {
                // add player entity on connect
                this->addCombatant(true, false, playerID);
            });

            server->registerConnectionLostCall( [this] (sf::Uint8 playerID) {
                // remove player entity on disconnect
                if(this->getContext().scontext->playersAlive[playerID])
                {
                    this->getContext().ecEngine->removeEntity(playerIDToEntityID[playerID]);
                }
            });
        }
        initBreakables();
    }

    // init fieldBG
    fieldBG.setFillColor(sf::Color(127,127,127));
    fieldBG.setPosition(120.0F, 0.0F);

    // init status
    statusText.setFont(context.resourceManager->getFont(Fonts::CLEAR_SANS));
    statusText.setCharacterSize(30);
    statusText.setPosition(360.0F, 50.0F);
    setStatusText();

    statusBG.setFillColor(sf::Color(0,0,0,127));
    statusBG.setSize(sf::Vector2f(600.0F, 400.0F));
    sf::FloatRect rect = statusBG.getLocalBounds();
    statusBG.setOrigin(rect.width / 2, rect.height / 2);
    statusBG.setPosition(360.0F, 240.0F);

    // init countdown text
    countdownText.setFont(context.resourceManager->getFont(Fonts::CLEAR_SANS));
    countdownText.setCharacterSize(70);
    countdownText.setPosition(360.0F, 240.0F);



    // init GUI
    /*
    if(*context.mode != 1) // not client
    {
        sf::Text startText(
        GuiButton startButton(context.window, GuiCommand(GuiCommand::VALUE_BOOL, GuiCommand::Value(false), GuiCommand::Ptr(&startPressed)), sf::Color::Green, sf::Color(127,255,127), sf::Vector2f(200.0F, 90.0F), 
    }
    */

    // register client callbacks
    if(client)
    {
        client->registerPlayersChangedCall( [this] (sf::Uint8 playerInfo) {
            if(!playerIDToEntityID.empty())
            {
                std::map<int, int> pIDToEIDCopy = playerIDToEntityID;
                for(auto & iter : pIDToEIDCopy)
                {
                    this->getContext().ecEngine->removeEntity(iter.second);
                }
                playerIDToEntityID.clear();
            }

            for(int i = 0; i < 4; ++i)
            {
                if((playerInfo & (0x10 << i)) != 0x0)
                {
                    addCombatant(true, true, i);
                }
                else if(i < (playerInfo & 0xF))
                    addCombatant(true, false, i);
                else
                    addCombatant(false, false, i);
            }
        });

        client->registerGameRestartedCall( [this] () {
            this->reset();
        });
    }

    // get client to connect to server
    if(*context.mode == 1)
    {
        client->connectToServer(sf::IpAddress(context.scontext->serverIP));
    }
}

void SplashState::draw()
{
    if(*getContext().mode != 4) // not no-draw mode
    {
        getContext().window->draw(fieldBG);

        getContext().ecEngine->draw(getContext());

        if(getContext().scontext->gameState != SS::STARTED)
        {
            getContext().window->draw(statusBG);

            if(getContext().scontext->startTimer > 0.0F)
            {
                getContext().window->draw(countdownText);
            }
            else if(*getContext().mode != 0 || getContext().scontext->gameState == SS::ENDED)
            {
                getContext().window->draw(statusText);
            }

            if(((getContext().scontext->gameState == SS::WAITING_FOR_PLAYERS || getContext().scontext->gameState == SS::WAITING_FOR_SERVER) && (*getContext().mode == 0 || getContext().scontext->startTimer < 0.0F)) || (getContext().scontext->gameState == SS::ENDED))
            {
                getContext().window->draw(playerInfoText);
            }
        }
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

    if(prevState != getContext().scontext->gameState)
    {
        if(client && getContext().scontext->gameState == SS::ENDED)
        {
            playerInfoText.setString("Waiting for server to restart...");
            Utility::centerTextOrigin(playerInfoText);
        }
        prevState = getContext().scontext->gameState;
        setStatusText();
    }

    if(getContext().scontext->gameState == SS::STARTED)
    {
        getContext().ecEngine->update(dt, getContext());
        if((server || *getContext().mode == 0) && playerIDToEntityID.size() <= 1)
        {
            getContext().scontext->gameState = SS::ENDED;

            if(playerIDToEntityID.find(0) != playerIDToEntityID.end())
            {
                getContext().sfxContext->happened[SoundContext::GAME_ENDED_WELL] = true;
            }
            else
            {
                getContext().sfxContext->happened[SoundContext::GAME_ENDED_BADLY] = true;
            }

            playerInfoText.setString("Press Enter to restart the game");
            Utility::centerTextOrigin(playerInfoText);
        }
        else if(client && countdownText.getString().getSize() > 0)
        {
            countdownText.setString("");
        }
    }
    else if(getContext().scontext->startTimer >= 0.0F && (getContext().scontext->gameState == SS::WAITING_FOR_PLAYERS || getContext().scontext->gameState == SS::WAITING_FOR_SERVER))
    {
        if(server || *getContext().mode == 0)
            getContext().scontext->startTimer -= dt.asSeconds();

        if(getContext().scontext->startTimer >= 0.0F)
        {
            if(server || *getContext().mode == 0)
            {
                if(countdownNumber != (int)(getContext().scontext->startTimer + 1.0F))
                    getContext().sfxContext->happened[SoundContext::COUNTDOWN_COUNTED] = true;
                countdownNumber = (getContext().scontext->startTimer + 1.0F);
                countdownText.setString(std::to_string(countdownNumber));
            }
            else if(client)
            {
                if(countdownNumber != (int)(getContext().scontext->startTimer))
                    getContext().sfxContext->happened[SoundContext::COUNTDOWN_COUNTED] = true;
                countdownNumber = getContext().scontext->startTimer;
                countdownText.setString(std::to_string(countdownNumber));
            }
            Utility::centerTextOrigin(countdownText);
        }
        else
        {
            getContext().sfxContext->happened[SoundContext::COUNTDOWN_ENDED] = true;
            startPressed = false;
            getContext().scontext->gameState = SS::STARTED;
            countdownText.setString("");
        }
    }

    sem.update(dt, getContext());

    return false;
}

bool SplashState::handleEvent(const sf::Event& event)
{
    if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::W)
    {
        if(client)
        {
            getContext().scontext->input[controllingPlayerID] |= 0x2;
        }
        else
        {
            dir = Direction::UP;
            wUp = true;
        }
    }
    else if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::A)
    {
        if(client)
        {
            getContext().scontext->input[controllingPlayerID] |= 0x8;
        }
        else
        {
            dir = Direction::LEFT;
            aLeft = true;
        }
    }
    else if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::S)
    {
        if(client)
        {
            getContext().scontext->input[controllingPlayerID] |= 0x4;
        }
        else
        {
            dir = Direction::DOWN;
            sDown = true;
        }
    }
    else if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::D)
    {
        if(client)
        {
            getContext().scontext->input[controllingPlayerID] |= 0x1;
        }
        else
        {
            dir = Direction::RIGHT;
            dRight = true;
        }
    }
    else if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::E)
    {
        cFired = true;
        if(client)
        {
            getContext().scontext->input[controllingPlayerID] |= 0x20;
        }
    }
    else if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space)
    {
        if(client)
        {
            getContext().scontext->input[controllingPlayerID] |= 0x10;
        }
        else
        {
            placeBalloon = true;
            placeAction = false;
        }
    }
    else if(event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::W)
    {
        wUp = false;
        checkReleasedInput();
        if(client)
        {
            getContext().scontext->input[controllingPlayerID] &= 0xFD;
        }
    }
    else if(event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::A)
    {
        aLeft = false;
        checkReleasedInput();
        if(client)
        {
            getContext().scontext->input[controllingPlayerID] &= 0xF7;
        }
    }
    else if(event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::S)
    {
        sDown = false;
        checkReleasedInput();
        if(client)
        {
            getContext().scontext->input[controllingPlayerID] &= 0xFB;
        }
    }
    else if(event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::D)
    {
        dRight = false;
        checkReleasedInput();
        if(client)
        {
            getContext().scontext->input[controllingPlayerID] &= 0xFE;
        }
    }
    else if(event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::E)
    {
        cFired = false;
        if(client)
        {
            getContext().scontext->input[controllingPlayerID] &= 0xDF;
        }
    }
    else if(event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Space)
    {
        if(client)
        {
            getContext().scontext->input[controllingPlayerID] &= 0xEF;
        }
    }
    else if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
    {
        getContext().ecEngine->clear();
        requestStackClear();
        requestStackPush(States::MENU);
    }
    else if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Return)
    {
        if(!client && !startPressed && (getContext().scontext->gameState == SS::WAITING_FOR_PLAYERS || getContext().scontext->gameState == SS::WAITING_FOR_SERVER))
        {
            getContext().scontext->startTimer = 2.99F;
            startPressed = true;

            if(*getContext().mode != 0)
            {
                if(playerIDToEntityID.find(0) == playerIDToEntityID.end())
                {
                    addCombatant(false, false, 0);
                }
                if(playerIDToEntityID.find(1) == playerIDToEntityID.end())
                {
                    addCombatant(false, false, 1);
                }
                if(playerIDToEntityID.find(2) == playerIDToEntityID.end())
                {
                    addCombatant(false, false, 2);
                }
                if(playerIDToEntityID.find(3) == playerIDToEntityID.end())
                {
                    addCombatant(false, false, 3);
                }
            }
        }
        else if(!client && getContext().scontext->gameState == SS::ENDED)
        {
            reset();
        }
    }
    return false;
}

void SplashState::addWall(float x, float y)
{
    Entity* wall = new Entity();

    cPosition* pos = new cPosition();
    pos->x = x;
    pos->y = y;
    wall->addComponent(std::type_index(typeid(cPosition)), std::unique_ptr<Component>(pos));

    if(*getContext().mode != 4)
    {
        cSprite* sprite = new cSprite();
        sprite->sprite.setTexture(getContext().resourceManager->getTexture(Textures::WALL));
        wall->addComponent(std::type_index(typeid(cSprite)), std::unique_ptr<Component>(sprite));
    }

    cWall* cwall = new cWall();
    wall->addComponent(std::type_index(typeid(cWall)), std::unique_ptr<Component>(cwall));

    getContext().ecEngine->addEntity(std::unique_ptr<Entity>(wall));
}

void SplashState::addCombatant(bool isPlayer, bool isPlayerLocallyControlled, int forceID)
{
    unsigned int ID;
    if(forceID >= 0 && forceID < 4 && playerIDToEntityID.find(forceID) == playerIDToEntityID.end())
        ID = forceID;
    else
        ID = IDcounter++;

    std::unique_ptr<Entity> combatant = std::unique_ptr<Entity>(new Entity());

    std::unique_ptr<Component> posP = std::unique_ptr<Component>(new cPosition());
    cPosition* pos = static_cast<cPosition*>(posP.get());

    std::unique_ptr<Component> spriteP;
    if(*getContext().mode != 4)
        spriteP = std::unique_ptr<Component>(new cSprite());

    switch(ID)
    {
    case 0:
        pos->x = 120.0F;
        pos->y = 0.0F;
        pos->rot = 0.0F;
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
        pos->x = 568.0F;
        pos->y = 0.0F;
        pos->rot = 0.0F;
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
        pos->x = 120.0F;
        pos->y = 448.0F;
        pos->rot = 0.0F;
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
        pos->x = 568.0F;
        pos->y = 448.0F;
        pos->rot = 0.0F;
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

    std::unique_ptr<Component> velP = std::unique_ptr<Component>(new cVelocity());
    cVelocity* vel = static_cast<cVelocity*>(velP.get());
    vel->x = 0.0F;
    vel->y = 0.0F;
    vel->rot = 0.0F;
    combatant->addComponent(std::type_index(typeid(cVelocity)), std::move(velP));

    std::unique_ptr<Component> livingP = std::unique_ptr<Component>(new cLiving());
    cLiving* living = static_cast<cLiving*>(livingP.get());
    living->ID = ID;
    combatant->addComponent(std::type_index(typeid(cLiving)), std::move(livingP));

    if(isPlayer && isPlayerLocallyControlled)
    {
        if(!client)
            combatant->addComponent(std::type_index(typeid(cPlayerControl)), std::unique_ptr<Component>(new cPlayerControl(&dir, &placeBalloon, &placeAction, &kick, &kickAction, ID, &cFired)));
        controllingPlayerID = ID;
        if(server)
            playerInfoText.setString("You are Player " + std::to_string(ID + 1) + "!\nPress Enter to Start");
        else
            playerInfoText.setString("You are Player " + std::to_string(ID + 1) + "!");
        Utility::centerTextOrigin(playerInfoText);
    }
    else if(isPlayer && server)
    {
        combatant->addComponent(std::type_index(typeid(cClientControl)), std::unique_ptr<Component>(new cClientControl()));
    }
    else if(!isPlayer && !client)
    {
        std::unique_ptr<Component> controlP = std::unique_ptr<Component>(new cAIControl());
        cAIControl* control = static_cast<cAIControl*>(controlP.get());
        control->ID = ID;
        control->pf = &cpf->pf;
        combatant->addComponent(std::type_index(typeid(cAIControl)), std::move(controlP));
    }

    combatant->addComponent(std::type_index(typeid(cPathFinderRef)), std::unique_ptr<Component>(new cPathFinderRef(cpf)));

    getContext().ecEngine->addEntity(std::move(combatant));

    if(client || server)
    {
        // set ID
        getContext().scontext->playerEID[ID] = playerIDToEntityID[ID];

        // assign context variables
        getContext().scontext->ppositions[ID] = pos;
        getContext().scontext->pvelocities[ID] = vel;
        getContext().scontext->playersAlive[ID] = true;

        // set alive to false when player entity is removed
        getContext().ecEngine->registerRemoveCall(playerIDToEntityID[ID], [ID, this] () {
            this->getContext().scontext->playersAlive[ID] = false;
        });
    }

    // remove ID mapping when player is removed
    // trigger death sfx
    getContext().ecEngine->registerRemoveCall(playerIDToEntityID[ID], [this, ID] () {
        this->playerIDToEntityID.erase(ID);
        this->getContext().sfxContext->happened[SoundContext::COMBATANT_DIED] = true;
    });
}

void SplashState::addPathFinder()
{
    Entity* pfHolder = new Entity();

    cpf = new cPathFinder();
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
    Entity* breakable = new Entity();

    cPosition* pos = new cPosition();
    pos->x = x;
    pos->y = y;
    pos->rot = 0.0F;
    breakable->addComponent(std::type_index(typeid(cPosition)), std::unique_ptr<Component>(pos));

    cPowerup* cpowerup = new cPowerup();
    cpowerup->powerup = powerup;
    breakable->addComponent(std::type_index(typeid(cPowerup)), std::unique_ptr<Component>(cpowerup));

    breakable->addComponent(std::type_index(typeid(cBreakable)), std::unique_ptr<Component>(new cBreakable()));

    cSprite* sprite = new cSprite();
    sprite->sprite.setTexture(getContext().resourceManager->getTexture(Textures::BREAKABLE));
    breakable->addComponent(std::type_index(typeid(cSprite)), std::unique_ptr<Component>(sprite));

    getContext().ecEngine->addEntity(std::unique_ptr<Entity>(breakable));

    if(server)
    {
        getContext().scontext->breakables.push_back((sf::Uint8)((x - (float)GRID_OFFSET_X) / GRID_SQUARE_SIZE) + ((sf::Uint8)((y - (float)GRID_OFFSET_Y) / GRID_SQUARE_SIZE) * GRID_WIDTH));
    }

    getContext().ecEngine->registerRemoveCall(breakable->getID(), [this] () {
        this->getContext().sfxContext->happened[SoundContext::BREAKABLE_BROKEN] = true;
    });
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
                validPoints.emplace_back(i,j);
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

void SplashState::setStatusText()
{
    switch(getContext().scontext->gameState)
    {
    case SS::WAITING_FOR_PLAYERS:
        statusText.setString("Waiting for players");
        Utility::centerTextOrigin(statusText);
        break;
    case SS::WAITING_FOR_SERVER:
        statusText.setString("Waiting for server");
        Utility::centerTextOrigin(statusText);
        break;
    case SS::STARTED:
        statusText.setString("");
        break;
    case SS::ENDED:
        statusText.setString("Game Over");
        Utility::centerTextOrigin(statusText);
        break;
    case SS::CONNECTION_LOST:
        statusText.setString("Connection Lost");
        Utility::centerTextOrigin(statusText);
    }
}

void SplashState::reset()
{
    getContext().scontext->gameState = SS::WAITING_FOR_PLAYERS;

    playerIDToEntityID.clear();

    getContext().ecEngine->clearEntities();

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

    if(!client)
    {
        getContext().scontext->balloons.clear();
        getContext().scontext->explosions.clear();
        getContext().scontext->powerups.clear();

        IDcounter = 0;
        if(*getContext().mode == 0) // local singleplayer
        {
            placeBalloon = false;

            addCombatant(true, true);
            addCombatant(false, false);
            addCombatant(false, false);
            addCombatant(false, false);

            startPressed = true;
            getContext().scontext->startTimer = 2.99F;
        }
        else if(*getContext().mode == 2) // server multiplayer
        {
            addCombatant(true, true);
        }
        getContext().scontext->breakables.clear();
        initBreakables();
    }
    else
        getContext().scontext->breakablesSet = false;
}
