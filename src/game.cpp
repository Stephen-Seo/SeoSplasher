
#include "game.hpp"

#include "seoSplasher/splashState.hpp"
#include "seoSplasher/splashMenu.hpp"

// set packfile name/filepath if one is being used
#define PACKFILE_NAME "seoSplasherResources.dat"

// set to true if a packfile is being used
#define IS_USING_PACKFILE true

// if not using cmake to build and using the ResourcePacker lib,
// define ResourcePacker_FOUND


#if defined(ResourcePacker_FOUND)
#else
#  define IS_USING_PACKFILE false
#endif

#if IS_USING_PACKFILE == true
#  define RESOURCE_MANAGER_MODE GameResources::PACKFILE
#else
#  define RESOURCE_MANAGER_MODE GameResources::DEFAULT
#endif

Game::Game()
: window(sf::VideoMode(720,480), "SeoSplasher"),
resourceManager(&stateStack, RESOURCE_MANAGER_MODE, PACKFILE_NAME),
mPlayer(),
sPlayer(),
stateStack(Context(window, resourceManager, mPlayer, sPlayer, ecEngine, rGen, isQuitting, mode, scontext, sfxContext)),
isQuitting(false),
rGen(std::random_device{}()),
mode(0)
{
    registerResources();
    registerStates();

    frameTime = sf::seconds(1.F / 60.F);
    window.setFramerateLimit(60);
}

void Game::run()
{
    sf::Clock clock;
    sf::Time lastUpdateTime = sf::Time::Zero;
    while (window.isOpen() && !isQuitting)
    {
        lastUpdateTime += clock.restart();
        while (lastUpdateTime > frameTime)
        {
            lastUpdateTime -= frameTime;
            processEvents();
            update(frameTime);
        }
        draw();
    }

    if(window.isOpen())
        window.close();
}

void Game::processEvents()
{
    sf::Event event;
    while (window.pollEvent(event))
    {
        stateStack.handleEvent(event);
        if(event.type == sf::Event::Closed)
            window.close();
        else if(event.type == sf::Event::Resized)
        {
//            window.setSize(sf::Vector2u(event.size.width, event.size.height));
            auto view = sf::View(sf::FloatRect(0.0F, 0.0F, 720.0F, 480.0F));
            window.setView(view);
        }
    }
}

void Game::update(sf::Time deltaTime)
{
    stateStack.update(deltaTime);
}

void Game::draw()
{
    window.clear();
    stateStack.draw();
    window.display();
}

