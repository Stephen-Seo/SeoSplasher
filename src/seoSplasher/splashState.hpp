
#ifndef SPLASH_STATE_HPP
#define SPLASH_STATE_HPP

#include "../state.hpp"

#include <random>
#include <memory>

#include "cPlayerControl.hpp"
#include "cPowerup.hpp"
#include "direction.hpp"
#include "../gui.hpp"

class SplashServer;
class SplashClient;

struct cPathFinder;

class SplashState : public State
{
public:
    SplashState(StateStack& stack, Context context);

    void draw();
    bool update(sf::Time dt);
    bool handleEvent(const sf::Event& event);
private:
    sf::RectangleShape fieldBG;

    unsigned char IDcounter;

    Direction::Direction dir;
    bool placeBalloon;
    bool placeAction;
    bool kick;
    bool kickAction;

    bool wUp, aLeft, sDown, dRight;

    bool cFired;

    cPathFinder* cpf;

    std::unique_ptr<SplashServer> server;
    std::unique_ptr<SplashClient> client;

    std::map<int, int> playerIDToEntityID;

    GuiSystem guiSystem;
    bool startPressed;

    void addWall(float x, float y);
    void addCombatant(bool isPlayer, bool isPlayerLocallyControlled, int forceID = -1);
    void addPathFinder();

    void checkReleasedInput();
    void addBreakable(float x, float y, cPowerup::Powerup powerup = cPowerup::NONE);
    void initBreakables();
    bool validBreakableCoordinate(int x, int y);
};

#endif
