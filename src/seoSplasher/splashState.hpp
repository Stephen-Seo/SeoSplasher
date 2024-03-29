
#ifndef SPLASH_STATE_HPP
#define SPLASH_STATE_HPP

#include "../state.hpp"

#include <random>
#include <memory>
#include <set>

#include "cPlayerControl.hpp"
#include "cPowerup.hpp"
#include "direction.hpp"
#include "../gui.hpp"
#include "soundEventManager.hpp"

class SplashServer;
class SplashClient;

struct cPathFinder;

class SplashState : public State
{
public:
    SplashState(StateStack& stack, Context context);

    void draw() override;
    bool update(sf::Time dt) override;
    bool handleEvent(const sf::Event& event) override;
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
    int controllingPlayerID;

    GuiSystem guiSystem;
    bool startPressed;

    sf::Text statusText;
    sf::Text countdownText;
    sf::Text playerInfoText;
    int countdownNumber;
    sf::RectangleShape statusBG;

    SoundEventManager sem;

    SS::GameState prevState;

    void addWall(float x, float y);
    void addCombatant(bool isPlayer, bool isPlayerLocallyControlled, int forceID = -1);
    void addPathFinder();

    void checkReleasedInput();
    void addBreakable(float x, float y, cPowerup::Powerup powerup = cPowerup::NONE);
    void initBreakables();
    bool validBreakableCoordinate(int x, int y);

    void setStatusText();
    void reset();
};

#endif