// register resources via resourceManager
// Resource IDs must be listed in resourceIdentifiers.hpp
void Game::registerResources()
{
    resourceManager.registerTexture(Textures::WALL, "wall.png");
    resourceManager.registerTexture(Textures::BREAKABLE, "breakable.png");
    resourceManager.registerTexture(Textures::BALLOON_0, "balloon00.png");
    resourceManager.registerTexture(Textures::BALLOON_1, "balloon01.png");
    resourceManager.registerTexture(Textures::BALLOON_2, "balloon02.png");
    resourceManager.registerTexture(Textures::SUPER_BALLOON_0, "superBalloon00.png");
    resourceManager.registerTexture(Textures::SUPER_BALLOON_1, "superBalloon01.png");
    resourceManager.registerTexture(Textures::SUPER_BALLOON_2, "superBalloon02.png");
    resourceManager.registerTexture(Textures::C_BALLOON_0, "controlledBalloon00.png");
    resourceManager.registerTexture(Textures::C_BALLOON_1, "controlledBalloon01.png");
    resourceManager.registerTexture(Textures::C_BALLOON_2, "controlledBalloon02.png");
    resourceManager.registerTexture(Textures::C_SUPER_BALLOON_0, "controlledSuperBalloon00.png");
    resourceManager.registerTexture(Textures::C_SUPER_BALLOON_1, "controlledSuperBalloon01.png");
    resourceManager.registerTexture(Textures::C_SUPER_BALLOON_2, "controlledSuperBalloon02.png");
    resourceManager.registerTexture(Textures::PLAYER_ONE, "player1.png");
    resourceManager.registerTexture(Textures::PLAYER_TWO, "player2.png");
    resourceManager.registerTexture(Textures::PLAYER_THREE, "player3.png");
    resourceManager.registerTexture(Textures::PLAYER_FOUR, "player4.png");
    resourceManager.registerTexture(Textures::C_PLAYER_ONE, "computer1.png");
    resourceManager.registerTexture(Textures::C_PLAYER_TWO, "computer2.png");
    resourceManager.registerTexture(Textures::C_PLAYER_THREE, "computer3.png");
    resourceManager.registerTexture(Textures::C_PLAYER_FOUR, "computer4.png");
    resourceManager.registerTexture(Textures::SPLOSION_PLUS, "splosionPLUS.png");
    resourceManager.registerTexture(Textures::SPLOSION_VERT, "splosionVERT.png");
    resourceManager.registerTexture(Textures::SPLOSION_HORIZ, "splosionHORIZ.png");
    resourceManager.registerTexture(Textures::BALLOON_UP_0, "balloonUp00.png");
    resourceManager.registerTexture(Textures::BALLOON_UP_1, "balloonUp01.png");
    resourceManager.registerTexture(Textures::BALLOON_UP_2, "balloonUp02.png");
    resourceManager.registerTexture(Textures::RANGE_UP_0, "rangeUp00.png");
    resourceManager.registerTexture(Textures::RANGE_UP_1, "rangeUp01.png");
    resourceManager.registerTexture(Textures::RANGE_UP_2, "rangeUp02.png");
    resourceManager.registerTexture(Textures::SPEED_UP_0, "speedUp00.png");
    resourceManager.registerTexture(Textures::SPEED_UP_1, "speedUp01.png");
    resourceManager.registerTexture(Textures::SPEED_UP_2, "speedUp02.png");
    resourceManager.registerTexture(Textures::KICK_UPGRADE_0, "kickUpgrade00.png");
    resourceManager.registerTexture(Textures::KICK_UPGRADE_1, "kickUpgrade01.png");
    resourceManager.registerTexture(Textures::KICK_UPGRADE_2, "kickUpgrade02.png");
    resourceManager.registerTexture(Textures::RCONTROL_UPGRADE_0, "rControl00.png");
    resourceManager.registerTexture(Textures::RCONTROL_UPGRADE_1, "rControl01.png");
    resourceManager.registerTexture(Textures::RCONTROL_UPGRADE_2, "rControl02.png");
    resourceManager.registerTexture(Textures::SBALLOON_UPGRADE_0, "superBalloonUp00.png");
    resourceManager.registerTexture(Textures::SBALLOON_UPGRADE_1, "superBalloonUp01.png");
    resourceManager.registerTexture(Textures::SBALLOON_UPGRADE_2, "superBalloonUp02.png");
    resourceManager.registerTexture(Textures::PIERCE_UPGRADE_0, "pierceUp00.png");
    resourceManager.registerTexture(Textures::PIERCE_UPGRADE_1, "pierceUp01.png");
    resourceManager.registerTexture(Textures::PIERCE_UPGRADE_2, "pierceUp02.png");
    resourceManager.registerTexture(Textures::SPREAD_UPGRADE_0, "spreadUp00.png");
    resourceManager.registerTexture(Textures::SPREAD_UPGRADE_1, "spreadUp01.png");
    resourceManager.registerTexture(Textures::SPREAD_UPGRADE_2, "spreadUp02.png");
    resourceManager.registerTexture(Textures::GHOST_UPGRADE_0, "ghostUp00.png");
    resourceManager.registerTexture(Textures::GHOST_UPGRADE_1, "ghostUp01.png");
    resourceManager.registerTexture(Textures::GHOST_UPGRADE_2, "ghostUp02.png");

    resourceManager.registerFont(Fonts::CLEAR_SANS, "ClearSans-Regular.ttf");

    resourceManager.registerSoundBuffer(Sound::BREAKABLE, "breakable.ogg");
    resourceManager.registerSoundBuffer(Sound::COUNTDOWN_0, "countdown0.ogg");
    resourceManager.registerSoundBuffer(Sound::COUNTDOWN_1, "countdown1.ogg");
    resourceManager.registerSoundBuffer(Sound::DEATH, "death.ogg");
    resourceManager.registerSoundBuffer(Sound::KICK, "kick.ogg");
    resourceManager.registerSoundBuffer(Sound::SPLOSION, "splosion.ogg");
    resourceManager.registerSoundBuffer(Sound::POWERUP, "powerup.ogg");
    resourceManager.registerSoundBuffer(Sound::TRY_AGAIN_TUNE, "tryagain.ogg");
    resourceManager.registerSoundBuffer(Sound::VICTORY_TUNE, "victory.ogg");
}

// register states via stateStack
// State IDs must be listed in stateIdentifiers.hpp
void Game::registerStates()
{
    stateStack.registerState<SplashState>(States::SPLASH);
    stateStack.registerState<SplashMenu>(States::MENU);

    stateStack.pushState(States::MENU);
}
